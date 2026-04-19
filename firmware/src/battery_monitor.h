#pragma once
#include <Arduino.h>

class BatteryMonitor {
public:
    BatteryMonitor(uint8_t adcPin, float dividerRatio = 4.0f);
    void begin();
    float getVoltage();
    int getPercent();
    bool isLow(float threshold = 10.5f);
private:
    uint8_t _pin;
    float   _ratio;
    // LiPo 3S: 12.6V full, 10.5V cutoff
    static constexpr float FULL_V = 12.6f;
    static constexpr float EMPTY_V = 10.5f;
};
