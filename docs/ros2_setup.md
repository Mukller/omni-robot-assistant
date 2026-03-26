# ROS2 Iron Setup on Raspberry Pi 4

## Install ROS2 Iron
```bash
sudo apt update && sudo apt install -y locales
locale-gen en_US en_US.UTF-8
sudo apt install -y ros-iron-desktop ros-iron-nav2-bringup \
    ros-iron-cartographer-ros ros-iron-explore-lite \
    ros-iron-micro-ros-agent
```

## Build loki_robot Package
```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/Mukller/omni-robot-assistant.git
cp -r omni-robot-assistant/ros2 loki_robot
cd ~/ros2_ws
source /opt/ros/iron/setup.bash
colcon build --packages-select loki_robot
source install/setup.bash
```

## Launch Bringup
```bash
# Terminal 1 — start microROS agent (ESP32 connects over UDP)
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888

# Terminal 2 — Cartographer SLAM
ros2 launch loki_robot loki_bringup.launch.py

# Terminal 3 — autonomous exploration
ros2 launch explore_lite explore.launch.py
```

## Visualize in RViz2
```bash
rviz2 -d ~/ros2_ws/src/loki_robot/rviz/navigation.rviz
```
