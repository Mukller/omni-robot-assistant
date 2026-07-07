#pragma once
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

struct IMUData {
    float ax = 0, ay = 0, az = 0;   // linear acceleration m/s²
    float gx = 0, gy = 0, gz = 0;   // angular velocity rad/s
    float qw = 1, qx = 0, qy = 0, qz = 0;  // orientation quaternion
};

class IMU {
public:
    bool    begin();
    IMUData read();
    // One I2C read for all three Euler angles — prefer getEuler() over calling
    // getHeading/getPitch/getRoll individually to avoid 3 separate I2C transactions.
    imu::Vector<3> getEuler();
    float   getHeading();   // degrees 0–360 (calls getEuler internally)
    float   getPitch();
    float   getRoll();
    bool    isCalibrated();
private:
    Adafruit_BNO055 _bno{55, 0x28};
};
