# Wiring Guide

## Power Distribution
```
LiPo 11.1V → Power switch → Fuse (5A)
  ├─ L298N motor drivers (12V direct)
  ├─ DC-DC buck 5V → Raspberry Pi 4 (via USB-C, 3A)
  ├─ DC-DC buck 5V → ESP32 (via VIN, 0.5A)
  └─ DC-DC buck 5V → 7" display
```

## ESP32 Connections
| Pin  | Function           |
|------|--------------------|
| 25   | L298N IN1 (Left)   |
| 26   | L298N IN2 (Left)   |
| 27   | L298N EN  (Left)   |
| 14   | L298N IN1 (Right)  |
| 12   | L298N IN2 (Right)  |
| 13   | L298N EN  (Right)  |
| 32   | Encoder L — A      |
| 33   | Encoder L — B      |
| 34   | Encoder R — A      |
| 35   | Encoder R — B      |
| 16   | LiDAR RX           |
| 17   | LiDAR TX           |
| 21   | IMU SDA (BNO055)   |
| 22   | IMU SCL (BNO055)   |
| 36   | Battery ADC        |

## ESP32 ↔ Raspberry Pi
- UDP over WiFi (microROS agent on Pi, port 8888)
- Alternatively: USB serial fallback

## I²C Bus
Shared SDA/SCL: BNO055 (0x28), INA219-L (0x40), INA219-R (0x41)
