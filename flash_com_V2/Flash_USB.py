import os
import struct
import serial
import time

# --- Constants ---
MESSAGE_TOKEN = 0x23
BOOT_UPDATE_REQUEST = 0x42

BOOT_ACK = 0x5F
BOOT_NACK = 0xAA


def compute_crc(buff, length):
    """
    Calculates CRC32 to match STM32 Hardware CRC unit.
    """
    crc = 0xFFFFFFFF
    for byte in buff[0:length]:
        # NOTE: Must shift byte to 24 (MSB) to match C code logic (Hardware CRC)
        crc = crc ^ (byte << 24)
        for i in range(32):
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = crc << 1
    return crc & 0xFFFFFFFF


def update_flash_mem(serial_com, file_name):
    # Warning: Standard USB CDC packets usually max out at 64 bytes.
    # If sending 128 bytes, data gets split and the C code might fail to reassemble it.
    # Recommended to use 48 or 64 to stay within safe limits.
    # WINDOW_SIZE must be divisible by 4 for Flash Alignment.
    WINDOW_SIZE = 48

    try:
        # Read the entire file into memory first
        flash_bin_file = open(file_name, "rb")
        file_content = flash_bin_file.read()
        flash_bin_file.close()
    except IOError:
        print("Error: Cannot open the bin file.")
        return

    # ==========================================
    # [CRITICAL] Padding for Word Alignment (4 Bytes)
    # ==========================================
    # If file size is not divisible by 4, append 0xFF.
    # This prevents the STM32 from writing to a misaligned address at the end.
    remainder = len(file_content) % 4
    if remainder != 0:
        padding_needed = 4 - remainder
        file_content += b"\xff" * padding_needed
        print(
            f"Note: Added {padding_needed} bytes of padding (0xFF) for 4-byte alignment."
        )

    size = len(file_content)

    # Prepare Header Token
    data_token = [MESSAGE_TOKEN, BOOT_UPDATE_REQUEST]
    # Pack header as Little Endian, Unsigned Char (1 byte each)
    data_token_bytes = struct.pack("<BB", *data_token)

    current_idx = 0

    while size > 0:
        # Slice the data chunk based on WINDOW_SIZE
        chunk_size = min(WINDOW_SIZE, size)
        data_sent = file_content[current_idx : current_idx + chunk_size]

        # Construct Packet:
        # [Token (2B)] + [Length (2B)] + [Data (n Bytes)]
        packet_without_crc = (
            data_token_bytes + struct.pack("<H", len(data_sent)) + data_sent
        )

        # Send Data (CRC is calculated and appended inside this function)
        send_data_serial(serial_com, packet_without_crc)

        # Update progress
        current_idx += len(data_sent)
        size -= len(data_sent)

        # Progress bar (Optional but helpful)
        total_size = len(file_content)
        print(f"\rProgress: {100 - (size/total_size)*100:.1f}%", end="")

        # Check for ACK from STM32
        if read_boot_reply(serial_com) != BOOT_ACK:
            print("\nError: Flash update failed (NACK received)")
            break

    print("\nFirmware update completed.")


def read_boot_reply(serial_com):
    ack_value = serial_com.read(1)
    if len(ack_value) > 0 and ack_value[0] == BOOT_ACK:
        return BOOT_ACK
    else:
        return BOOT_NACK


def send_data_serial(serial_com, data):
    # Append CRC32 (4 Bytes, Little Endian) to the end of the packet
    data = data + (struct.pack("<I", compute_crc(data, len(data))))
    serial_com.write(data)


# --- Main Configuration ---
comport = "COM14"
# Ensure baudrate matches your STM32 settings
serial_com = serial.Serial(port=comport, baudrate=115200, timeout=15)

# Example message buffer (not used in main loop but kept for reference)
message = "Ukulele ukulele \n \0"
message = bytes(message, "utf-8")

while True:
    try:
        command = int(input("\nEnter command (1 to Update, other to Exit): "))
        if command == 1:
            print("Starting Firmware Update...")
            update_flash_mem(serial_com, "test_F407.bin")
        else:
            print("Exiting...")
            break
    except ValueError:
        print("Invalid input. Please enter a number.")
    except Exception as e:
        print(f"An error occurred: {e}")
        break
