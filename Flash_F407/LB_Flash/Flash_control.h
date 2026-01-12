/*
 * Flash_control.h
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */

#ifndef FLASH_CONTROL_H_
#define FLASH_CONTROL_H_

#include "stdio.h"
#include "main.h"

#define VERIFY_CRC_SUCCESS 0
#define VERIFY_CRC_FAIL    1

typedef enum{
	FLASH_OK = 0x00,
	FLASH_CRC_ERROR = 0x01,
}FLASH_StatusTypeDef;

void store_flash_memory(uint32_t memory_address, uint8_t *data,uint16_t data_length);

#endif /* FLASH_CONTROL_H_ */
