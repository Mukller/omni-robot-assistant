cmake_minimum_required(VERSION 3.8)
project(loki_robot)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav2_bringup REQUIRED)
find_package(cartographer_ros REQUIRED)
find_package(explore_lite REQUIRED)

install(DIRECTORY config launch rviz urdf
  DESTINATION share/${PROJECT_NAME})

ament_package()
