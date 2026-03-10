#pragma once
#include <Adafruit_INA219.h>

class CurrentSensor {
public:
    CurrentSensor(uint8_t addr);
    bool begin();
    float getCurrent_mA();
    float getVoltage_V();
    float getPower_mW();
private:
    Adafruit_INA219 _ina;
};
