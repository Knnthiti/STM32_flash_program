/*
 * Flash_control.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */

#include "Flash_control.h"
#include "stdint.h"
#include "stm32f4xx_it.h"

void store_flash_memory(uint32_t memory_address, uint8_t *data, uint16_t data_length){
	uint8_t data_temp[8];
	HAL_FLASH_Unlock();
	int pointer_element = 0;
	while (data_length > 8){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, memory_address + pointer_element, *((uint64_t *)&data[pointer_element]));

		pointer_element += 8;
		data_length -= 8;
	}
	if(data_length > 0){
		for(int i = 0 ; i < data_length ; i++){
			data_temp[i] = data[pointer_element + i];
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, memory_address + pointer_element, *((uint64_t *)&data_temp));
	}
	HAL_FLASH_Lock();
}
