/*
 * Flash_control.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */

#include "Flash_control.h"
#include "stdint.h"
#include "stm32f4xx_it.h"

void store_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length)
{
    HAL_FLASH_Unlock();
    int pointer = 0;

    while (data_length >= 4)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          *((uint32_t *)&data[pointer]));

        pointer += 4;
        data_length -= 4;
    }

    if (data_length > 0)
    {
        uint32_t last_word = 0xFFFFFFFF;

        for (int i = 0; i < data_length; i++)
        {
            ((uint8_t*)&last_word)[i] = data[pointer + i];
        }

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          last_word);
    }

    HAL_FLASH_Lock();
}
