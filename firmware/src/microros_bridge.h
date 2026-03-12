#pragma once
// microROS bridge — publishes odom, LaserScan, IMU; subscribes cmd_vel
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <nav_msgs/msg/odometry.h>
#include <sensor_msgs/msg/laser_scan.h>
#include <geometry_msgs/msg/twist.h>

class MicroROSBridge {
public:
    bool begin(const char* agentIP, int agentPort);
    void spinOnce();
    void publishOdom(float x, float y, float theta, float vx, float wz);
    void publishScan(const float* ranges, int count, float angleMin, float angleMax);

    // Latest cmd_vel received
    float cmdVx  = 0;
    float cmdWz  = 0;
    bool  hasNew = false;
private:
    rcl_node_t _node;
    rcl_publisher_t _odomPub, _scanPub;
    rcl_subscription_t _cmdSub;
    rclc_executor_t _exec;
    rclc_support_t  _support;
    rcl_allocator_t _alloc;
    nav_msgs__msg__Odometry    _odomMsg;
    sensor_msgs__msg__LaserScan _scanMsg;
    geometry_msgs__msg__Twist   _cmdMsg;
};
