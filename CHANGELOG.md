# Changelog

All notable changes to this project are documented here.
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

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
