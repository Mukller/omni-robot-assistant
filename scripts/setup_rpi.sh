#!/usr/bin/env bash
# Automated Raspberry Pi 4 setup for Loki 200mm robot
# Ubuntu 22.04 LTS + ROS2 Iron
# Run as regular user (not root): bash setup_rpi.sh

set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
info()    { echo -e "${GREEN}[INFO]${NC} $*"; }
warn()    { echo -e "${YELLOW}[WARN]${NC} $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*"; exit 1; }

# ─── Checks ───────────────────────────────────────────────────────────────────
[[ $(id -u) -eq 0 ]] && error "Run as regular user, not root."
[[ $(uname -m) == "aarch64" ]] || warn "Expected ARM64 (Raspberry Pi), got $(uname -m)"

UBUNTU_VER=$(lsb_release -rs 2>/dev/null || echo "unknown")
[[ "$UBUNTU_VER" == "22.04" ]] || warn "Tested on Ubuntu 22.04, got $UBUNTU_VER"

# ─── System update ────────────────────────────────────────────────────────────
info "Updating system packages..."
sudo apt-get update -q
sudo apt-get upgrade -y -q

# ─── ROS2 Iron ────────────────────────────────────────────────────────────────
if ! command -v ros2 &>/dev/null; then
  info "Installing ROS2 Iron..."

  sudo apt-get install -y curl gnupg lsb-release
  sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
    -o /usr/share/keyrings/ros-archive-keyring.gpg

  echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
    http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" \
    | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

  sudo apt-get update -q
  sudo apt-get install -y ros-iron-desktop
  info "ROS2 Iron installed."
else
  info "ROS2 already installed: $(ros2 --version 2>&1 | head -1)"
fi

# Source ROS2 in .bashrc
grep -q "source /opt/ros/iron/setup.bash" ~/.bashrc || \
  echo "source /opt/ros/iron/setup.bash" >> ~/.bashrc
source /opt/ros/iron/setup.bash

# ─── ROS2 packages ────────────────────────────────────────────────────────────
info "Installing ROS2 navigation and SLAM packages..."
sudo apt-get install -y \
  ros-iron-navigation2 \
  ros-iron-nav2-bringup \
  ros-iron-cartographer \
  ros-iron-cartographer-ros \
  ros-iron-slam-toolbox \
  ros-iron-robot-localization \
  ros-iron-teleop-twist-keyboard \
  ros-iron-tf2-tools \
  ros-iron-rqt \
  ros-iron-rqt-common-plugins \
  ros-iron-rviz2 \
  ros-iron-micro-ros-agent \
  python3-colcon-common-extensions \
  python3-rosdep

# ─── explore_lite ─────────────────────────────────────────────────────────────
info "Setting up ROS2 workspace..."
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src

# explore_lite (frontier exploration)
if [ ! -d "m-explore-ros2" ]; then
  git clone https://github.com/robo-friends/m-explore-ros2.git
fi

# loki_robot package (this repo's ROS2 code)
LOKI_SRC="$HOME/omni-robot-assistant/ros2"
if [ ! -L "loki_robot" ] && [ -d "$LOKI_SRC" ]; then
  ln -s "$LOKI_SRC" loki_robot
  info "Linked ros2/ → ~/ros2_ws/src/loki_robot"
fi

cd ~/ros2_ws
rosdep init 2>/dev/null || true
rosdep update
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
grep -q "source ~/ros2_ws/install/setup.bash" ~/.bashrc || \
  echo "source ~/ros2_ws/install/setup.bash" >> ~/.bashrc

# ─── microROS agent ───────────────────────────────────────────────────────────
info "Configuring micro_ros_agent systemd service..."
# NOTE: systemd services cannot use `source`; use a wrapper script instead.
cat > /tmp/microros_start.sh << 'EOF'
#!/bin/bash
source /opt/ros/iron/setup.bash
exec ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
EOF
chmod +x /tmp/microros_start.sh
sudo mv /tmp/microros_start.sh /usr/local/bin/microros_start.sh

cat > /tmp/microros.service << 'EOF'
[Unit]
Description=micro-ROS Agent (UDP port 8888)
After=network-online.target
Wants=network-online.target

[Service]
User=REPLACE_USER
Environment="ROS_DOMAIN_ID=0"
ExecStart=/usr/local/bin/microros_start.sh
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF
sed -i "s/REPLACE_USER/$USER/" /tmp/microros.service
sudo mv /tmp/microros.service /etc/systemd/system/microros.service
sudo systemctl daemon-reload
sudo systemctl enable microros.service
info "microros.service enabled (will start on reboot)."

# ─── Static IP for robot network ──────────────────────────────────────────────
info "Checking network configuration..."
IFACE=$(ip route | awk '/^default/ {print $5; exit}')
if [ -n "$IFACE" ]; then
  warn "Current interface: $IFACE. Configure static IP manually if needed."
  warn "Recommended: 192.168.1.100 for RPi, ESP32 → set AGENT_IP in firmware/src/wifi_config.h"
fi

# ─── Serial port permissions ──────────────────────────────────────────────────
sudo usermod -a -G dialout "$USER"
sudo usermod -a -G tty "$USER"

# ─── Performance tweaks ───────────────────────────────────────────────────────
info "Applying performance tweaks..."

# Increase GPU memory split
if ! grep -q "gpu_mem=128" /boot/firmware/config.txt 2>/dev/null; then
  echo "gpu_mem=128" | sudo tee -a /boot/firmware/config.txt
fi

# Increase swap to 2GB for colcon build
SWAP_FILE=/var/swap
if [ ! -f "$SWAP_FILE" ]; then
  sudo fallocate -l 2G $SWAP_FILE
  sudo chmod 600 $SWAP_FILE
  sudo mkswap $SWAP_FILE
  sudo swapon $SWAP_FILE
  echo "$SWAP_FILE none swap sw 0 0" | sudo tee -a /etc/fstab
fi

# ─── Aliases ──────────────────────────────────────────────────────────────────
cat >> ~/.bashrc << 'ALIASES'

# Loki robot aliases
alias loki-agent='ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888'
alias loki-teleop='ros2 run teleop_twist_keyboard teleop_twist_keyboard'
alias loki-rviz='rviz2 -d ~/ros2_ws/src/loki_robot/rviz/navigation.rviz'
alias loki-topics='ros2 topic list | grep -v /_'
alias loki-hz='ros2 topic hz /scan && ros2 topic hz /odom'
ALIASES

info "────────────────────────────────────────────────"
info "Setup complete! Next steps:"
info "1. Reboot: sudo reboot"
info "2. Flash ESP32: cd omni-robot-assistant && bash scripts/flash_esp32.sh"
info "3. Start agent: sudo systemctl start microros"
info "4. Launch robot: ros2 launch loki_robot robot.launch.py"
info "────────────────────────────────────────────────"
