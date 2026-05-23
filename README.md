<div align="center">

[English](README_EN.md) • **Русский**

</div>

<div align="center">

# 🤖 Loki Robot Assistant

**Автономный домашний робот-ассистент на базе MakersPet Loki 200mm**
ESP32 + microROS · Raspberry Pi 4 · ROS2 Iron · Cartographer SLAM · Nav2

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)
[![ROS2 Iron](https://img.shields.io/badge/ROS2-Iron-blue)](https://docs.ros.org/en/iron/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange)](https://platformio.org/)
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%204-red)](https://www.raspberrypi.com/)

</div>

---

## О проекте

Loki Robot Assistant — автономный мобильный робот на базе платформы **MakersPet Loki 200mm**.
Двухколёсный дифференциальный привод с кастером, 360° LiDAR-сканирование, полный стек ROS2 Iron.

- **ESP32** управляет моторами, датчиками и LiDAR — через **microROS** передаёт данные по UDP
- **Raspberry Pi 4** запускает **ROS2 Iron**: SLAM, навигация, автономное исследование

---

## Архитектура системы

![System Architecture](docs/images/system_arch.svg)

---

## Видео и демо

| Картограф SLAM | Автономная навигация | Сборка платформы |
|:-:|:-:|:-:|
| [![SLAM](https://img.youtube.com/vi/WPB2B1DPf_s/mqdefault.jpg)](https://www.youtube.com/watch?v=WPB2B1DPf_s) | [![Nav2](https://img.youtube.com/vi/XOc5kCE3MC0/mqdefault.jpg)](https://www.youtube.com/watch?v=XOc5kCE3MC0) | [![Build](https://img.youtube.com/vi/L_XbkA4pwRc/mqdefault.jpg)](https://www.youtube.com/watch?v=L_XbkA4pwRc) |

---

## Аппаратное обеспечение

| Компонент | Кол-во | Цена |
|-----------|--------|------|
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
| **Итого** | | **~$264** |

Полный список с ссылками: [cad/bom.csv](cad/bom.csv)

---

## Стек прошивки ESP32

```
firmware/
├── config.h                  # Пины и константы
├── platformio.ini            # micro_ros_kaia, kaiaai/LDS, PID_Timed
└── src/
    ├── main.ino              # Главный цикл
    ├── motor_driver.*        # L298N управление
    ├── encoder.*             # Квадратурные энкодеры
    ├── odometry.*            # Дифференциальная одометрия
    ├── pid.*                 # ПИД-регулятор с анти-насыщением
    ├── lidar_driver.*        # YDLidar X4 / LDS02RR парсер
    ├── imu.*                 # BNO055 обёртка
    ├── current_sensor.*      # INA219
    ├── safety.*              # Estop / low-batt / overcurrent
    ├── battery_monitor.*     # LiPo 3S процент заряда
    ├── wifi_config.h         # WiFi + OTA
    └── microros_bridge.*     # microROS odom/scan/cmd_vel
```

---

## Стек ROS2 (Raspberry Pi 4)

```
ros2/
├── CMakeLists.txt
├── package.xml
├── config/
│   ├── cartographer_lds_2d.lua   # Cartographer SLAM (0.15–7m)
│   ├── navigation.yaml            # Nav2 + DWB controller
│   ├── explore_lite.yaml          # Автономное исследование
│   ├── ekf.yaml                   # EKF fusion odom + IMU
│   └── telem.yaml
├── launch/
│   ├── loki_bringup.launch.py    # microROS agent + SLAM + Nav2
│   ├── slam.launch.py
│   └── explore.launch.py
├── urdf/
│   └── robot.urdf.xacro          # Модель Loki 200mm
└── rviz/
    └── navigation.rviz
```

### Запуск

```bash
# На Raspberry Pi 4
source /opt/ros/iron/setup.bash
source ~/ros2_ws/install/setup.bash

# Полный запуск
ros2 launch loki_robot loki_bringup.launch.py

# Только SLAM
ros2 launch loki_robot slam.launch.py

# Автономное исследование
ros2 launch loki_robot explore.launch.py
```

---

## Навигационный автомат

![Navigation States](docs/images/nav_states.svg)

---

## Быстрый старт

### 1. Прошивка ESP32
```bash
cd firmware
pio run --target upload --environment esp32dev
```

### 2. Настройка Raspberry Pi 4
```bash
# Установка ROS2 Iron
sudo apt install -y ros-iron-desktop ros-iron-nav2-bringup \
    ros-iron-cartographer-ros ros-iron-explore-lite \
    ros-iron-micro-ros-agent

# Сборка пакета
mkdir -p ~/ros2_ws/src && cd ~/ros2_ws/src
git clone https://github.com/Mukller/omni-robot-assistant.git
cp -r omni-robot-assistant/ros2 loki_robot
cd ~/ros2_ws && colcon build && source install/setup.bash
```

### 3. Запуск
```bash
ros2 launch loki_robot loki_bringup.launch.py
```

---

## Документация

- [Схема подключения](docs/wiring.md)
- [Сборка](docs/assembly.md)
- [Калибровка](docs/calibration.md)
- [Настройка ROS2](docs/ros2_setup.md)
- [CAD источники](cad/SOURCES.md)

---

## Ссылки

- [MakersPet Loki 200mm](https://github.com/makerspet/makerspet_loki) — платформа
- [kaiaai/LDS](https://github.com/kaiaai/LDS) — библиотека LiDAR (20+ моделей)
- [micro-ROS Arduino](https://github.com/micro-ROS/micro_ros_arduino) — microROS для ESP32
- [Cartographer ROS](https://google-cartographer-ros.readthedocs.io/) — SLAM
- [Nav2](https://navigation.ros.org/) — навигационный стек
