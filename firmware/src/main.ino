#include <Arduino.h>
#include "../config.h"
#include "wifi_config.h"
#include "motor_driver.h"
#include "encoder.h"
#include "odometry.h"
#include "pid.h"
#include "imu.h"
#include "lidar_driver.h"
#include "current_sensor.h"
#include "battery_monitor.h"
#include "safety.h"
#include "microros_bridge.h"
#include "ota_update.h"

MotorDriver   motorL(MOT_L_IN1, MOT_L_IN2, MOT_L_EN);
MotorDriver   motorR(MOT_R_IN1, MOT_R_IN2, MOT_R_EN);
Encoder       encL(ENC_L_A, ENC_L_B);
Encoder       encR(ENC_R_A, ENC_R_B);
Odometry      odom(WHEEL_BASE_M, WHEEL_RADIUS_M, TICKS_PER_REV);
PID           pidL(2.5f, 0.8f, 0.05f, -255, 255);
PID           pidR(2.5f, 0.8f, 0.05f, -255, 255);
IMU           imu;
LidarDriver   lidar(Serial2, LIDAR_RX, LIDAR_TX);
CurrentSensor csL(INA_ADDR_L), csR(INA_ADDR_R);
BatteryMonitor batt(BATT_PIN);
SafetyMonitor  safety(motorL, motorR, pidL, pidR);
MicroROSBridge ros;

// Max wheel speed: JGA25-370 12V 40:1 ≈ 100 RPM no-load → 2π×0.0375×100/60 ≈ 0.39 m/s
static constexpr float MAX_VEL_MS = 0.39f;

unsigned long lastMs = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("[Loki] Booting...");

    motorL.begin(); motorR.begin();
    encL.begin();   encR.begin();
    odom.reset();

    if (!imu.begin())    Serial.println("[WARN] BNO055 not found");
    if (!lidar.begin())  Serial.println("[WARN] LiDAR init failed");
    csL.begin(); csR.begin();
    batt.begin();

    // Connect WiFi + microROS agent using credentials from wifi_config.h
    Serial.printf("[WiFi] Connecting to %s\n", WIFI_SSID);
    ros.begin(AGENT_IP, AGENT_PORT);
    Serial.println("[microROS] Agent connected");

    setupOTA(OTA_PASSWORD);  // OTA updates: pio run -t upload --upload-port <ip>
}

void loop() {
    unsigned long now   = millis();
    unsigned long dtMs  = now - lastMs;
    if (dtMs == 0) return;           // avoid division by zero on fast loops
    float dt = dtMs / 1000.0f;
    lastMs = now;

    // ── Odometry ────────────────────────────────────────────────────────
    odom.update(encL.getTicks(), encR.getTicks());
    const Pose& p = odom.getPose();

    // ── Safety watchdog ─────────────────────────────────────────────────
    float battVolt = batt.getVoltage();
    float currL    = csL.getCurrent();
    float currR    = csR.getCurrent();
    safety.update(battVolt, currL, currR, ros.lastCmdMs());
    if (safety.isEStop()) {
        motorL.stop(BRAKE_STOP);
        motorR.stop(BRAKE_STOP);
        pidL.reset(); pidR.reset();
        ros.spinOnce();
        return;
    }

    // ── Velocity PID ─────────────────────────────────────────────────────
    // Measured wheel velocities in m/s (ticks/s → m/s via circumference)
    const float ticksToMs = (2.0f * M_PI * WHEEL_RADIUS_M) / TICKS_PER_REV;
    float velL_ms = encL.getTicksPerSec(dtMs) * ticksToMs;
    float velR_ms = encR.getTicksPerSec(dtMs) * ticksToMs;

    // Differential kinematics: Twist → wheel target velocities
    float targetL = ros.cmdVx - ros.cmdWz * (WHEEL_BASE_M / 2.0f);
    float targetR = ros.cmdVx + ros.cmdWz * (WHEEL_BASE_M / 2.0f);

    // Clamp to hardware max
    targetL = constrain(targetL, -MAX_VEL_MS, MAX_VEL_MS);
    targetR = constrain(targetR, -MAX_VEL_MS, MAX_VEL_MS);

    // PID: setpoint = target m/s, measured = actual m/s → PWM output
    int pwmL = (int)pidL.compute(targetL, velL_ms, dt);
    int pwmR = (int)pidR.compute(targetR, velR_ms, dt);
    motorL.setSpeed(pwmL);
    motorR.setSpeed(pwmR);

    // ── OTA + microROS spin ──────────────────────────────────────────────
    ArduinoOTA.handle();
    ros.spinOnce();

    // Publish odometry at full rate
    ros.publishOdom(p.x, p.y, p.theta, ros.cmdVx, ros.cmdWz);

    // Publish IMU at 50 Hz
    static unsigned long lastImu = 0;
    if (now - lastImu >= 20) {
        lastImu = now;
        IMUData d = imu.read();
        ros.publishImu(d.ax, d.ay, d.az, d.gx, d.gy, d.gz,
                       d.qw, d.qx, d.qy, d.qz);
    }

    // Publish battery state at 1 Hz
    static unsigned long lastBatt = 0;
    if (now - lastBatt >= 1000) {
        lastBatt = now;
        ros.publishBattery(battVolt, batt.getPercent(), currL + currR);
    }

    // Publish LaserScan at ~10 Hz
    static unsigned long lastScan = 0;
    if (now - lastScan >= 100) {
        lastScan = now;
        lidar.update();
        float ranges[360];
        for (int i = 0; i < 360; i++) {
            const LidarSample& s = lidar.getSamples()[i];
            ranges[i] = (s.valid && s.dist_mm >= 120) ? s.dist_mm / 1000.0f : INFINITY;
        }
        ros.publishScan(ranges, 360, -M_PI, M_PI);
    }
}
