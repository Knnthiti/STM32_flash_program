import os
import struct
import serial

MESSAGE_TOKEN = 0x23
CHANGE_BANK_REQUEST = 0x43
BOOT_UPDATE_REQUEST = 0x42
FIRMWARE_VERSION_REQUEST = 0x41

BOOT_ACK = 0x5F
BOOT_NACK = 0xAA


# แก้ไขฟังก์ชัน compute_crc ให้ตรงกับ C (Shift << 24)
def compute_crc(buff, length):
    crc = 0xFFFFFFFF
    for byte in buff[0:length]:
        # แก้ตรงนี้: ต้องเลื่อน byte ไปที่ 24 (MSB) เหมือนในภาษา C
        crc = crc ^ (byte << 24)
        for i in range(32):
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = crc << 1
    return crc & 0xFFFFFFFF


def update_flash_mem(serial_com, file_name):  # เปลี่ยนชื่อ parameter ให้ตรง
    # คำเตือน: USB Packet ปกติรับได้ max 64 bytes
    # ถ้าส่ง 128 bytes ข้อมูลจะถูกหั่น และ C Code คุณจะรับไม่ทัน
    # แนะนำให้ลดเหลือ 50 เพื่อความปลอดภัย
    WINDOW_SIZE = 48

    try:
        flash_bin_file = open(file_name, "rb")
    except:
        print("cannot open the bin file")  # ใส่ print กันเหนียว
        return  # ออกจากฟังก์ชันเลย

    size = os.path.getsize(file_name)

    data_token = [MESSAGE_TOKEN, BOOT_UPDATE_REQUEST]
    # วิธี pack bytes ที่ถูกต้องและง่ายกว่า
    data_token_bytes = struct.pack("<BB", *data_token)

    while size > 0:
        data_sent = flash_bin_file.read(WINDOW_SIZE)

        # สร้าง Packet
        # [Token 2 byte] + [Len 2 byte] + [Data n byte]
        packet_without_crc = (
            data_token_bytes + struct.pack("<H", len(data_sent)) + data_sent
        )

        # ส่ง (ฟังก์ชันส่งจะเติม CRC ให้เอง)
        send_data_serial(serial_com, packet_without_crc)

        # ... (ส่วนลด size และ print)...
        size -= len(data_sent)

        if read_boot_reply(serial_com) != BOOT_ACK:
            print("flash update failed")
            break  # ใช้ break ออกจาก loop ไม่ต้อง raise Exception ก็ได้ครับ

    flash_bin_file.close()
    print("Firmware update is over")


def read_boot_reply(serial_com):
    ack_value = serial_com.read(1)
    if len(ack_value) > 0 and ack_value[0] == BOOT_ACK:
        return BOOT_ACK
    else:
        return BOOT_NACK


def send_data_serial(serial_com, data):
    data = data + (struct.pack("<I", compute_crc(data, len(data))))
    serial_com.write(data)


comport = "COM14"
serial_com = serial.Serial(port=comport, baudrate=115200, timeout=15)

message = "Ukulele ukulele \n \0"

message = bytes(message, "utf-8")

while True:
    command = int(input("enter the command number 1 :"))
    if command == 1:
        update_flash_mem(serial_com, "test_F407.bin")
    else:
        break
