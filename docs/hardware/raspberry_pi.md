# Raspberry Pi 4 — Настройка и конфигурация

## Характеристики

| Параметр | Значение |
|----------|----------|
| Модель | Raspberry Pi 4 Model B |
| RAM | 4GB (рекомендовано) |
| CPU | Cortex-A72 (ARM v8) @ 1.8GHz, 4 ядра |
| Питание | 5В, 3А через USB-C |
| Интерфейсы | WiFi 2.4/5GHz, BT 5.0, 2×USB 3.0, 2×USB 2.0 |
| GPIO | 40-pin |
| Хранение | microSD 32GB+ (Class 10) или SSD через USB |

## Установка ОС

Рекомендуется **Ubuntu Server 22.04 LTS (arm64)** — официально поддерживается ROS2 Iron.

```bash
# Записать образ через Raspberry Pi Imager
# → выбрать Ubuntu Server 22.04.x LTS (64-bit)
# → настроить WiFi и SSH перед записью (кнопка ⚙️)
```

## Первоначальная настройка

```bash
# Подключиться по SSH
ssh ubuntu@<IP-адрес>

# Обновить систему
sudo apt update && sudo apt upgrade -y

# Настроить hostname
sudo hostnamectl set-hostname loki-robot

# Увеличить swap (важно для сборки ROS2)
sudo dphys-swapfile swapoff
sudo nano /etc/dphys-swapfile  # CONF_SWAPSIZE=2048
sudo dphys-swapfile setup && sudo dphys-swapfile swapon
```

## Установка ROS2 Iron

```bash
# Настройка locale
sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8

# Добавление репозитория ROS2
sudo apt install -y software-properties-common curl
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
    -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
    http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" \
    | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Установка ROS2 Iron + пакетов
sudo apt update
sudo apt install -y \
    ros-iron-desktop \
    ros-iron-nav2-bringup \
    ros-iron-cartographer-ros \
    ros-iron-explore-lite \
    ros-iron-micro-ros-agent \
    ros-iron-robot-localization \
    ros-iron-robot-state-publisher \
    ros-iron-joint-state-publisher \
    ros-iron-tf2-tools \
    python3-colcon-common-extensions \
    python3-rosdep

# Инициализация rosdep
sudo rosdep init && rosdep update

# Добавить в .bashrc
echo "source /opt/ros/iron/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

## Сборка пакета loki_robot

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/Mukller/omni-robot-assistant.git
cp -r omni-robot-assistant/ros2 loki_robot

cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r -y
colcon build --packages-select loki_robot --symlink-install
source install/setup.bash

# Добавить в .bashrc
echo "source ~/ros2_ws/install/setup.bash" >> ~/.bashrc
```

## Автозапуск при загрузке (systemd)

```bash
sudo nano /etc/systemd/system/loki-robot.service
```

```ini
[Unit]
Description=Loki Robot ROS2 Bringup
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=ubuntu
Environment="HOME=/home/ubuntu"
ExecStartPre=/bin/bash -c "source /opt/ros/iron/setup.bash && source /home/ubuntu/ros2_ws/install/setup.bash"
ExecStart=/bin/bash -c "source /opt/ros/iron/setup.bash && source /home/ubuntu/ros2_ws/install/setup.bash && ros2 launch loki_robot loki_bringup.launch.py"
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable loki-robot
sudo systemctl start loki-robot
sudo journalctl -u loki-robot -f  # просмотр логов
```

## Параметры WiFi (microROS агент)

ESP32 подключается к Raspberry Pi по UDP. Убедитесь что:

1. ESP32 и RPi в одной WiFi сети
2. RPi статический IP или hostname в `/etc/hosts` ESP32
3. Порт 8888/UDP открыт (UFW: `sudo ufw allow 8888/udp`)

```bash
# Узнать IP Raspberry Pi
hostname -I
# Прописать в firmware/src/wifi_config.h:
# #define AGENT_IP "192.168.1.XXX"
```

## 7" HDMI Display

| Параметр | Значение |
|----------|----------|
| Подключение | micro HDMI → HDMI |
| Разрешение | 1024×600 |
| Питание | USB 5В |

```bash
# В /boot/firmware/config.txt добавить:
hdmi_group=2
hdmi_mode=87
hdmi_cvt 1024 600 60 6 0 0 0
hdmi_drive=1
```
