# KT6368A BLE 2.1 Protocol Overview

This document summarizes the key concepts and message flows for the KT6368A BLE 2.1 protocol. Refer to `protocal.md` for the full specification.

## 1. Introduction

The KT6368A BLE 2.1 protocol defines Bluetooth Low Energy commands and responses between a host (PC or mobile) and the KT6368A module. It supports control, status queries, and data streaming.

## 2. Packet Format

Each BLE 2.1 packet consists of:

| Byte(s) | Field        | Description                        |
|--------:|--------------|------------------------------------|
| 1       | Start byte   | `0xAA`                             |
| 1       | Command ID   | Operation code                     |
| 1       | Length       | Payload length (N)                 |
| N       | Payload      | Command parameters or data         |
| 1       | Checksum     | XOR of all previous bytes          |

## 3. Common Commands

- **0x10**: Device Info  
  - Request: no payload  
  - Response payload: firmware version, hardware ID  

- **0x20**: Start Measurement  
  - Payload: measurement mode (1 byte)  
  - Response: acknowledgment (0x01 = OK, 0x00 = Error)  

- **0x30**: Read Data  
  - Payload: data block index (1 byte)  
  - Response: data packet (variable length)  

## 4. Error Codes

| Code | Meaning           |
|-----:|-------------------|
| 0x00 | Success           |
| 0xFF | General failure   |
| 0xFE | Invalid parameter |

## 5. Connection Workflow

1. **Scan & Connect**: Pair with the KT6368A BLE module via UUID `0000FFF0-0000-1000-8000-00805F9B34FB`.  
2. **Initialize**: Send Device Info command (0x10) and verify firmware version.  
3. **Operate**: Issue Start Measurement (0x20), then poll Read Data (0x30) until complete.  

## 6. CRC & Reliability

- All messages use a simple XOR checksum; discard packets with a checksum mismatch.  
- Implement a 100 ms timeout and up to 3 retry attempts per command.

## 7. References

- Full protocol details: `protocal.md`  
- BLE UUIDs and service definitions: see manufacturer documentation.
