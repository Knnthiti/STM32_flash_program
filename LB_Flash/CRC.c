/*
 * CRC.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */
#include "CRC.h"
#include "stdio.h"

CRC_HandleTypeDef* hcrc;

uint8_t parse_request(CommandTypeDef* command ,uint8_t* data ,uint16_t size){
	if((uint8_t)(*data) == MESSAGE_TOKEN){
		if((uint8_t)(*(data + 1)) == BOOT_UPDATE_REQUSET ){
			command->rw_request = *(data + 1);
			command->data_size = *((uint16_t*)&data[2]);

			if(size != command->data_size + 2 + 2 +4){
				return SERIAL_SIZE_ERROR;
			}

			command->data = &data[4];
			command->crc_value = *((uint32_t *)&data[4 + command->data_size]);

			if(boot_verify_crc(data ,size - 4 ,command->crc_value)){
				return SERIAL_CRC_ERROR;
			}
		}
	}else{
		return SERIAL_MESSAGE_ERROR;
	}
	return SERIAL_OK;
}

//uint
