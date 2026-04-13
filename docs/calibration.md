# Calibration Guide

## 1. Encoder Calibration
```bash
# Drive robot 1m straight, measure actual distance
# Adjust TICKS_PER_REV if off > 2%
```
Expected: 1920 ticks/rev for JGA25-370 48CPR + 40:1 gearbox

## 2. PID Tuning (Velocity Control)
Start values: Kp=2.5, Ki=0.1, Kd=0.05

Tuning steps:
1. Set Ki=Kd=0, increase Kp until slight oscillation
2. Reduce Kp by 20%, add Ki slowly (0.05 increments)
3. Add Kd to dampen overshoot

## 3. IMU Calibration (BNO055)
```cpp
// Run calibration sketch:
// 1. Place robot flat — accelerometer cal
// 2. Rotate in all 6 orientations — mag cal
// 3. Move figure-8 — gyro cal
// 4. Save calibration offsets to NVS
```

## 4. LiDAR Offset
Mount LiDAR centered; if offset, update `base_link → laser` TF in URDF.

## 5. Wheel Odometry vs IMU Fusion
Nav2 EKF node fuses `/odom` + `/imu/data` for better localization.
