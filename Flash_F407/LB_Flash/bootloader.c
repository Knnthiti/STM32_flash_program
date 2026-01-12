/*
 * bootloader.c
 *
 *  Created on: Dec 19, 2025
 *      Author: Knnn
 */

#include "bootloader.h"
#include "usbd_cdc_if.h" // เพื่อเรียกใช้ CDC_Transmit_FS

/*
 * bootloader.c
 * Adapted for STM32F407
 */

#include "bootloader.h"
#include "usbd_cdc_if.h" // ต้อง Include เพื่อใช้ CDC_Transmit

// Global Vars
uint32_t Flashed_offset = 0;
FlashStatus flashStatus = Unerased;
AppSelection App = App1;

// Helper Function for STM32F4
static uint32_t GetSector(uint32_t Address)
{
  if((Address < 0x08004000) && (Address >= 0x08000000)) return FLASH_SECTOR_0;
  if((Address < 0x08008000) && (Address >= 0x08004000)) return FLASH_SECTOR_1;
  if((Address < 0x0800C000) && (Address >= 0x08008000)) return FLASH_SECTOR_2;
  if((Address < 0x08010000) && (Address >= 0x0800C000)) return FLASH_SECTOR_3;
  if((Address < 0x08020000) && (Address >= 0x08010000)) return FLASH_SECTOR_4;
  if((Address < 0x08040000) && (Address >= 0x08020000)) return FLASH_SECTOR_5;
  if((Address < 0x08060000) && (Address >= 0x08040000)) return FLASH_SECTOR_6;
  if((Address < 0x08080000) && (Address >= 0x08060000)) return FLASH_SECTOR_7;
  // F407 มีถึง Sector 11 ถ้าใช้เต็มความจุให้เพิ่มต่อไป
  return FLASH_SECTOR_7;
}

void bootloaderInit()
{
    Flashed_offset = 0;
    flashStatus = Unerased;
    BootloaderMode bootloaderMode;

    // เช็คขา BOOT1 (Button) หรือขา PA0 ที่คุณใช้
    // สมมติ: BOOT1_Pin คือ User Button (PA0) -> กด = SET
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        bootloaderMode = FlashMode;

        // ไฟกระพริบบอกว่าเข้า Bootloader
        for(uint8_t i=0; i<6; i++)
        {
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12); // LED Green
            HAL_Delay(50);
        }
        // เปิด USB (Code เดิมของคุณใช้วิธีเปิดไฟเลี้ยง USB, ของ F4 มัก Init USB เลย)
        // MX_USB_DEVICE_Init(); // เรียกใน main แล้ว หรือเรียกตรงนี้ก็ได้
    }
    else
    {
        bootloaderMode = JumpMode;
    }

    // Default App Selection
    App = App1;

    if(bootloaderMode == JumpMode)
    {
        uint32_t app_start_addr = (App == App1) ? APP1_START : APP2_START;

        // Check if application exists (First word is Stack Pointer, usually 0x200xxxxx)
        uint32_t stack_ptr = readWord(app_start_addr);
        if((stack_ptr & 0x2FFE0000) == 0x20000000)
        {
            jumpToApp(app_start_addr);
        }
        else
        {
            // ไม่มี App -> บังคับเข้าโหมด Flash หรือ Error Blink
            errorBlink();
        }
    }
}

void flashWord(uint32_t dataToFlash)
{
    if(flashStatus == Unlocked)
    {
        HAL_StatusTypeDef status;
        uint32_t address;

        if(App == App1) address = APP1_START + Flashed_offset;
        else            address = APP2_START + Flashed_offset;

        // F4 ใช้ Program Type WORD (32-bit)
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, dataToFlash);

        if(status != HAL_OK)
        {
            CDC_Transmit_FS((uint8_t*)"Flashing Error!\n", 16);
        }
        else
        {
            Flashed_offset += 4;
            // ไม่แนะนำให้ส่ง OK ทุก Word เพราะจะช้ามาก แต่ถ้า Code เดิมส่ง ก็ส่งตามนั้น
//             CDC_Transmit_FS((uint8_t*)"Flash: OK\n", 10);
        }
    }
    else
    {
        CDC_Transmit_FS((uint8_t*)"Error: Locked!\n", 15);
    }
}

uint32_t readWord(uint32_t address)
{
    return *(__IO uint32_t*)address;
}

void eraseMemory()
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;
    uint32_t startAddr = (App == App1) ? APP1_START : APP2_START;

    // --- แก้ไขสำหรับ F407 (Sector Erase) ---
    EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 2.7-3.6V
    EraseInitStruct.Sector       = GetSector(startAddr);

    // คำนวณจำนวน Sector ที่จะลบ (ตัวอย่าง: ลบตั้งแต่ Start ถึง End of Flash)
    // สมมติ App1 เริ่ม Sector 2 -> ลบถึง Sector 7 (หรือ 11)
    // 0x08008000 อยู่ Sector 2
    // Sector สุดท้ายคือ 7 (0x08060000)
    EraseInitStruct.NbSectors    = (FLASH_SECTOR_7 - EraseInitStruct.Sector) + 1;

    if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        errorBlink();
    }

//    HAL_FLASH_Lock();

    flashStatus = Erased;
    Flashed_offset = 0;
}

void unlockFlashAndEraseMemory()
{
    eraseMemory(); // เรียกใช้ฟังก์ชัน erase ที่แก้แล้ว
    flashStatus = Unlocked;
}

void lockFlash()
{
    HAL_FLASH_Lock();
    flashStatus = Locked;
}

void jumpToApp(const uint32_t address)
{
    // 1. De-Init Peripherals
    HAL_RCC_DeInit();
    HAL_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // 2. Disable Interrupts
    __disable_irq();

    // 3. Set Vector Table (สำคัญมากสำหรับ F4)
    SCB->VTOR = address;

    // 4. Jump
    const JumpStruct* vector_p = (JumpStruct*)address;

    // Set Main Stack Pointer
    __set_MSP(vector_p->stack_addr);

    // Jump to Reset Handler
    void (*pResetHandler)(void) = (void*)vector_p->func_p;
    pResetHandler();
}

uint8_t string_compare(char array1[], char array2[], uint16_t length)
{
    return (strncmp(array1, array2, length) == 0) ? 1 : 0;
}

void errorBlink()
{
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12); // LED Green
        HAL_Delay(100);
    }
}

void messageHandler(uint8_t* Buf)
{
    // 1. กรณีรับคำสั่ง Erase (ลบ Flash อย่างเดียว)
    if(string_compare((char*)Buf, ERASE_FLASH_MEMORY, strlen(ERASE_FLASH_MEMORY))
            && flashStatus != Unlocked)
    {
        eraseMemory();
        CDC_Transmit_FS((uint8_t*)"Flash: Erased!\n", 15);
    }
    // 2. กรณีรับคำสั่ง Start (ปลดล็อค + ลบข้อมูลเก่า)
    else if(string_compare((char*)Buf, FLASHING_START, strlen(FLASHING_START)))
    {
        unlockFlashAndEraseMemory();
        CDC_Transmit_FS((uint8_t*)"Flash: Unlocked!\n", 17);
    }
    // 3. กรณีรับคำสั่ง Finish (จบงาน + รีเซ็ต)
    else if(string_compare((char*)Buf, FLASHING_FINISH, strlen(FLASHING_FINISH))
             && flashStatus == Unlocked)
    {
        lockFlash(); // ล็อค Flash กลับคืน
        CDC_Transmit_FS((uint8_t*)"Flash: Success!\n", 16); // ส่ง ACK กลับไป

        // --- [ส่วนที่แก้ไข: Software Delay] ---
        // ใช้การวนลูปเปล่าๆ เพื่อถ่วงเวลาให้ USB ส่งข้อมูล "Success" ออกไปให้เสร็จก่อน
        // เราใช้ HAL_Delay ไม่ได้ เพราะมันจะค้าง (Deadlock) เมื่ออยู่ใน Interrupt ของ USB
        // Loop ประมาณ 5 ล้านรอบ จะถ่วงเวลาได้ประมาณ 100-200ms บน F407
        for(volatile uint32_t i = 0; i < 5000000; i++)
        {
            __NOP(); // คำสั่ง No Operation
        }
        // -----------------------------------

        NVIC_SystemReset(); // รีเซ็ตบอร์ดเพื่อเริ่มรันโปรแกรมใหม่
    }
    // 4. กรณีรับคำสั่ง Abort (ยกเลิกกลางคัน)
    else if(string_compare((char*)Buf, FLASHING_ABORT, strlen(FLASHING_ABORT))
             && flashStatus == Unlocked)
    {
        lockFlash();
        eraseMemory(); // ลบข้อมูลทิ้งเพื่อความปลอดภัย
        CDC_Transmit_FS((uint8_t*)"Flash: Aborted!\n", 16);
    }
    // 5. กรณีได้รับข้อมูลที่ไม่รู้จัก (Unknown Command)
    else
    {
        CDC_Transmit_FS((uint8_t*)"Error: Unknown!\n", 16);
    }
}

