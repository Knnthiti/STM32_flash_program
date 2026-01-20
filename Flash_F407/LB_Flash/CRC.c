/*
 * CRC.c
 *
 * Created on: Jan 10, 2026
 * Author: Knnn
 */
#include "CRC.h"
#include "stdio.h"
#include "usbd_cdc_if.h"

extern CRC_HandleTypeDef hcrc;

/**
 * @brief  Parses the received data packet from the Host (PC).
 * @param  command: Pointer to the Command structure to store parsed data.
 * @param  data:    Pointer to the raw received buffer.
 * @param  size:    Size of the received buffer.
 * @retval Status (SERIAL_OK, SERIAL_SIZE_ERROR, SERIAL_CRC_ERROR, etc.)
 */
uint8_t parse_request(CommandTypeDef* command, uint8_t* data, uint16_t size) {
    // 1. Check Message Token
    if ((uint8_t)(*data) == MESSAGE_TOKEN) {

        // 2. Check Command Type (e.g., Firmware Update Request)
        if ((uint8_t)(*(data + 1)) == BOOT_UPDATE_REQUSET) {
            command->rw_request = *(data + 1);

            // 3. Extract Data Length (2 Bytes, Little Endian)
            command->data_size = *((uint16_t*)&data[2]);

            // 4. Validate Total Packet Size
            // Format: [Token (1)] + [Cmd (1)] + [Len (2)] + [Data (N)] + [CRC (4)]
            // Total overhead = 8 bytes (Wait... checking logic: 1+1+2+4 = 8)
            // Code uses: data_size + 2 (Token+Cmd) + 2 (Len) + 4 (CRC)
            if (size != command->data_size + 2 + 2 + 4) {
                return SERIAL_SIZE_ERROR;
            }

            // 5. Set pointer to the actual data payload
            command->data = &data[4];

            // 6. Extract the CRC32 sent by the Host (Last 4 bytes)
            command->crc_value = *((uint32_t *)&data[4 + command->data_size]);

            // 7. Verify CRC integrity
            // Calculate CRC of the packet (excluding the CRC bytes themselves)
            if (boot_verify_crc(data, size - 4, command->crc_value)) {
                return SERIAL_CRC_ERROR;
            }
        }
    } else {
        return SERIAL_MESSAGE_ERROR; // Invalid Token
    }
    return SERIAL_OK;
}

/**
 * @brief  Calculates CRC32 using a software bit-wise algorithm.
 * NOTE: This implementation strictly matches the Python script logic.
 * @param  data:   Pointer to the data buffer.
 * @param  length: Length of data to calculate.
 * @retval Calculated CRC32 value.
 */
uint32_t software_crc32(uint8_t *data, uint16_t length) {
    uint32_t crc = 0xFFFFFFFF; // Initial value (Standard CRC32)

    for (uint16_t i = 0; i < length; i++) {
        // 1. XOR the input byte into the high byte of the CRC register
        // (Equivalent to `crc = crc ^ byte` in Python but shifted for MSB processing)
        crc ^= ((uint32_t)data[i] << 24);

        // 2. Process 32 bits (Bit-wise shift)
        for (uint8_t j = 0; j < 32; j++) {
            // Check if the MSB is set
            if (crc & 0x80000000) {
                // Shift left and XOR with Polynomial 0x04C11DB7
                crc = (crc << 1) ^ 0x04C11DB7;
            } else {
                // Just shift left
                crc <<= 1;
            }
        }
    }
    return crc; // Return result (Already 32-bit, no need to mask in C)
}

/**
 * @brief  Verifies the data integrity by comparing calculated CRC vs Host CRC.
 * @param  data:     Pointer to the data buffer.
 * @param  len:      Length of the data.
 * @param  crc_host: CRC value received from the Host.
 * @retval 0 if Correct, 1 if Failed.
 */
uint8_t boot_verify_crc(uint8_t *data, uint8_t len, uint32_t crc_host) {
    // Use Software CRC to ensure compatibility with Python script
    uint32_t crc_calc = software_crc32(data, len);

    if (crc_calc == crc_host) {
        return 0; // CRC Match (Success)
    }
    return 1; // CRC Mismatch (Error)
}

/**
 * @brief  Sends an Acknowledge (ACK) byte to the Host.
 */
void boot_send_ack() {
    uint8_t ack = BOOT_ACK;
    CDC_Transmit_FS(&ack, 1);
}

/**
 * @brief  Sends a Not-Acknowledge (NACK) byte to the Host.
 */
void boot_send_nack(void) {
    uint8_t nack = BOOT_NACK;
    CDC_Transmit_FS(&nack, 1);
}

/**
 * @brief  Sends raw data to the PC via USB CDC.
 * @param  data: Pointer to data buffer.
 * @param  size: Number of bytes to send.
 */
void send_data2pc(uint8_t* data, uint8_t size) {
    CDC_Transmit_FS(data, size);
}