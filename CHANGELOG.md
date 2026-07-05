# Changelog

All notable changes to this project are documented here.
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

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
