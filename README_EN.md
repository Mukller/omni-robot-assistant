<div align="center">

**English** • [Русский](README.md)

</div>

<div align="center">

# 🤖 Loki Robot Assistant

**Autonomous home robot assistant based on MakersPet Loki 200mm**
ESP32 + microROS · Raspberry Pi 4 · ROS2 Iron · Cartographer SLAM · Nav2

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)
[![ROS2 Iron](https://img.shields.io/badge/ROS2-Iron-blue)](https://docs.ros.org/en/iron/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange)](https://platformio.org/)
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%204-red)](https://www.raspberrypi.com/)

</div>

---

## About

Loki Robot Assistant is an autonomous mobile robot built on the **MakersPet Loki 200mm** platform.
Differential drive with caster wheel, 360° LiDAR scanning, full ROS2 Iron stack.

- **ESP32** controls motors, sensors, and LiDAR — publishes data over UDP via **microROS**
- **Raspberry Pi 4** runs **ROS2 Iron**: SLAM, autonomous navigation, frontier exploration

---

## System Architecture

![System Architecture](docs/images/system_arch.svg)

---

## Video & Demo

| Cartographer SLAM | Autonomous Navigation | Platform Build |
|:-:|:-:|:-:|
| [![SLAM](https://img.youtube.com/vi/WPB2B1DPf_s/mqdefault.jpg)](https://www.youtube.com/watch?v=WPB2B1DPf_s) | [![Nav2](https://img.youtube.com/vi/XOc5kCE3MC0/mqdefault.jpg)](https://www.youtube.com/watch?v=XOc5kCE3MC0) | [![Build](https://img.youtube.com/vi/L_XbkA4pwRc/mqdefault.jpg)](https://www.youtube.com/watch?v=L_XbkA4pwRc) |

---

## Hardware BOM

| Component | Qty | Price |
|-----------|-----|-------|
| Raspberry Pi 4 (4GB) | 1 | $55 |
| ESP32 DevKit v1 | 1 | $4.50 |
| MakersPet Loki 200mm frame | 1 | $45 |
| JGA25-370 DC gear motor 12V | 2 | $14 |
| Omni wheel 48mm | 2 | $7 |
| Caster ball | 1 | $2 |
| L298N motor driver | 1 | $2.50 |
| YDLidar X4 (360°) | 1 | $55 |
| BNO055 IMU | 1 | $3.50 |
| INA219 current sensor | 2 | $2.40 |
| LiPo 11.1V 3000mAh | 1 | $18 |
| 7" HDMI display | 1 | $35 |
| **Total** | | **~$264** |

Full BOM with links: [cad/bom.csv](cad/bom.csv)

---

## ESP32 Firmware Stack

```
firmware/
├── config.h                  # Pin definitions and constants
├── platformio.ini            # micro_ros_kaia, kaiaai/LDS, PID_Timed
└── src/
    ├── main.ino              # Main loop
    ├── motor_driver.*        # L298N motor control
    ├── encoder.*             # Quadrature encoder reader
    ├── odometry.*            # Differential drive odometry
    ├── pid.*                 # PID controller with anti-windup
    ├── lidar_driver.*        # YDLidar X4 / LDS02RR parser
    ├── imu.*                 # BNO055 wrapper
    ├── current_sensor.*      # INA219
    ├── safety.*              # Estop / low-batt / overcurrent
    ├── battery_monitor.*     # LiPo 3S percentage
    ├── wifi_config.h         # WiFi + OTA
    └── microros_bridge.*     # microROS odom/scan/cmd_vel
```

---

## ROS2 Stack (Raspberry Pi 4)

```
ros2/
├── CMakeLists.txt
├── package.xml
├── config/
│   ├── cartographer_lds_2d.lua   # Cartographer SLAM (0.15–7m)
│   ├── navigation.yaml            # Nav2 + DWB controller
│   ├── explore_lite.yaml          # Autonomous frontier exploration
│   ├── ekf.yaml                   # EKF fusion odom + IMU
│   └── telem.yaml
├── launch/
│   ├── loki_bringup.launch.py
│   ├── slam.launch.py
│   └── explore.launch.py
├── urdf/robot.urdf.xacro
└── rviz/navigation.rviz
```

### Quick Launch

```bash
# On Raspberry Pi 4
source /opt/ros/iron/setup.bash && source ~/ros2_ws/install/setup.bash
ros2 launch loki_robot loki_bringup.launch.py
```

---

## Navigation State Machine

![Navigation States](docs/images/nav_states.svg)

---

## Quick Start

### 1. Flash ESP32
```bash
cd firmware && pio run --target upload --environment esp32dev
```

### 2. Setup Raspberry Pi 4
```bash
sudo apt install -y ros-iron-desktop ros-iron-nav2-bringup \
    ros-iron-cartographer-ros ros-iron-explore-lite ros-iron-micro-ros-agent
mkdir -p ~/ros2_ws/src && cd ~/ros2_ws/src
git clone https://github.com/Mukller/omni-robot-assistant.git
cp -r omni-robot-assistant/ros2 loki_robot
cd ~/ros2_ws && colcon build && source install/setup.bash
```

### 3. Launch
```bash
ros2 launch loki_robot loki_bringup.launch.py
```

---

## Documentation

- [Wiring Guide](docs/wiring.md)
- [Assembly Guide](docs/assembly.md)
- [Calibration](docs/calibration.md)
- [ROS2 Setup](docs/ros2_setup.md)
- [CAD Sources](cad/SOURCES.md)

---

## References

- [MakersPet Loki 200mm](https://github.com/makerspet/makerspet_loki) — robot platform
- [kaiaai/LDS](https://github.com/kaiaai/LDS) — LiDAR library (20+ models)
- [micro-ROS Arduino](https://github.com/micro-ROS/micro_ros_arduino) — microROS for ESP32
- [Cartographer ROS](https://google-cartographer-ros.readthedocs.io/) — SLAM
- [Nav2](https://navigation.ros.org/) — navigation stack
