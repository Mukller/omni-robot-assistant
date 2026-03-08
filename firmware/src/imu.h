#pragma once
#include <Adafruit_BNO055.h>

class IMU {
public:
    bool begin();
    float getHeading();   // degrees 0-360
    float getPitch();
    float getRoll();
    bool isCalibrated();
private:
    Adafruit_BNO055 _bno{55, 0x28};
};
