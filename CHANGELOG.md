# Changelog

All notable changes to this project are documented here.
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

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
