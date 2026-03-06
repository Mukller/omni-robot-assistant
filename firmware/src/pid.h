#pragma once

class PID {
public:
    PID(float kp, float ki, float kd, float outMin, float outMax);
    float compute(float setpoint, float measured, float dt);
    void reset();
    void setGains(float kp, float ki, float kd);
private:
    float _kp, _ki, _kd;
    float _outMin, _outMax;
    float _integral = 0;
    float _prevErr  = 0;
};
