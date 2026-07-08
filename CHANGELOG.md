# Changelog

All notable changes to this project are documented here.
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## [1.1.8] — 2026-07-09

### Fixed
- `firmware/src/safety.cpp` — `_latchedFault` was overwritten (`= _battLow || overcurrent`) at the end of every `update()` call, so the latch only survived one extra cycle after fault cleared, then auto-reset without `clearEStop()`; also `clearEStop()` only cleared `_estop` not `_latchedFault` so calling it didn't actually work immediately; fixed both: latch only sets (never auto-clears), and `clearEStop()` clears `_latchedFault` too
- `cad/SOURCES.md` — YDLidar X4 entry referenced non-existent local `stl/ydlidar_x4_bracket.stl`; pointed to upstream MakersPet 3D models repo

## [1.1.7] — 2026-07-08

### Fixed
- `ros2/config/navigation.yaml` — `velocity_smoother` limits were `max_velocity: [0.26, 0, 1.0]`, `max_accel: [2.5, 0, 3.2]`, `max_decel: [-2.5, 0, -3.2]`, inconsistent with DWB (0.22/1.5/-2.0); also `odom_topic: "odom"` → `/odometry/filtered`
- `ros2/config/self_drive_gazebo.yaml` — forward speed `0.3 m/s` exceeded safe max (0.22); corrected
- `ros2/config/teleop_keyboard.yaml` — `max_lin_vel: 0.61` above JGA25-370 physical max; corrected to 0.39
- `docs/hardware/motors_and_wheels.md` — "Omni Wheels 48mm" section listed omni-wheel specs; replaced with correct 75mm rubber drive wheel table
- `cad/bom.csv` — "Omni wheel 48mm" corrected to "Rubber wheel 75mm"
- `docs/hardware/raspberry_pi.md` — systemd `ExecStartPre` sourcing bash setup files in a child shell (env doesn't propagate to `ExecStart`); removed the no-op directive

## [1.1.6] — 2026-07-07

### Fixed
- `docs/troubleshooting.md` — "Robot застрял" advice said to **increase** inflation_radius but larger radius blocks narrow paths and makes it harder; changed to **decrease**; also fixed incorrect `lifecycle_node list` restart command
- `docs/assembly.md` — STL files referenced non-existent `stl/` folder; pointed to MakersPet GitHub; "48mm omni wheels" was wrong — Loki uses 75mm rubber wheels

## [1.1.5] — 2026-07-07

### Fixed
- `firmware/config.h` — add `BATT_DIVIDER_RATIO 4.03f` (100kΩ/33kΩ → exact ratio); document GPIO 34/35 no-pullup limitation
- `firmware/src/main.ino` — pass `BATT_DIVIDER_RATIO` to `BatteryMonitor` constructor (was using header default 4.0f)
- `docs/wiring.md` — GPIO table still said "100kΩ/220kΩ" for GPIO 36 (stale); add explicit pull-up note for GPIO 34/35
- `ros2/sdf/makerspet_loki/model.sdf` — IMU plugin was at model level (never produced data); moved into `<sensor type="imu">` element inside `base_link` with BNO055-realistic noise values; removed duplicate model-level plugin
- `ros2/sdf/empty.world` — created missing world file; `gazebo.launch.py` crashed with FileNotFoundError without it

## [1.1.4] — 2026-07-07

### Fixed
- `firmware/test/test_pid.cpp` — `test_reset_clears_state` expected `~0.1` but with Kd=1 the derivative kick gives `~10.1`; changed to pure-I controller (Kd=0) so expected value is correct
- `.gitignore` — add `ros2/build/`, `ros2/install/`, `ros2/log/` to prevent committing ROS2 workspace artifacts; add `*.swp` for vim

## [1.1.3] — 2026-07-07

### Fixed
- `firmware/src/battery_monitor.cpp` — `analogSetAttenuation(ADC_11db)` changed attenuation on all ESP32 ADC channels globally; replaced with `analogSetPinAttenuation(_pin, ADC_11db)` which scopes to the battery pin only
- `firmware/src/imu.h/.cpp` — `getHeading()`, `getPitch()`, `getRoll()` each issued a separate I2C read; added public `getEuler()` that reads once; individual getters now call it (still 1 read each when used alone; callers needing all three should use `getEuler()` directly)

## [1.1.2] — 2026-07-07

### Fixed
- `ros2/launch/loki_bringup.launch.py` — raw `.xacro` file read passed unexpanded macros as `robot_description`; replaced with `xacro.process_file()`
- `firmware/src/motor_driver.cpp` — `stop(BRAKE_STOP)` set `analogWrite(_en, 0)` at the end, disabling the H-bridge and causing coast instead of brake; fix sets `en=255` for BRAKE_STOP
- `firmware/src/encoder.cpp` — `pinMode(pin, INPUT)` left encoder pins floating, causing phantom tick counts; changed to `INPUT_PULLUP`
- `host/web_monitor.py` — `<meta http-equiv="refresh" content="0">` caused instant infinite page reload; removed (JS polling via `setInterval` handles updates)
- `docs/faq.md` — broken external link restored to correct local `lidar_compatibility.md`

## [1.1.1] — 2026-07-07

### Fixed
- `firmware/src/odometry.cpp` — position integration used updated heading θ+dθ instead of midpoint θ+dθ/2, causing systematic drift in curves
- `ros2/launch/gazebo.launch.py` — raw-read of `.xacro` passed unexpanded XML to `robot_description`; replaced with `xacro.process_file()`
- `docs/faq.md` — max speed still said 0.61 m/s (stale); corrected to 0.39 m/s; broken link to nonexistent `lidar_compatibility.md` replaced with upstream kaiaai/LDS URL
- `docs/roadmap.md` — v1.0 date was 2025 (typo); v1.1 completed items now checked off
- `ros2/rviz/navigation.rviz` — `Nav2Panel` was listed under Displays (wrong section); moved to Panels; added `/odometry/filtered`, TF, and Particle Cloud displays

## [1.1.0] — 2026-07-07

### Fixed (Critical)
- `firmware/src/safety.cpp` — e-stop latched permanently after startup; now cmd_vel timeout auto-clears when commands resume; battery/overcurrent still latch
- `firmware/src/safety.cpp` — ignore battery voltage 0V at boot (ADC not ready) to avoid false low-battery e-stop
- `firmware/src/microros_bridge.cpp` — `_lastCmdMs` initialized to 0 triggered immediate cmd_vel timeout; now set to `millis()` in `begin()`
- `firmware/src/pid.cpp` — skipped integral accumulation when Ki=0 (was silently accumulating unbounded integral that never affected output)
- `docs/wiring.md` — voltage divider resistors 100kΩ/220kΩ would put 8.6V on GPIO36 (above 3.3V ADC limit); corrected to 100kΩ/33kΩ
- `firmware/src/main.ino` — `MAX_VEL_MS = 0.61f` exceeded JGA25-370 physical maximum; corrected to 0.39 m/s
- `ros2/config/cartographer_lds_2d.lua` — IMU was disabled (`use_imu_data=false`); enabled with correct `tracking_frame = "imu_link"`
- `ros2/launch/slam.launch.py` — missing `robot_state_publisher` caused broken TF tree in standalone SLAM mode

### Added
- `firmware/src/main.ino` — OTA firmware update support (`setupOTA` + `ArduinoOTA.handle`)
- `firmware/src/ota_update.h` — progress %, error callback, hostname (`loki-esp32`)
- `firmware/test/test_encoder.cpp` — 6 host-side unit tests for tick-to-velocity math
- `.github/workflows/ros2_build.yml` — ROS2 colcon build + launch file validation CI
- `host/map_saver.py` — CLI flags (`--dir`, `--prefix`, `--interval`), auto-save loop, timeout

### Changed
- `firmware/src/safety.h` — added `_latchedFault` field to track latching vs auto-clearing faults
- `firmware/src/wifi_config.h` — added setup instructions and `AGENT_RECONNECT_MS`
- `ros2/urdf/robot.urdf.xacro` — added inertia macros (cylinder/sphere), caster wheel, wheel dynamics, LiDAR visual
- `ros2/config/ekf.yaml` — 50 Hz frequency, full process noise covariance matrix, `odom0_differential: false`
- `ros2/config/navigation.yaml` — DWB tuned for JGA25-370 physics (max_vel_x 0.22 m/s, acc 1.5 m/s², inflation 0.18 m)
- `ros2/launch/teleop.launch.py` — removed xterm dependency (not installed by default on RPi)
- `ros2/launch/slam.launch.py` — added robot_state_publisher for correct TF
- `ros2/package.xml` — added missing exec_depends (robot_localization, nav2_map_server, etc.)
- `ros2/CMakeLists.txt` — install sdf directory; use QUIET for optional packages
- `docs/wiring.md` — full GPIO table with notes, I2C device table with addresses, corrected voltage divider
- `docs/calibration.md` — fixed Ki reference (0.1→0.8 to match firmware), added ROS2 verification commands
- `docs/ros2_setup.md` — complete rewrite with correct symlink-based workspace setup and topic verification
- `scripts/setup_rpi.sh` — fixed systemd service (ExecStartPre source → wrapper script), build loki_robot package
- `README.md` / `README_EN.md` — added CI badge for PlatformIO Build + ROS2 Build

## [Unreleased] — 2026-07-05
### Added
- Gazebo SDF model (model.sdf) with realistic inertia, diff drive plugin, LiDAR sensor
- GitHub Actions CI/CD workflow for PlatformIO ESP32 build + artifact upload
- `docs/faq.md` — comprehensive FAQ (hardware, firmware, ROS2, 3D printing)
- `docs/roadmap.md` — v1.0–v2.0 feature roadmap with known limitations table
- `docs/troubleshooting.md` — ESP32 / RPi / electrical / mechanical troubleshooting
- `scripts/setup_rpi.sh` — automated RPi4 setup (ROS2 Iron, Nav2, micro_ros_agent service)
- `scripts/flash_esp32.sh` — one-command ESP32 flash + serial monitor
- `ros2/config/teleop_keyboard.yaml` — teleop speed configuration
- `ros2/config/self_drive_gazebo.yaml` — Gazebo self-drive parameters
- `ros2/launch/teleop.launch.py` — teleop_twist_keyboard launch file
- `ros2/launch/gazebo.launch.py` — Gazebo + Loki SDF model launch file
### Changed
- `ros2/config/navigation.yaml` — replaced with full production config from makerspet_loki
- `ros2/config/explore_lite.yaml` — replaced with real upstream config
- `ros2/sdf/makerspet_loki/model.config` — correct Gazebo metadata (Ilia O. / makerspet)

## [1.0.0] — 2026-04-25
### Added
- Full ESP32 firmware with microROS (micro_ros_kaia)
- Differential drive motor driver + encoder reader
- YDLidar X4 / LDS02RR driver via kaiaai/LDS
- BNO055 IMU + INA219 current sensor integration
- Differential drive odometry
- PID velocity controller with anti-windup
- Safety monitor (estop, low-batt, overcurrent)
- Battery monitor (LiPo 3S percentage)
- OTA firmware update support
- ROS2 Iron package (loki_robot)
- Cartographer 2D SLAM configuration
- Nav2 navigation stack configuration
- explore_lite autonomous frontier exploration
- EKF sensor fusion (odom + IMU)
- URDF robot model for Loki 200mm platform
- RViz2 navigation config
- Web telemetry dashboard (Flask)
- Map saver utility
- Complete wiring guide
- Assembly guide with print settings
- Calibration procedures
- ROS2 setup documentation
- System architecture SVG diagram
- PID and odometry unit tests (Unity)
