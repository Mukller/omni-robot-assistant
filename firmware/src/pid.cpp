#include "pid.h"
#include <algorithm>

PID::PID(float kp, float ki, float kd, float outMin, float outMax)
    : _kp(kp), _ki(ki), _kd(kd), _outMin(outMin), _outMax(outMax) {}

float PID::compute(float setpoint, float measured, float dt) {
    float err = setpoint - measured;
    _integral += err * dt;
    // Anti-windup clamp
    _integral = std::max(_outMin / _ki, std::min(_outMax / _ki, _integral));
    float deriv = (err - _prevErr) / dt;
    _prevErr = err;
    float out = _kp * err + _ki * _integral + _kd * deriv;
    return std::max(_outMin, std::min(_outMax, out));
}

void PID::reset() { _integral = 0; _prevErr = 0; }
void PID::setGains(float kp, float ki, float kd) { _kp = kp; _ki = ki; _kd = kd; }
