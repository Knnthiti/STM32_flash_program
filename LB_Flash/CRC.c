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

//uint8_t boot_verify_crc(uint8_t *data ,uint8_t len ,uint32_t crc_host){
//	uint32_t crc_value = 0xff;
//	for(uint32_t i = 0 ; i < len ; i++){
//		uint32_t i_data = data[i];
//		crc_value = HAL_CRC_Accumulate(&hcrc, &i_data, 1);
//	}
//	__HAL_CRC_DR_RESET(&hcrc);
//	if(crc_value == crc_host){
//		return 0;
//	}
//	return 1;
//}

/* ฟังก์ชันนี้คำนวณเหมือน Python ที่คุณเขียนมา 100% */
uint32_t software_crc32(uint8_t *data, uint16_t length) {
    uint32_t crc = 0xFFFFFFFF; // ค่าเริ่มต้นเหมือน Python

    for (uint16_t i = 0; i < length; i++) {
        // 1. XOR ข้อมูลทีละ Byte (เหมือนบรรทัด crc = crc ^ byte ใน Python)
        crc ^= ((uint32_t)data[i] << 24); // เลื่อนไปไว้บนสุดเพื่อให้ตรงกับ Logic Shift Left

        // 2. วนลูป 32 รอบเพื่อ Shift Bit (เหมือน Python: for i in range(32))
        for (uint8_t j = 0; j < 32; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ 0x04C11DB7; // Polynomial เดียวกัน
            } else {
                crc <<= 1;
            }
        }
    }
    return crc; // ส่งค่ากลับ (ไม่ต้อง & 0xFFFFFFFF เพราะ C เป็น 32-bit อยู่แล้ว)
}

/* ฟังก์ชันเรียกใช้ */
uint8_t boot_verify_crc(uint8_t *data ,uint8_t len ,uint32_t crc_host){
    // เรียกใช้ Software CRC แทน Hardware CRC
    uint32_t crc_calc = software_crc32(data, len);

    if(crc_calc == crc_host){
        return 0; // ถูกต้อง
    }
    return 1; // ผิดพลาด
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
