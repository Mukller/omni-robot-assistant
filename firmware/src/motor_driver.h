#pragma once
#include <Arduino.h>

enum BrakeMode { BRAKE_COAST, BRAKE_STOP };

class MotorDriver {
public:
    MotorDriver(uint8_t in1, uint8_t in2, uint8_t en);
    void begin();
    void setSpeed(int speed);  // -255..255
    void stop(BrakeMode mode = BRAKE_STOP);
    bool isRunning() const;
private:
    uint8_t _in1, _in2, _en;
    int _speed = 0;
};
