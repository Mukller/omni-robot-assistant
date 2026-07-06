# ROS2 Iron Setup on Raspberry Pi 4

> **Quick start:** run `bash scripts/setup_rpi.sh` — it installs everything below automatically.
> This page documents the manual steps for reference.

## 1. Install ROS2 Iron

```bash
sudo apt update && sudo apt install -y locales curl gnupg lsb-release
locale-gen en_US en_US.UTF-8

# Add ROS2 repo
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
  -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
  http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" \
  | sudo tee /etc/apt/sources.list.d/ros2.list

sudo apt update
sudo apt install -y ros-iron-desktop \
    ros-iron-navigation2 ros-iron-nav2-bringup \
    ros-iron-cartographer ros-iron-cartographer-ros \
    ros-iron-robot-localization \
    ros-iron-micro-ros-agent \
    python3-colcon-common-extensions python3-rosdep
```

## 2. Build Workspace

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src

# Clone the repo and link ros2/ as the ROS2 package
git clone https://github.com/Mukller/omni-robot-assistant.git
ln -s omni-robot-assistant/ros2 loki_robot

# explore_lite (not in apt — build from source)
git clone https://github.com/robo-friends/m-explore-ros2.git

cd ~/ros2_ws
source /opt/ros/iron/setup.bash
rosdep init 2>/dev/null || true
rosdep update
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash
echo "source ~/ros2_ws/install/setup.bash" >> ~/.bashrc
```

## 3. Configure micro-ROS Agent (systemd)

```bash
# Runs automatically after setup_rpi.sh — manual start:
sudo systemctl start microros
sudo systemctl status microros
```

Or manually:
```bash
source /opt/ros/iron/setup.bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

## 4. Launch Sequences

### Full bringup (SLAM + Nav2 + explore_lite)
```bash
# Terminal 1
ros2 launch loki_robot loki_bringup.launch.py

# Terminal 2 (after map starts appearing in RViz)
ros2 launch loki_robot explore.launch.py
```

### SLAM only (no Nav2)
```bash
ros2 launch loki_robot slam.launch.py
```

### Teleoperation
```bash
# In a terminal with keyboard focus:
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

### Save map
```bash
python3 ~/ros2_ws/src/omni-robot-assistant/host/map_saver.py --dir ~/maps
# Auto-save every 30s during mapping:
python3 ~/ros2_ws/src/omni-robot-assistant/host/map_saver.py --interval 30
```

## 5. Visualize in RViz2

```bash
rviz2 -d ~/ros2_ws/install/loki_robot/share/loki_robot/rviz/navigation.rviz
```

## 6. Verify Topics

```bash
ros2 topic hz /scan              # ~8 Hz from YDLidar X4
ros2 topic hz /odom              # loop rate (~20 Hz)
ros2 topic hz /imu/data          # 50 Hz
ros2 topic hz /odometry/filtered # 50 Hz (EKF output)
ros2 topic hz /map               # ~0.3 Hz (Cartographer publishes slowly)
```
