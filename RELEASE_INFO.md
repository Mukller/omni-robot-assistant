# Release Information

## v1.1.8 — 2026-07-09

### Critical Fix
- **Safety latch broken in two ways**: (1) `_latchedFault = _battLow || overcurrent` at the end of `update()` unconditionally overwrote the latch every cycle — fault lasting one cycle would auto-reset two cycles later without any operator action; (2) `clearEStop()` only cleared `_estop`, not `_latchedFault`, so calling it didn't take immediate effect. Fixed: latch only ever SETS (`if (...) _latchedFault = true`); `clearEStop()` now clears both.

### Other
- `cad/SOURCES.md` — YDLidar X4 entry pointed to non-existent local `stl/ydlidar_x4_bracket.stl`; updated to upstream MakersPet 3D models link.

---

## v1.1.7 — 2026-07-08

Final audit sweep — config consistency and docs correctness.

### Fixed
- **`velocity_smoother` limits**: `max_velocity` was `[0.26, 0, 1.0]`; synced to DWB values `[0.22, 0, 1.8]`; `max_accel` `[2.5]` → `[1.5]`; `max_decel` `[-2.5]` → `[-2.0]`; `odom_topic: "odom"` → `/odometry/filtered`
- **`self_drive_gazebo.yaml`**: forward speed `0.3 m/s` exceeded robot safe max; lowered to `0.22`
- **`teleop_keyboard.yaml`**: `max_lin_vel: 0.61` far above JGA25-370 physical max (~0.39 m/s); corrected to `0.39`
- **`motors_and_wheels.md`**: "Omni Wheels 48mm" section had wrong type and specs; replaced with "Drive Wheels (75mm rubber)" matching actual hardware
- **`cad/bom.csv`**: "Omni wheel 48mm" → "Rubber wheel 75mm"
- **`raspberry_pi.md`**: `ExecStartPre` sourcing bash files in a child shell (env does not propagate to `ExecStart`); removed the no-op line

---

## v1.1.6 — 2026-07-07

Stability and correctness sprint — 6 patch releases fixing bugs found in code audit.

### Critical Fixes
- **e-stop permanent latch**: cmd_vel timeout now auto-clears; overcurrent/battery latch until `clearEStop()`
- **Startup e-stop**: `_lastCmdMs` initialised to `millis()` — no more instant timeout on boot
- **Motor brake**: `BRAKE_STOP` now keeps L298N enable pin HIGH (was coast, not brake)
- **Encoder floating pins**: `INPUT_PULLUP` on GPIO 32/33; external 10kΩ pull-ups documented for GPIO 34/35
- **Odometry drift**: position integration now uses midpoint heading θ+dθ/2
- **Gazebo IMU**: moved `libgazebo_ros_imu_sensor` into `<sensor type="imu">` element — was at model level, produced no data
- **Missing world file**: `sdf/empty.world` created — `gazebo.launch.py` crashed without it
- **xacro not processed**: `loki_bringup.launch.py` and `gazebo.launch.py` now call `xacro.process_file()` instead of raw file read
- **ADC attenuation**: `analogSetPinAttenuation` (pin-scoped) replaces `analogSetAttenuation` (all channels)
- **PID test wrong assertion**: `test_reset_clears_state` expected 0.1 but Kd term made output ~10.1
- **Backwards advice**: troubleshooting doc told to *increase* `inflation_radius` when stuck — correct is decrease

### Other Fixes
- `IMU::getEuler()` public method to avoid 3 separate I2C reads for heading/pitch/roll
- `BATT_DIVIDER_RATIO 4.03f` in `config.h` (100kΩ/33kΩ exact ratio)
- `ros2_architecture.md` stale params (use_imu_data, max_vel_x, acc_lim_x, sim_time)
- `.gitignore` now covers `ros2/build/`, `ros2/install/`, `ros2/log/`
- Assembly guide: wrong STL path + wrong wheel size (48mm omni → 75mm rubber)

---

## v1.0.0 — 2026-05-23

First public release of Loki Robot Assistant.

### Highlights
- Complete ESP32 firmware with microROS (micro_ros_kaia + kaiaai/LDS)
- Full ROS2 Iron package: Cartographer SLAM + Nav2 + explore_lite
- EKF sensor fusion (odom + IMU)
- Web telemetry dashboard
- Comprehensive documentation

### Supported Hardware
- MakersPet Loki 200mm platform
- YDLidar X4 or Xiaomi LDS02RR
- ESP32 DevKit v1 + Raspberry Pi 4
