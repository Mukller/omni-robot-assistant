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
    float   getHeading();   // degrees 0–360
    float   getPitch();
    float   getRoll();
    bool    isCalibrated();
private:
    Adafruit_BNO055 _bno{55, 0x28};
};
