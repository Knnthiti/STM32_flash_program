/*
 * CRC.h
 *
 *  Created on: Jan 10, 2026
 *      Author: Knnn
 */

#ifndef CRC_H_
#define CRC_H_

#include "main.h"

#define MESSAGE_TOKEN 0x23

#define BOOT_UPDATE_REQUSET 0x42

#define BOOT_NACK 0xaa
#define BOOT_ACK  0x5f

typedef enum{
	SERIAL_OK = 0x00,
	SERIAL_MESSAGE_ERROR = 0x01,
	SERIAL_SIZE_ERROR = 0x02,
	SERIAL_CRC_ERROR = 0x03
}Serial_StatusTypeDef;

typedef struct{
	uint8_t rw_request;
	uint8_t *data;
	uint16_t data_size;
	uint32_t crc_value;
}__attribute__((__packed__))CommandTypeDef;

uint8_t boot_verify_crc(uint8_t *data ,uint8_t len ,uint32_t crc_host);
uint8_t parse_request(CommandTypeDef* command,uint8_t *data,uint16_t size);

void boot_send_nack(void);
void boot_send_ack();
void send_data2pa(uint8_t *data ,uint8_t size);


#endif /* CRC_H_ */
