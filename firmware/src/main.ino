#include <Arduino.h>
#include "../config.h"
#include "motor_driver.h"
#include "encoder.h"
#include "odometry.h"
#include "pid.h"
#include "imu.h"
#include "lidar_driver.h"
#include "current_sensor.h"
#include "microros_bridge.h"

MotorDriver motorL(MOT_L_IN1, MOT_L_IN2, MOT_L_EN);
MotorDriver motorR(MOT_R_IN1, MOT_R_IN2, MOT_R_EN);
Encoder     encL(ENC_L_A, ENC_L_B);
Encoder     encR(ENC_R_A, ENC_R_B);
Odometry    odom(WHEEL_BASE_M, WHEEL_RADIUS_M, TICKS_PER_REV);
PID         pidL(2.5f, 0.1f, 0.05f, -255, 255);
PID         pidR(2.5f, 0.1f, 0.05f, -255, 255);
IMU         imu;
LidarDriver lidar(Serial2, LIDAR_RX, LIDAR_TX);
CurrentSensor csL(INA_ADDR_L), csR(INA_ADDR_R);
MicroROSBridge ros;

unsigned long lastMs = 0;

void setup() {
    Serial.begin(115200);
    motorL.begin(); motorR.begin();
    encL.begin();   encR.begin();
    imu.begin();
    lidar.begin();
    csL.begin(); csR.begin();
    ros.begin("192.168.1.100", 8888);
}

void loop() {
    unsigned long now = millis();
    float dt = (now - lastMs) / 1000.0f;
    lastMs = now;

    odom.update(encL.getTicks(), encR.getTicks());
    lidar.update();
    ros.spinOnce();

    if (ros.hasNew) {
        ros.hasNew = false;
        // Convert Twist to wheel speeds
        float vL = (ros.cmdVx - ros.cmdWz * WHEEL_BASE_M / 2.0f);
        float vR = (ros.cmdVx + ros.cmdWz * WHEEL_BASE_M / 2.0f);
        // Scale m/s to PWM (max ~0.3 m/s)
        motorL.setSpeed((int)(vL / 0.3f * 255));
        motorR.setSpeed((int)(vR / 0.3f * 255));
    }

    const Pose& p = odom.getPose();
    ros.publishOdom(p.x, p.y, p.theta, ros.cmdVx, ros.cmdWz);

    // Publish scan every 100ms
    static unsigned long lastScan = 0;
    if (now - lastScan > 100) {
        lastScan = now;
        float ranges[360];
        for (int i = 0; i < 360; i++) {
            const LidarSample& s = lidar.getSamples()[i];
            ranges[i] = (s.valid && s.dist_mm > 0) ? s.dist_mm / 1000.0f : INFINITY;
        }
        ros.publishScan(ranges, 360, -M_PI, M_PI);
    }
}
