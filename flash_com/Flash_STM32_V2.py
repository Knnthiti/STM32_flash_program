import os
import struct
import serial
import time

# --- ส่วนตั้งค่าค่าคงที่ (Constants) ---
# ค่าเหล่านี้ต้องตรงกับที่เขียนไว้ในภาษา C บนบอร์ด STM32 เป๊ะๆ
MESSAGE_TOKEN = 0x23  # เฮดเดอร์บอกจุดเริ่ม Packet (#)
CHANGE_BANK_REQUEST = 0x43  # คำสั่งขอสลับ Bank (เช่น App1 -> App2)
BOOT_UPDATE_REQUEST = 0x42  # คำสั่งขอเขียน Flash ใหม่
FIRMWARE_VERSION_REQUEST = 0x41  # คำสั่งขอเลขเวอร์ชั่น

BOOT_ACK = 0x5F  # บอร์ดตอบกลับว่า OK (_)
BOOT_NACK = 0xAA  # บอร์ดตอบกลับว่า Error


# --- ฟังก์ชันคำนวณ CRC32 (เช็คความถูกต้องของข้อมูล) ---
def compute_crc(buff, length):
    crc = 0xFFFFFFFF
    # วนลูปเช็คทีละ Byte (เลียนแบบ Hardware CRC ของ STM32)
    for byte in buff[0:length]:
        crc = crc ^ (byte)
        for i in range(32):
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7  # Polynomial มาตรฐาน Ethernet
            else:
                crc = crc << 1
    return crc & 0xFFFFFFFF


# --- ฟังก์ชันรอรับคำตอบจากบอร์ด (Read Reply) ---
def read_boot_reply(serial_com):
    # อ่านมา 1 byte เพื่อดูว่าเป็น ACK หรือ NACK
    # (ในโค้ดจริงอาจต้องใส่ Timeout กันค้างด้วย)
    return int.from_bytes(serial_com.read(1), byteorder="big")


# --- ฟังก์ชันหลัก: ส่งไฟล์ Firmware (.bin) ---
def update_flash_mem(serial_com, file_name):
    WINDOW_SIZE = 128  # ขนาดก้อนข้อมูลที่จะส่งต่อ 1 Packet

    try:
        flash_bin_file = open(file_name, "rb")
    except:
        print("cannot open the bin file")
        return

    size = os.path.getsize(file_name)  # ขนาดไฟล์ทั้งหมด
    size_copy = size  # เก็บไว้คำนวณ % Progress

    # เตรียมหัว Packet (Token + Command)
    data_token = [MESSAGE_TOKEN, BOOT_UPDATE_REQUEST]

    # แปลง List ตัวเลขให้เป็น Bytes (Binary)
    data_token_bytes = bytes()
    # struct.pack ใช้แปลงตัวเลข Python เป็น Byte แบบ C
    # '<' = Little Endian, 'B' = Unsigned Char (1 byte)
    for val in data_token:
        data_token_bytes += struct.pack("<B", val)

    # --- ลูปส่งข้อมูลทีละก้อน (Chunk) ---
    while size > 0:
        # 1. อ่านข้อมูลจากไฟล์มา 128 bytes
        data_sent = flash_bin_file.read(WINDOW_SIZE)

        # 2. สร้าง Packet ที่สมบูรณ์
        # Format: [Token + Command] + [Length (2 bytes)] + [Data Payload]
        # '<H' คือแปลงความยาวข้อมูลเป็น unsigned short (2 bytes)
        packet = data_token_bytes + struct.pack("<H", len(data_sent)) + data_sent

        # (หมายเหตุ: ในรูป Image 4 ตัดไปก่อนจะเห็นการส่ง CRC
        # ปกติจะต้องเอา packet ไปคำนวณ compute_crc แล้วต่อท้ายไปด้วย)

        # 3. ส่งข้อมูลผ่าน Serial
        send_data_serial(serial_com, packet)

        # 4. คำนวณ % ความคืบหน้าเพื่อแสดงผล
        progress = int(100 * (size_copy - size) / size_copy)
        print(f"Firmware update {progress}%")

        # 5. รอรับ ACK จากบอร์ด
        if read_boot_reply(serial_com) != BOOT_ACK:
            print("Flash update failed (NACK received)")
            break  # ถ้าบอร์ดบอกว่าผิดพลาด ให้หยุดส่งทันที

        # ลดขนาดที่เหลือลง เพื่อทำก้อนต่อไป
        size -= len(data_sent)

    flash_bin_file.close()
    print("Firmware update is over")


# --- ฟังก์ชันส่งข้อมูล (Helper) ---
def send_data_serial(serial_com, data):
    serial_com.write(data)


# --- ฟังก์ชันขอเปลี่ยน Firmware (สลับ Bank) ---
def change_firmware(serial_com):
    # สร้าง Packet: [0x23, 0x43, 0x00(Len)]
    command_message = [MESSAGE_TOKEN, CHANGE_BANK_REQUEST, 0]
    b = bytes()
    for val in command_message:
        b += struct.pack("<B", val)  # Pack เป็น Byte

    send_data_serial(serial_com, b)

    # รอ ACK
    if read_boot_reply(serial_com) != BOOT_ACK:
        print("Flash change failed")


# --- MAIN PROGRAM ---
# ตั้งค่า Serial Port
comport = "COM10"  # เปลี่ยนให้ตรงกับเครื่องคุณ
# baudrate สูงๆ เพื่อความไว
serial_com = serial.Serial(port=comport, baudrate=115200, timeout=15)

# ส่งข้อความทักทาย (Handshake - Optional ตามโปรโตคอลเขา)
# message = "Ukulele ukulele \n\0"
# serial_com.write(bytes(message, 'utf-8'))

while True:
    print("\n--- Menu ---")
    print("1 - Update Firmware")
    print("2 - Change Firmware (Bank Swap)")
    print("3 - Check Version")

    try:
        command = int(input("Enter command number: "))

        if command == 1:
            # เรียกฟังก์ชันส่งไฟล์
            update_flash_mem(serial_com, "CUSTOM_BOOTLOADER_STM32L4.bin")
        elif command == 2:
            change_firmware(serial_com)
        elif command == 3:
            # (ฟังก์ชัน read_firmware_version ในรูปที่ 2)
            pass
        else:
            break
    except ValueError:
        print("Invalid input")

serial_com.close()
