/*
 * Flash_control.c
 *
 * Created on: Jan 10, 2026
 * Author: Knnn
 */

#include "Flash_control.h"
#include "stdint.h"
#include "stm32f4xx_it.h"


void store_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length)
{
    HAL_FLASH_Unlock();
    int pointer = 0;

    // 1. Write data in 4-Byte chunks (Word Access)
    // Loop while there are at least 4 bytes remaining
    while (data_length >= 4)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          *((uint32_t *)&data[pointer])); // Typecast to uint32_t to write a full word

        pointer += 4;      // Increment offset by 4
        data_length -= 4;  // Decrease remaining length by 4
    }

    // 2. Handle remaining bytes (1, 2, or 3 bytes)
    // This handles the "Tail" if the data size is not divisible by 4
    if (data_length > 0)
    {
        // Create a temporary 4-byte container.
        // IMPORTANT: Initialize with 0xFFFFFFFF because Flash bits can only be programmed from 1 to 0.
        // Leaving it as 0xFF ensures unwritten bytes remain in their erased state.
        uint32_t last_word = 0xFFFFFFFF;

        // Copy the remaining bytes into the temporary variable
        for (int i = 0; i < data_length; i++)
        {
            // Map bytes directly into the 32-bit variable (Preserves Endianness)
            ((uint8_t*)&last_word)[i] = data[pointer + i];
        }

        // Program the final word containing the remaining data + padding
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          last_word);
    }

    HAL_FLASH_Lock();
}