#include "microros_bridge.h"
#include "wifi_config.h"
#include <micro_ros_arduino.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <Arduino.h>
#include <math.h>

MicroROSBridge* MicroROSBridge::_instance = nullptr;

// Static subscription callback — rclc requires a plain function pointer
void MicroROSBridge::_cmdCallback(const void* msg) {
    if (!_instance) return;
    const geometry_msgs__msg__Twist* twist =
        reinterpret_cast<const geometry_msgs__msg__Twist*>(msg);
    _instance->cmdVx     = twist->linear.x;
    _instance->cmdWz     = twist->angular.z;
    _instance->hasNew    = true;
    _instance->_lastCmdMs = millis();
}

bool MicroROSBridge::begin(const char* agentIP, int agentPort) {
    _instance  = this;
    _lastCmdMs = millis();  // prevents instant cmd_vel timeout on startup

    // Connect WiFi + establish UDP transport to micro_ros_agent
    set_microros_wifi_transports(WIFI_SSID, WIFI_PASS, agentIP, agentPort);

    _alloc = rcl_get_default_allocator();
    rclc_support_init(&_support, 0, nullptr, &_alloc);
    rclc_node_init_default(&_node, "loki_esp32", "", &_support);

    // Publishers
    rclc_publisher_init_default(&_odomPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "/odom");
    rclc_publisher_init_default(&_scanPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan), "/scan");
    rclc_publisher_init_default(&_imuPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu), "/imu/data");
    rclc_publisher_init_default(&_battPub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "/battery_state");

    // Subscriber
    rclc_subscription_init_default(&_cmdSub, &_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/cmd_vel");

    // Executor with 1 subscription handle
    rclc_executor_init(&_exec, &_support.context, 1, &_alloc);
    rclc_executor_add_subscription(&_exec, &_cmdSub, &_cmdMsg,
        &MicroROSBridge::_cmdCallback, ON_NEW_DATA);

    // Pre-allocate scan buffer once — avoids heap fragmentation
    _scanMsg.ranges.data     = _scanBuf;
    _scanMsg.ranges.size     = SCAN_POINTS;
    _scanMsg.ranges.capacity = SCAN_POINTS;

    // Static scan fields
    _scanMsg.header.frame_id.data = (char*)"laser";
    _scanMsg.range_min = 0.12f;
    _scanMsg.range_max = 10.0f;
    _scanMsg.time_increment = 0.0f;
    _scanMsg.scan_time      = 1.0f / 8.0f;  // 8 Hz YDLidar X4

    // IMU covariance: unknown (-1 diagonal)
    _imuMsg.orientation_covariance[0]         = -1.0;
    _imuMsg.angular_velocity_covariance[0]    = -1.0;
    _imuMsg.linear_acceleration_covariance[0] = -1.0;
    _imuMsg.header.frame_id.data = (char*)"imu_link";

    return true;
}

void MicroROSBridge::spinOnce() {
    rclc_executor_spin_some(&_exec, RCL_MS_TO_NS(5));
}

void MicroROSBridge::publishOdom(float x, float y, float theta, float vx, float wz) {
    _odomMsg.header.frame_id.data       = (char*)"odom";
    _odomMsg.child_frame_id.data        = (char*)"base_link";
    _odomMsg.pose.pose.position.x       = x;
    _odomMsg.pose.pose.position.y       = y;
    _odomMsg.pose.pose.position.z       = 0.0;

    // theta (yaw) → unit quaternion: q = (0, 0, sin(θ/2), cos(θ/2))
    _odomMsg.pose.pose.orientation.x    = 0.0;
    _odomMsg.pose.pose.orientation.y    = 0.0;
    _odomMsg.pose.pose.orientation.z    = sinf(theta * 0.5f);
    _odomMsg.pose.pose.orientation.w    = cosf(theta * 0.5f);

    _odomMsg.twist.twist.linear.x       = vx;
    _odomMsg.twist.twist.angular.z      = wz;

    rcl_publish(&_odomPub, &_odomMsg, nullptr);
}

void MicroROSBridge::publishScan(const float* ranges, int count, float angleMin, float angleMax) {
    int n = min(count, SCAN_POINTS);
    _scanMsg.angle_min       = angleMin;
    _scanMsg.angle_max       = angleMax;
    _scanMsg.angle_increment = (angleMax - angleMin) / n;
    _scanMsg.ranges.size     = n;
    memcpy(_scanBuf, ranges, n * sizeof(float));
    rcl_publish(&_scanPub, &_scanMsg, nullptr);
}

void MicroROSBridge::publishImu(float ax, float ay, float az,
                                 float gx, float gy, float gz,
                                 float qw, float qx, float qy, float qz) {
    _imuMsg.orientation.w               = qw;
    _imuMsg.orientation.x               = qx;
    _imuMsg.orientation.y               = qy;
    _imuMsg.orientation.z               = qz;
    _imuMsg.angular_velocity.x          = gx;
    _imuMsg.angular_velocity.y          = gy;
    _imuMsg.angular_velocity.z          = gz;
    _imuMsg.linear_acceleration.x       = ax;
    _imuMsg.linear_acceleration.y       = ay;
    _imuMsg.linear_acceleration.z       = az;
    rcl_publish(&_imuPub, &_imuMsg, nullptr);
}

void MicroROSBridge::publishBattery(float voltage, float percent, float current) {
    _battMsg.voltage    = voltage;
    _battMsg.percentage = percent / 100.0f;  // ROS convention: 0.0–1.0
    _battMsg.current    = current;
    _battMsg.present    = true;
    _battMsg.power_supply_technology = 3;    // POWER_SUPPLY_TECHNOLOGY_LIPO
    rcl_publish(&_battPub, &_battMsg, nullptr);
}
