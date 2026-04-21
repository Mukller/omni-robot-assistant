#include "microros_bridge.h"
#include <micro_ros_arduino.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

bool MicroROSBridge::begin(const char* agentIP, int agentPort) {
    set_microros_wifi_transports(WIFI_SSID, WIFI_PASS, agentIP, agentPort);
    _alloc = rcl_get_default_allocator();
    rclc_support_init(&_support, 0, nullptr, &_alloc);
    rclc_node_init_default(&_node, "loki_esp32", "", &_support);

    rclc_publisher_init_default(&_odomPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "/odom");
    rclc_publisher_init_default(&_scanPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan), "/scan");
    rclc_subscription_init_default(&_cmdSub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/cmd_vel");

    rclc_executor_init(&_exec, &_support.context, 1, &_alloc);
    rclc_executor_add_subscription(&_exec, &_cmdSub, &_cmdMsg,
        [](const void* msg) {
            // static callback — store to global then update bridge fields
        }, ON_NEW_DATA);
    return true;
}

void MicroROSBridge::spinOnce() {
    rclc_executor_spin_some(&_exec, RCL_MS_TO_NS(10));
    cmdVx = _cmdMsg.linear.x;
    cmdWz = _cmdMsg.angular.z;
}

void MicroROSBridge::publishOdom(float x, float y, float theta, float vx, float wz) {
    _odomMsg.pose.pose.position.x = x;
    _odomMsg.pose.pose.position.y = y;
    _odomMsg.twist.twist.linear.x  = vx;
    _odomMsg.twist.twist.angular.z = wz;
    rcl_publish(&_odomPub, &_odomMsg, nullptr);
}

void MicroROSBridge::publishScan(const float* ranges, int count, float angleMin, float angleMax) {
    _scanMsg.angle_min = angleMin;
    _scanMsg.angle_max = angleMax;
    _scanMsg.angle_increment = (angleMax - angleMin) / count;
    _scanMsg.range_min = 0.15f;
    _scanMsg.range_max = 7.0f;
    rosidl_runtime_c__float__Sequence__init(&_scanMsg.ranges, count);
    memcpy(_scanMsg.ranges.data, ranges, count * sizeof(float));
    rcl_publish(&_scanPub, &_scanMsg, nullptr);
}
