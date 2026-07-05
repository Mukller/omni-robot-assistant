#pragma once
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <nav_msgs/msg/odometry.h>
#include <sensor_msgs/msg/laser_scan.h>
#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/battery_state.h>
#include <geometry_msgs/msg/twist.h>

class MicroROSBridge {
public:
    bool begin(const char* agentIP, int agentPort);
    void spinOnce();

    void publishOdom(float x, float y, float theta, float vx, float wz);
    void publishScan(const float* ranges, int count, float angleMin, float angleMax);
    void publishImu(float ax, float ay, float az,
                    float gx, float gy, float gz,
                    float qw, float qx, float qy, float qz);
    void publishBattery(float voltage, float percent, float current);

    // Latest cmd_vel — updated in subscription callback
    float cmdVx  = 0.0f;
    float cmdWz  = 0.0f;
    bool  hasNew = false;

    // Returns millis() of last received cmd_vel (for watchdog)
    unsigned long lastCmdMs() const { return _lastCmdMs; }

private:
    static void _cmdCallback(const void* msg);
    static MicroROSBridge* _instance;

    rcl_node_t       _node;
    rcl_publisher_t  _odomPub, _scanPub, _imuPub, _battPub;
    rcl_subscription_t _cmdSub;
    rclc_executor_t  _exec;
    rclc_support_t   _support;
    rcl_allocator_t  _alloc;

    nav_msgs__msg__Odometry       _odomMsg;
    sensor_msgs__msg__LaserScan   _scanMsg;
    sensor_msgs__msg__Imu         _imuMsg;
    sensor_msgs__msg__BatteryState _battMsg;
    geometry_msgs__msg__Twist     _cmdMsg;

    unsigned long _lastCmdMs = 0;

    static constexpr int SCAN_POINTS = 360;
    float _scanBuf[SCAN_POINTS];  // pre-allocated, no heap churn
};
