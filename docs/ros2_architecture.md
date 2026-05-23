# ROS2 Architecture — Подробное описание

## Топологическая схема нод

```
ESP32 (microROS)
   │ UDP :8888
   ▼
micro_ros_agent ──────────────────────────────────────────────────────┐
   │                                                                   │
   ├─ Publishes: /odom (nav_msgs/Odometry)                             │
   ├─ Publishes: /scan (sensor_msgs/LaserScan)                         │
   ├─ Publishes: /imu/data (sensor_msgs/Imu)                           │
   ├─ Publishes: /battery_state (sensor_msgs/BatteryState)             │
   └─ Subscribes: /cmd_vel (geometry_msgs/Twist)                       │
                                                                       │
robot_state_publisher                                                  │
   └─ Publishes: /robot_description, /tf (base_link, laser, wheels)    │
                                                                       │
ekf_filter_node (robot_localization)                                   │
   ├─ Input: /odom + /imu/data                                         │
   └─ Output: /odometry/filtered → используется Nav2 как /odom         │
                                                                       │
cartographer_node                                                      │
   ├─ Input: /scan, /odom (или /odometry/filtered)                     │
   ├─ Output: /map (nav_msgs/OccupancyGrid)                            │
   └─ Output: /tf (map → odom)                                         │
                                                                       │
cartographer_occupancy_grid_node                                       │
   ├─ Input: /map                                                       │
   └─ Output: /map (периодически обновляет)                            │
                                                                       │
Nav2 стек:                                                             │
   ├─ amcl → /tf (map → odom), /particle_cloud                        │
   ├─ controller_server → /cmd_vel ──────────────────────────────────►┘
   ├─ planner_server → глобальный путь
   ├─ bt_navigator → выполнение поведенческого дерева
   ├─ waypoint_follower → следование точкам маршрута
   └─ map_server → /map

explore_lite
   ├─ Input: /map, /tf
   └─ Output: навигационные цели → bt_navigator
```

## Топики ROS2

| Топик | Тип | Кто публикует | Кто подписывается |
|-------|-----|---------------|-------------------|
| `/scan` | `LaserScan` | ESP32/microROS | Cartographer, Nav2 costmap |
| `/odom` | `Odometry` | ESP32/microROS | EKF, Cartographer |
| `/imu/data` | `Imu` | ESP32/microROS | EKF |
| `/battery_state` | `BatteryState` | ESP32/microROS | Web monitor |
| `/cmd_vel` | `Twist` | controller_server | ESP32/microROS |
| `/map` | `OccupancyGrid` | Cartographer | Nav2 planners, RViz |
| `/tf` | `TFMessage` | Cartographer, robot_state_pub | Все |
| `/odometry/filtered` | `Odometry` | EKF | Nav2, Cartographer |
| `/goal_pose` | `PoseStamped` | RViz (вручную) | bt_navigator |
| `/robot_description` | `String` | robot_state_publisher | RViz |

## TF-дерево

```
map
 └── odom  (Cartographer публикует: map → odom)
      └── base_footprint  (EKF / robot_state_publisher)
           └── base_link
                ├── laser  (фиксированный, центр верхней платы)
                ├── imu_link  (фиксированный, центр шасси)
                ├── wheel_left_link  (вращающийся)
                └── wheel_right_link  (вращающийся)
```

## Параметры Cartographer

Файл: `config/cartographer_lds_2d.lua`

| Параметр | Значение | Описание |
|----------|----------|----------|
| `min_range` | 0.15 м | Минимальная дальность сканирования |
| `max_range` | 7.0 м | Максимальная дальность |
| `use_imu_data` | false | ИМУ не используется в 2D режиме |
| `use_online_correlative_scan_matching` | true | Улучшает качество в реальном времени |
| `constraint_builder.min_score` | 0.65 | Порог для создания loop-closure связей |
| `optimize_every_n_nodes` | 90 | Частота оптимизации позового графа |

## Параметры Nav2

Файл: `config/navigation.yaml`

### DWB Controller (local planner)

| Параметр | Значение |
|----------|----------|
| `max_vel_x` | 0.26 м/с |
| `max_vel_theta` | 1.0 рад/с |
| `acc_lim_x` | 2.5 м/с² |
| `sim_time` | 1.7 с |
| `vx_samples` | 20 |
| `vtheta_samples` | 40 |

### Local costmap

| Параметр | Значение |
|----------|----------|
| Размер окна | 3×3 м |
| Разрешение | 0.05 м/ячейку |
| `robot_radius` | 0.12 м |

## Запуск отдельных компонентов

```bash
# Только microROS агент (ESP32 подключается автоматически)
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888

# Только Cartographer SLAM
ros2 launch loki_robot slam.launch.py

# Только навигация (нужна готовая карта)
ros2 launch nav2_bringup localization_launch.py \
    map:=/home/pi/maps/room.yaml \
    params_file:=$HOME/ros2_ws/install/loki_robot/share/loki_robot/config/navigation.yaml

# Автономное исследование
ros2 launch loki_robot explore.launch.py

# Сохранение карты
ros2 run nav2_map_server map_saver_cli -f ~/maps/room

# Просмотр в RViz2
rviz2 -d $HOME/ros2_ws/install/loki_robot/share/loki_robot/rviz/navigation.rviz
```

## Отладка

```bash
# Список активных топиков
ros2 topic list

# Частота поступления скана
ros2 topic hz /scan

# Просмотр одометрии
ros2 topic echo /odom --once

# Проверка TF-дерева
ros2 run tf2_tools view_frames

# Статус Nav2
ros2 lifecycle_node list
```
