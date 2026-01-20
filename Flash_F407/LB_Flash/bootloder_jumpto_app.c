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


#include "usbd_core.h"

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

    USBD_DeInit(&hUsbDeviceFS);  // 1. De-initialize USB (Critical! If skipped, the App will crash during its USB re-init)
    HAL_RCC_DeInit();            // 2. De-initialize RCC (Reset system clock to default HSI)
    HAL_DeInit();                // 3. De-initialize the HAL library (Reset all peripherals)
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

