/*
 * bootloader.h
 *
 *  Created on: Dec 19, 2025
 *      Author: Knnn
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
#include "main.h"
#include "string.h"
#include "stm32f4xx_hal.h"

// --- Config for STM32F407 ---
// Sector 2 starts at 0x08008000
#define APP1_START      0x0800A800
// Sector 6 starts at 0x08040000 (สมมติว่า App2 อยู่ตรงนี้ หรือไม่ได้ใช้)
#define APP2_START      0x08040000

// Command Strings (ตาม Code เดิมของคุณ)
#define ERASE_FLASH_MEMORY "ERASE_FLASH_MEMORY"
#define FLASHING_START     "FLASHING_START"
#define FLASHING_FINISH    "FLASHING_FINISH"
#define FLASHING_ABORT     "FLASHING_ABORT"

typedef enum {
    Unerased,
    Erased,
    Unlocked,
    Locked
} FlashStatus;

typedef enum {
    FlashMode,
    JumpMode
} BootloaderMode;

typedef enum {
    App1,
    App2
} AppSelection;

// Struct for Jump
typedef struct {
    uint32_t stack_addr;
    uint32_t func_p;
} JumpStruct;

// Global Variables
extern uint32_t Flashed_offset;
extern FlashStatus flashStatus;
extern AppSelection App;

// Functions
void bootloaderInit(void);
void flashWord(uint32_t dataToFlash);
void eraseMemory(void);
void unlockFlashAndEraseMemory(void);
void lockFlash(void);
void jumpToApp(const uint32_t address);
void errorBlink(void);
void messageHandler(uint8_t* Buf);
uint32_t readWord(uint32_t address);

#endif /* BOOTLOADER_H_ */
