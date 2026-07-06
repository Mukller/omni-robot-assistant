# Wiring Guide

## Power Distribution
```
LiPo 11.1V → Power switch → Fuse (5A)
  ├─ L298N motor drivers (12V direct)
  ├─ DC-DC buck 5V → Raspberry Pi 4 (via USB-C, 3A)
  ├─ DC-DC buck 5V → ESP32 (via VIN, 0.5A)
  └─ DC-DC buck 5V → 7" display
```

## ESP32 GPIO Map

| GPIO | Function              | Notes                              |
|------|-----------------------|------------------------------------|
| 25   | L298N IN1 — Left      | Direction                          |
| 26   | L298N IN2 — Left      | Direction                          |
| 27   | L298N ENA — Left      | PWM (LEDC ch0, 20 kHz)             |
| 14   | L298N IN3 — Right     | Direction                          |
| 12   | L298N IN4 — Right     | Direction                          |
| 13   | L298N ENB — Right     | PWM (LEDC ch1, 20 kHz)             |
| 32   | Encoder L — Phase A   | Interrupt (rising)                 |
| 33   | Encoder L — Phase B   | Interrupt (rising)                 |
| 34   | Encoder R — Phase A   | Input-only, interrupt              |
| 35   | Encoder R — Phase B   | Input-only, interrupt              |
| 16   | LiDAR UART RX         | Serial2, 115200 baud               |
| 17   | LiDAR UART TX         | Serial2                            |
| 21   | I²C SDA               | BNO055 + INA219 shared bus         |
| 22   | I²C SCL               | BNO055 + INA219 shared bus         |
| 36   | Battery ADC (VP)      | Input-only; 100kΩ/220kΩ divider    |

> **GPIO 34/35/36** are input-only on ESP32 — do not connect to output drivers.

## I²C Devices

| Device   | Address | Function                    |
|----------|---------|-----------------------------|
| BNO055   | 0x28    | IMU — accel + gyro + fusion |
| INA219 L | 0x40    | Left motor current + voltage |
| INA219 R | 0x41    | Right motor current + voltage|

Pull-ups: 4.7 kΩ to 3.3 V on SDA and SCL.

## ESP32 ↔ Raspberry Pi
- **Primary**: UDP over WiFi — microROS agent on Pi port 8888
- **Fallback**: USB serial (`/dev/ttyUSB0`, 115200) — change `main.ino` transport to serial

## Voltage Divider for Battery ADC

```
LiPo+ ──┤ 100 kΩ ├──┬──┤ 33 kΩ ├── GND
                     │
                  GPIO 36 (max 3.3 V)
```

Scale factor in firmware: `V_bat = adc_voltage × (100 + 33) / 33 ≈ adc_voltage × 4.03`  
BatteryMonitor uses `dividerRatio = 4.0f` (close enough — ~0.7 % error).  
Full charge (12.6 V) → **3.13 V** at GPIO 36. Under the 3.3 V ADC limit.

> Use 1% tolerance resistors for accuracy. Available in E96 series: 100kΩ + 33.2kΩ.
