# Calibration Guide

## 1. Encoder Calibration

Drive the robot exactly 1 m on a flat surface and measure actual distance travelled.
If off by more than 2%, adjust `TICKS_PER_REV` in `firmware/config.h`.

Expected: **1920 ticks/rev** — JGA25-370 (48 CPR encoder × 40:1 gearbox).

Verify with serial monitor while driving 1 m:
```
Ticks L: 4800  Ticks R: 4810   (should each be ~4800 = 1920 × 2.5 revs per metre)
```

## 2. PID Tuning (Velocity Control)

Default values in `firmware/src/main.ino`: **Kp=2.5, Ki=0.8, Kd=0.05**

These work for JGA25-370 at 12 V. To re-tune:

1. Set `Ki=0`, `Kd=0`. Increase `Kp` until slight oscillation appears (~3–4).
2. Reduce `Kp` by 20%. Slowly add `Ki` (start 0.1, increase by 0.1 until steady-state error disappears).
3. Add small `Kd` (0.01–0.1) to dampen overshoot after direction change.

Monitor wheel velocity via ROS2:
```bash
ros2 topic echo /odom | grep -A3 twist
```

## 3. IMU Calibration (BNO055)

BNO055 in NDOF mode self-calibrates at runtime. Check status:
```bash
ros2 topic echo /imu/data --field header.frame_id   # confirms publishing
```

For best calibration before a mapping session:
1. Place robot flat for 5 s (accelerometer).
2. Rotate slowly on all 6 faces (magnetometer).
3. Drive figure-8 twice (gyroscope).

Watch calibration quality in serial monitor:
```
[IMU] sys=3 gyr=3 acc=3 mag=3   ← all 3 = fully calibrated
```

## 4. LiDAR Offset

LiDAR is mounted at the centre of the robot body. If mechanically offset,
update the `laser_joint` origin in `ros2/urdf/robot.urdf.xacro`:
```xml
<origin xyz="0.03 0 0.08"/>   <!-- x forward offset in metres -->
```

Verify TF in RViz2: **Fixed Frame = map**, display **TF** — `laser` frame should
sit at the physical LiDAR location.

## 5. EKF Verification

Confirm odometry + IMU are fusing correctly:
```bash
ros2 topic hz /odom              # should be ~20 Hz (loop rate)
ros2 topic hz /imu/data          # should be ~50 Hz
ros2 topic hz /odometry/filtered # should be ~50 Hz (EKF output)
```

Drive a square (1 m sides, 4 × 90° turns) and check return-to-origin error:
- Good: < 5 cm drift after full square
- Needs tuning: > 10 cm drift → adjust `process_noise_covariance` in `ros2/config/ekf.yaml`

## 6. Battery ADC Calibration

Measure actual battery voltage with a multimeter and compare to the web dashboard.
If off, the voltage divider resistors may be non-ideal (use 1% tolerance: 100 kΩ + 33 kΩ).

Adjust `dividerRatio` in `firmware/src/battery_monitor.h` to match:
```cpp
// If measured=12.4V and reported=11.9V: ratio = 4.0 × (12.4/11.9) = 4.17
BatteryMonitor batt(BATT_PIN, 4.17f);
```
