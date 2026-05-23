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
[![kaiaai/LDS](https://img.shields.io/badge/LiDAR-kaiaai%2FLDS-informational)](https://github.com/kaiaai/LDS)

</div>

---

## О проекте

Loki Robot Assistant — автономный мобильный робот на базе платформы **MakersPet Loki 200mm**.
Двухколёсный дифференциальный привод с кастером, 360° LiDAR-сканирование, полный стек ROS2 Iron.

- **ESP32** управляет моторами, датчиками и LiDAR — через **microROS** передаёт данные по UDP
- **Raspberry Pi 4** запускает **ROS2 Iron**: Cartographer SLAM, навигация Nav2, автономное исследование
- Полностью 3D-печатаемый корпус, открытое железо и ПО

---

## Фотографии

<div align="center">

![Loki Robot Platform](https://github.com/makerspet/makerspet_loki/assets/143911662/9e3857b0-df87-4fda-9d94-bfc53fed399d)

*MakersPet Loki 200mm — базовая платформа*

</div>

| Вид сверху | Архитектура системы |
|:-:|:-:|
| ![Top View](docs/images/robot_top_view.svg) | ![System Architecture](docs/images/system_arch.svg) |

---

## Видео и демо

| Сборка платформы | Настройка ПО | Первый запуск | ESP32 LiDAR |
|:-:|:-:|:-:|:-:|
| [![Assembly](https://img.youtube.com/vi/WPB2B1DPf_s/mqdefault.jpg)](https://www.youtube.com/watch?v=WPB2B1DPf_s) | [![Setup](https://img.youtube.com/vi/XOc5kCE3MC0/mqdefault.jpg)](https://www.youtube.com/watch?v=XOc5kCE3MC0) | [![Bringup](https://img.youtube.com/vi/L_XbkA4pwRc/mqdefault.jpg)](https://www.youtube.com/watch?v=L_XbkA4pwRc) | [![ESP32](https://img.youtube.com/vi/zizGI8MjANU/mqdefault.jpg)](https://www.youtube.com/watch?v=zizGI8MjANU) |

---

## Аппаратное обеспечение

| Компонент | Кол-во | Цена | Ссылка |
|-----------|--------|------|--------|
| Raspberry Pi 4 (4GB) | 1 | $55 | [raspberrypi.com](https://www.raspberrypi.com/) |
| ESP32 DevKit v1 | 1 | $4.50 | AliExpress |
| MakersPet Loki 200mm frame | 1 | $45 | [makerspet.com](https://makerspet.com/store) |
| JGA25-370 DC gear motor 12V, 40:1 | 2 | $7 × 2 | AliExpress |
| Omni wheel 48mm | 2 | $3.50 × 2 | AliExpress |
| Caster ball 15mm | 1 | $2 | AliExpress |
| L298N motor driver | 1 | $2.50 | AliExpress |
| **YDLidar X4** (360°, 10м) | 1 | $55 | [Amazon](https://www.amazon.com/s?k=ydlidar+x4) |
| BNO055 IMU (9-DOF) | 1 | $3.50 | AliExpress |
| INA219 current sensor | 2 | $1.20 × 2 | AliExpress |
| LiPo 11.1V 3S 3000мАч | 1 | $18 | Магазин RC |
| 7" HDMI дисплей | 1 | $35 | AliExpress |
| USB wide-angle камера | 1 | $12 | AliExpress |
| Болты, провода, разъёмы | — | $8 | Локально |
| **Итого** | | **~$264** | |

> LiDAR можно заменить на **Xiaomi LDS02RR** (~$15 б/у с AliExpress) или **RPLIDAR A1** (~$99).
> Все модели поддерживаются библиотекой [kaiaai/LDS](https://github.com/kaiaai/LDS) без изменения кода.

Полный список с характеристиками: [cad/bom.csv](cad/bom.csv)

---

## Архитектура системы

![System Architecture](docs/images/system_arch.svg)

### Стек прошивки ESP32

```
firmware/
├── config.h                  # Пины ESP32 и константы платформы
├── platformio.ini            # micro_ros_kaia · kaiaai/LDS · PID_Timed · ESPAsyncWebServer
└── src/
    ├── main.ino              # Главный цикл: 50 Гц управление
    ├── motor_driver.*        # L298N PWM управление (coast / brake)
    ├── encoder.*             # Квадратурный энкодер (IRAM ISR)
    ├── odometry.*            # Дифференциальная одометрия (1920 тиков/об)
    ├── pid.*                 # ПИД с анти-насыщением интеграла
    ├── lidar_driver.*        # Обёртка kaiaai/LDS (YDLidar X4 / LDS02RR / RPLIDAR A1 …)
    ├── imu.*                 # BNO055 Adafruit — режим NDOF fusion
    ├── current_sensor.*      # INA219 — мониторинг тока моторов
    ├── safety.*              # E-stop / low-batt / overcurrent watchdog
    ├── battery_monitor.*     # LiPo 3S процент по ADC
    ├── wifi_config.h         # WiFi SSID/PASS + OTA пароль
    └── microros_bridge.*     # microROS: /odom · /scan · /imu/data ← /cmd_vel
```

### Стек ROS2 (Raspberry Pi 4)

```
ros2/
├── CMakeLists.txt · package.xml
├── config/
│   ├── cartographer_lds_2d.lua   # Cartographer: min 0.15м, max 7м
│   ├── navigation.yaml            # Nav2 DWB + costmap 3×3м
│   ├── explore_lite.yaml          # Автономное исследование фронтиров
│   ├── ekf.yaml                   # EKF: odom + IMU → /odometry/filtered
│   └── telem.yaml                 # robot_state_publisher 30 Гц
├── launch/
│   ├── loki_bringup.launch.py    # microROS agent + Cartographer + Nav2
│   ├── slam.launch.py             # Только SLAM
│   └── explore.launch.py          # Только explore_lite
├── urdf/robot.urdf.xacro          # Kинематическая модель Loki 200mm
└── rviz/navigation.rviz           # Карта + скан + одометрия
```

---

## Поддерживаемые LiDAR

Библиотека [kaiaai/LDS](https://github.com/kaiaai/LDS) поддерживает **20+ моделей** без изменения кода ROS2:

| Модель | Дальность | Цена | Рекомендация |
|--------|-----------|------|-------------|
| **YDLidar X4** | 10 м | ~$60 | ✅ Рекомендован |
| RPLIDAR A1 | 12 м | ~$99 | Лучшее качество |
| Xiaomi LDS02RR | 6 м | ~$15 (б/у) | Самый дешёвый |
| Neato XV11 | 6 м | ~$20 (б/у) | |
| YDLidar X3 | 8 м | ~$40 | Бюджет |
| LDROBOT LD14P | 8 м | ~$40 | |

Полная таблица совместимости: [docs/lidar_compatibility.md](docs/lidar_compatibility.md)

---

## Навигационный автомат

![Navigation States](docs/images/nav_states.svg)

---

## Быстрый старт

### 1. Прошивка ESP32
```bash
# Настроить WiFi в firmware/src/wifi_config.h
cd firmware
pio run --target upload --environment esp32dev
pio device monitor   # проверить подключение
```

### 2. Настройка Raspberry Pi 4

```bash
# Установка ROS2 Iron
sudo apt install -y ros-iron-desktop ros-iron-nav2-bringup \
    ros-iron-cartographer-ros ros-iron-explore-lite \
    ros-iron-micro-ros-agent ros-iron-robot-localization

# Сборка пакета
mkdir -p ~/ros2_ws/src && cd ~/ros2_ws/src
git clone https://github.com/Mukller/omni-robot-assistant.git
cp -r omni-robot-assistant/ros2 loki_robot
cd ~/ros2_ws
colcon build && source install/setup.bash
```

### 3. Запуск

```bash
# Полный стек (SLAM + Nav2 + microROS agent)
ros2 launch loki_robot loki_bringup.launch.py

# Автономное исследование
ros2 launch loki_robot explore.launch.py

# Сохранить карту
python3 host/map_saver.py

# Веб-интерфейс телеметрии
python3 host/web_monitor.py  # → http://robot-ip:5000
```

---

## Документация

| Документ | Описание |
|----------|----------|
| [docs/wiring.md](docs/wiring.md) | Полная схема подключения |
| [docs/assembly.md](docs/assembly.md) | Сборка с параметрами печати |
| [docs/calibration.md](docs/calibration.md) | Калибровка энкодеров, IMU, PID |
| [docs/ros2_setup.md](docs/ros2_setup.md) | Установка ROS2 Iron на RPi 4 |
| [docs/ros2_architecture.md](docs/ros2_architecture.md) | Топология нод, топики, TF-дерево |
| [docs/lidar_compatibility.md](docs/lidar_compatibility.md) | Таблица совместимых LiDAR |
| [docs/stl_files.md](docs/stl_files.md) | Список STL с настройками печати |
| [docs/hardware/ydlidar_x4.md](docs/hardware/ydlidar_x4.md) | Характеристики YDLidar X4 |
| [docs/hardware/motors_and_wheels.md](docs/hardware/motors_and_wheels.md) | Моторы, колёса, L298N |
| [docs/hardware/sensors.md](docs/hardware/sensors.md) | BNO055, INA219, аккумулятор |
| [docs/hardware/raspberry_pi.md](docs/hardware/raspberry_pi.md) | Настройка RPi 4 + systemd |
| [cad/bom.csv](cad/bom.csv) | Полный BOM с ценами |
| [cad/SOURCES.md](cad/SOURCES.md) | CAD-источники, STL-ссылки |

---

## Ссылки и источники

- [MakersPet Loki 200mm](https://github.com/makerspet/makerspet_loki) — платформа, STL, схемы
- [MakersPet 3D Models](https://github.com/makerspet/3d_models/tree/main/loki_200mm) — STL для всех вариантов
- [kaiaai/LDS](https://github.com/kaiaai/LDS) — Arduino LiDAR библиотека (20+ моделей)
- [micro-ROS Arduino](https://github.com/micro-ROS/micro_ros_arduino) — microROS для ESP32
- [Cartographer ROS](https://google-cartographer-ros.readthedocs.io/) — SLAM документация
- [Nav2](https://navigation.ros.org/) — навигационный стек ROS2
- [Awesome 2D LiDARs](https://github.com/kaiaai/awesome-2d-lidars) — каталог всех 2D LiDAR
- [MakersPet PCB](https://github.com/makerspet/pcb) — KiCad схемы и PCB
- [Braccio MoveIt](https://github.com/lots-of-things/braccio_moveit_gazebo) — Braccio рука (будущее расширение)
