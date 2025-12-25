import serial
import time
import os
import sys

# --- ตั้งค่า (CONFIG) ---
COM_PORT = "COM14"  # เปลี่ยนให้ตรงกับ Port ของบอร์ดคุณ
BAUD_RATE = 115200  # Baudrate (USB CDC ไม่ซีเรียสค่านี้)
FILE_PATH = "test_F407.bin"  # ชื่อไฟล์ Firmware .bin ที่ต้องการอัปโหลด
CHUNK_SIZE = 256  # ขนาดข้อมูลที่ส่งต่อ 1 packet (ควรหาร 8 ลงตัว)


def wait_for_response(ser, expected_response, timeout=5):
    start_time = time.time()
    buffer = b""
    while (time.time() - start_time) < timeout:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            buffer += data
            # print(f"DEBUG: {data}") # เปิดบรรทัดนี้ถ้าอยากเห็นว่าบอร์ดตอบอะไร

            if expected_response.encode() in buffer:
                return True
            if b"Error" in buffer:
                print(f"[-] Device Error: {buffer}")
                return False
        time.sleep(0.005)
    return False


def main():
    if not os.path.exists(FILE_PATH):
        print(f"[-] File {FILE_PATH} not found!")
        return

    try:
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
        print(f"[+] Connected to {COM_PORT}")
    except Exception as e:
        print(f"[-] Connection failed: {e}")
        return

    # 1. สั่งเริ่มกระบวนการ (Start & Erase)
    print("[*] Sending Start Command (Erasing Flash)...")
    ser.write(b"FLASHING_START")

    # รอนานหน่อยเผื่อลบ Flash เยอะ
    if wait_for_response(ser, "Flash: Unlocked!", timeout=15):
        print("[+] Flash Erased and Ready.")
    else:
        print("[-] Failed to initialize flash.")
        ser.close()
        return

    # 2. เริ่มส่งไฟล์
    print(f"[*] Flashing {FILE_PATH}...")
    with open(FILE_PATH, "rb") as f:
        file_content = f.read()

    total_bytes = len(file_content)
    sent_bytes = 0

    while sent_bytes < total_bytes:
        chunk = file_content[sent_bytes : sent_bytes + CHUNK_SIZE]
        ser.write(chunk)

        # **สำคัญ** รอคำว่า "OK" จากบอร์ดก่อนส่งก้อนถัดไป (Flow Control)
        if not wait_for_response(ser, "OK", timeout=2):
            print(f"\n[-] No ACK received at offset {sent_bytes}")
            break

        sent_bytes += len(chunk)

        # แสดง Progress Bar
        progress = (sent_bytes / total_bytes) * 100
        sys.stdout.write(
            f"\r[>] Progress: {progress:.2f}% ({sent_bytes}/{total_bytes} bytes)"
        )
        sys.stdout.flush()

    print("\n[+] Data transfer finished.")

    # 3. จบงาน (Finish & Reboot)
    print("[*] Sending Finish Command...")
    ser.write(b"FLASHING_FINISH")

    if wait_for_response(ser, "Flash: Success!", timeout=5):
        print("[+] Success! Device is rebooting...")
    else:
        print("[-] Finish command not acknowledged.")

    ser.close()


if __name__ == "__main__":
    main()
