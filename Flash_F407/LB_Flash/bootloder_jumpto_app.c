/*
 * bootloder_jumpto_app.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Knnn
 */
#include "main.h"
#include "stdint.h"
#include "stm32f4xx_it.h"
#include "bootloder_jumpto_app.h"

/* --- เพิ่มตรงนี้ --- */
#include "usbd_core.h"
/* ----------------- */

extern USBD_HandleTypeDef hUsbDeviceFS;

#define BL_START_ADDR        0x08000000
#define APP_START_ADDR       0x0800C000

typedef void (*pFunction)(void);

void JumpToApplication(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;
    pFunction appEntry;

    /* Read application stack pointer */
    appStack = *(volatile uint32_t*)APP_START_ADDR;

    /* Read reset handler address */
    appResetHandler = *(volatile uint32_t*)(APP_START_ADDR + 4);
    appEntry = (pFunction)appResetHandler;

    USBD_DeInit(&hUsbDeviceFS);  // 1. ปิด USB (สำคัญมาก! ถ้าไม่ปิด App จะพังตอน Init USB ซ้ำ)
    HAL_RCC_DeInit();            // 2. คืนค่า Clock ให้เป็น Default (HSI)
    HAL_DeInit();                // 3. ยกเลิกการ Init ของ HAL ทั้งหมด
    /* Disable interrupts */
    __disable_irq();

    /* Stop SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Set main stack pointer */
    __set_MSP(appStack);

    /* Jump to application reset handler */
    appEntry();
}

//void JumpToApplication(void)
//{
//    uint32_t appStack;
//    uint32_t appResetHandler;
//    pFunction appEntry;
//
//    /* 1. อ่านค่า Stack Pointer และ Reset Handler จาก Flash */
//    appStack = *(volatile uint32_t*)APP_START_ADDR;
//    appResetHandler = *(volatile uint32_t*)(APP_START_ADDR + 4);
//    appEntry = (pFunction)appResetHandler;
//
//    /* ------------------------------------------------------------------
//       [SAFETY CHECK] ตรวจสอบว่าค่า Stack อยู่ใน RAM หรือไม่ (STM32F4 RAM เริ่มที่ 0x20000000)
//       ถ้า Flash ว่างเปล่า (0xFFFFFFFF) แล้วเรากระโดดไป = HardFault แน่นอน
//       ------------------------------------------------------------------ */
//    if ((appStack & 0x2FF00000) != 0x20000000)
//    {
//        // ข้อมูลไม่ถูกต้อง! (อาจจะกระพริบไฟแดงเตือนตรงนี้)
//        return;
//    }
//
//    /* ------------------------------------------------------------------
//       [STEP 1] ปิด Peripherals ระดับ High-Level ก่อน
//       ------------------------------------------------------------------ */
//    USBD_DeInit(&hUsbDeviceFS);  // ปิด USB Driver
//    HAL_Delay(100);              // รอให้ USB ตัดการเชื่อมต่อชัวร์ๆ (Optional)
//
//    /* ------------------------------------------------------------------
//       [STEP 2] ปิด Interrupt ทั้งหมดทันที! (สำคัญมากต้องทำตรงนี้)
//       เพื่อป้องกัน Interrupt แทรกขณะกำลังเคลียร์ Clock
//       ------------------------------------------------------------------ */
//    __disable_irq();
//
//    /* ------------------------------------------------------------------
//       [STEP 3] เคลียร์ System Core และ Clock
//       ------------------------------------------------------------------ */
//    SysTick->CTRL = 0;
//    SysTick->LOAD = 0;
//    SysTick->VAL  = 0;
//
//    HAL_RCC_DeInit();            // คืนค่า Clock เป็น Default (HSI)
//    HAL_DeInit();                // ยกเลิก HAL
//
//    // (เพิ่มเติม) เคลียร์ Pending Interrupt ที่อาจค้างอยู่
//    for (int i = 0; i < 8; i++)
//    {
//        NVIC->ICER[i] = 0xFFFFFFFF;
//        NVIC->ICPR[i] = 0xFFFFFFFF;
//    }
//
//    /* ------------------------------------------------------------------
//       [STEP 4] ตั้งค่า Stack และ Jump
//       ------------------------------------------------------------------ */
//    __set_MSP(appStack);
//
//    // [IMPORTANT] เพิ่ม 2 บรรทัดนี้ เพื่อ Force ให้ CPU ใช้ Stack ใหม่ทันที
//    __DSB(); // Data Synchronization Barrier
//    __ISB(); // Instruction Synchronization Barrier
//
//    /* Go! */
//    appEntry();
//}
