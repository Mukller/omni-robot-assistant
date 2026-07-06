cmake_minimum_required(VERSION 3.8)
project(loki_robot)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Runtime packages — not linked, just declared for rosdep/colcon awareness
find_package(nav2_bringup QUIET)
find_package(cartographer_ros QUIET)
find_package(robot_localization QUIET)
find_package(robot_state_publisher QUIET)

install(DIRECTORY config launch rviz sdf urdf
  DESTINATION share/${PROJECT_NAME})

ament_package()
