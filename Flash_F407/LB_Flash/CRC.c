/*
 * CRC.c
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */
#include "CRC.h"
#include "stdio.h"
#include "usbd_cdc_if.h"

extern CRC_HandleTypeDef hcrc;

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

uint32_t software_crc32(uint8_t *data, uint16_t length) {
    uint32_t crc = 0xFFFFFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= ((uint32_t)data[i] << 24);

        for (uint8_t j = 0; j < 32; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ 0x04C11DB7;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

uint8_t boot_verify_crc(uint8_t *data ,uint8_t len ,uint32_t crc_host){
    uint32_t crc_calc = software_crc32(data, len);

    if(crc_calc == crc_host){
        return 0;
    }
    return 1;
}

void boot_send_ack(){
	uint8_t ack = BOOT_ACK;
	CDC_Transmit_FS(&ack ,1);
}

void boot_send_nack(void){
	uint8_t nack = BOOT_NACK;
	CDC_Transmit_FS(&nack ,1);
}

void send_data2pc(uint8_t* data ,uint8_t size){
	CDC_Transmit_FS(data ,size);
}
