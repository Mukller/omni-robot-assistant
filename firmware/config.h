#pragma once
// Motor driver (L298N) — left motor
#define MOT_L_IN1  25
#define MOT_L_IN2  26
#define MOT_L_EN   27
// Motor driver — right motor
#define MOT_R_IN1  14
#define MOT_R_IN2  12
#define MOT_R_EN   13
// Encoders
#define ENC_L_A  32
#define ENC_L_B  33
#define ENC_R_A  34
#define ENC_R_B  35
// LiDAR (UART2)
#define LIDAR_RX  16
#define LIDAR_TX  17
// IMU (I2C)
#define IMU_SDA  21
#define IMU_SCL  22
// Current sensors (I2C shared)
#define INA_ADDR_L  0x40
#define INA_ADDR_R  0x41
// Battery ADC
#define BATT_PIN  36
// Robot geometry (Loki 200mm platform)
#define WHEEL_RADIUS_M   0.0375f  // 75mm diameter
#define WHEEL_BASE_M     0.172f   // distance between wheels
#define TICKS_PER_REV    1920     // 48 CPR * 40:1 gearbox
