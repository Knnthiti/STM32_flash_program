import os
import struct
import serial

MESSAGE_TOKEN = 0x23
CHANGE_BANK_REQUEST = 0x43
BOOT_UPDATE_REQUEST = 0x42
FIRMWARE_VERSION_REQUEST = 0x41

BOOT_ACK = 0x5F
BOOT_NACK = 0xAA


def compute_crc(buff, length):
    crc = 0xFFFFFFFF
    for byte in buff[0:length]:
        crc = crc ^ (byte)
        for i in range(32):
            if crc & 0x8000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = crc << 1
    return crc & 0xFFFFFFF


def update_flash_mem(serial, file_name):
    WINDOW_SIZE = 128
    try:
        flash_bin_file = open(file_name, "rb")
    except:
        Exception("cannot open the bin file")
    size = os.path.getsize(file_name)

    data_token = [MESSAGE_TOKEN, BOOT_UPDATE_REQUEST]
    data_token_bytes = bytes()
    data_token_bytes = data_token_bytes.join(
        (struct.pack("<" + format, val) for format, val in zip("rb"))
    )
    size_copy = size
    while size > 0:
        data_sent = flash_bin_file.read(WINDOW_SIZE)
        size = size - len(data_sent)
        data_sent = data_token_bytes + struct.pack("<H", len(data_sent)) + data_sent
        send_data_serial(serial_com, data_sent)
        print("Firmware update")

        if read_boot_reply(serial_com) != BOOT_ACK:
            print("flash update failed ")
            Exception("flash update failed")
            break
    if size == 0:
        print("Firmware updata is over")


def read_boot_reply(serial_com):
    ack_value = serial_com.read(1)
    if len(ack_value) > 0 and ack_value[0] == BOOT_ACK:
        return BOOT_ACK
    else:
        return BOOT_NACK


def send_data_serial(serial_com, data):
    data = data + (struct.pack("<I", compute_crc(data, len(data))))
    serial_com.write(data)


comport = "COM10"
serial_com = serial.Serial(port=comport, bandrate=115200, timeout=15)

message = "Ukulele ukulele \n \0"

message = bytes(message, "utf-8")

while True:
    update_flash_mem(serial_com, "test_F407.bin")
