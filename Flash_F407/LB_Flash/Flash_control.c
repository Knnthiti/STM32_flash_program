/*
 * Flash_control.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */

#include "Flash_control.h"
#include "stdint.h"
#include "stm32f4xx_it.h"

//void store_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length){
//	uint8_t data_temp[8];
//	HAL_FLASH_Unlock();
//	int pointer_element = 0;
//	while (data_length > 8){
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, memory_address + pointer_element, *((uint64_t *)&data[pointer_element]));
//
//		pointer_element += 8;
//		data_length -= 8;
//	}
//	if(data_length > 0){
//		for(int i = 0 ; i < data_length ; i++){
//			data_temp[i] = data[pointer_element + i];
//		}
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, memory_address + pointer_element, *((uint64_t *)&data_temp));
//	}
//	HAL_FLASH_Lock();
//}

void store_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length)
{
    HAL_FLASH_Unlock();
    int pointer = 0;

    // 1. เขียนทีละ 4 Bytes (Word)
    // แก้เงื่อนไขเป็น >= 4 และขยับทีละ 4
    while (data_length >= 4)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          *((uint32_t *)&data[pointer])); // แก้เป็น uint32_t

        pointer += 4;      // ขยับทีละ 4
        data_length -= 4;  // ลดทีละ 4
    }

    // 2. เก็บตกเศษที่เหลือ (1, 2, หรือ 3 Bytes)
    if (data_length > 0)
    {
        // สร้างตัวแปร 4 bytes มารองรับ (สำคัญ: ต้องเติม 0xFF รอไว้)
        uint32_t last_word = 0xFFFFFFFF;

        // Copy เศษข้อมูลใส่ลงไปทีละ byte
        for (int i = 0; i < data_length; i++)
        {
            // Shift ข้อมูลเข้าไป (Little Endian)
            // หรือใช้ pointer cast แบบง่ายๆ:
            ((uint8_t*)&last_word)[i] = data[pointer + i];
        }

        // เขียนก้อนสุดท้ายปิดท้าย
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          memory_address + pointer,
                          last_word);
    }

    HAL_FLASH_Lock();
}
