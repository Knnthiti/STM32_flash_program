# STM32 USB CDC Bootloader 🚀

A custom Bootloader implementation for **STM32F407** Microcontrollers using the **USB CDC (Virtual COM Port)** interface. This project enables firmware updates directly from a PC via a Python script, eliminating the need for external hardware programmers (like ST-Link/J-Link) for field updates.

---

## 📋 Table of Contents
- [System Architecture](#-system-architecture)
- [Memory Map](#-memory-map-stm32f407)
- [Communication Protocol](#-communication-protocol)
- [Project Structure](#-project-structure)
- [How It Works](#how-it-works)
- [Usage Guide](#-usage-guide)

---

## 🧠 System Architecture

The Flash memory is partitioned into two distinct regions. The Bootloader resides at the beginning of the Flash, while the User Application starts at a specific offset (Sector 3).

---

## 💾 Memory Map (STM32F407)

| Region | Start Address | Flash Sector | Description |
| :--- | :--- | :--- | :--- |
| **Bootloader** | `0x0800 0000` | Sector 0 | Handles USB communication, Flash erasing, and writing. |
| **Application** | `0x0800 C000` | Sector 3 | The user application firmware (LED Blink, etc.). |

---

## 📡 Communication Protocol

The PC (Host) communicates with the STM32 (Device) using a custom packet structure over USB Serial.

### Packet Structure
```text
[Token (1B)] + [Command (1B)] + [Length (2B)] + [Data (N Bytes)] + [CRC32 (4B)]
```

-Token: 0x23 (Start of Frame)

-Command: 0x42 (Firmware Update Request)

-Length: Payload size (Little Endian, max 48-64 bytes recommended)

-Data: Firmware binary chunk

-CRC32: Checksum (Standard Ethernet Polynomial 0x04C11DB7)

### Responses

-✅ ACK (0x5F): Packet received, verified, and written successfully.

-❌ NACK (0xAA): Error (CRC mismatch, size error, or flash write failure).

---

## 📂 Project Structure
Bootloader Firmware: STM32 C code handles the USB CDC, Flash memory logic, and Jump mechanism.

Python Host Script: Reads the binary file, handles padding, calculates CRC, and sends data packets.

User Application: The target firmware (must be linked to 0x0800 C000).

---

## <a id="how-it-works"></a>⚙️ How It Works
1. Host Side (Python)
Read File: Opens the .bin firmware file.

Padding (Critical): Appends 0xFF bytes to the end of the data to ensure the total size is divisible by 4. This is required for STM32 Flash Word alignment.

Chunking: Splits data into small chunks (e.g., 48 bytes) to fit within USB endpoint buffers.

Transmission: Sends packets sequentially using a "Stop-and-Wait" mechanism.

2. Device Side (STM32)
Receive: USB CDC callback captures the data.

Verify: Calculates CRC32 of the payload and compares it with the received CRC.

Flash Operation:

Erase: On the first packet, erases Application sectors (Sector 3 onwards).

Program: Writes data to Flash in 4-byte words.

Acknowledge: Sends ACK back to the PC.

3. Jump to Application
When the update is finished or triggered by a user button:

De-Init: The Bootloader disables USB, SysTick, and resets the RCC Clock to default (HSI).

Stack Pointer: Sets the Main Stack Pointer (MSP) using the value at 0x0800 C000.

Jump: Branches to the Reset Handler of the Application.

---

## 🚀 Usage Guide
### Step 1: Prepare the User Application
Navigate to the project folder:
📂 **[/Flash_F407](./Flash_F407)**

**1. Linker Script (.ld):** Change the Flash origin address.

```c
/* In STM32F407x_FLASH.ld */
FLASH (rx) : ORIGIN = 0x800C000, LENGTH = ...
```

**2. Vector Table Offset: Set this in system_stm32f4xx.c OR main.c.

```
/* Option A: In system_stm32f4xx.c */
#define VECT_TAB_OFFSET  0xC000

/* Option B: In main.c (first line of main) */
SCB->VTOR = 0x0800C000;
```

Step 2: Flash the Bootloader
Flash the Bootloader code to the STM32 (Sector 0) using ST-Link.

Step 3: Run the Python Script
Connect the STM32 via USB to the PC and run the script:

📂 **[flash_com_V2/Flash_USB.py](./flash_com_V2/Flash_USB.py)**

Enter the COM port and select the binary file when prompted.
