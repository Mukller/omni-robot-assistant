#include "motor_driver.h"

MotorDriver::MotorDriver(uint8_t in1, uint8_t in2, uint8_t en)
    : _in1(in1), _in2(in2), _en(en) {}

void MotorDriver::begin() {
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_en, OUTPUT);
    stop();
}

void MotorDriver::setSpeed(int speed) {
    _speed = constrain(speed, -255, 255);
    if (_speed > 0) {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
    } else if (_speed < 0) {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
    } else {
        stop();
        return;
    }
    analogWrite(_en, abs(_speed));
}

void MotorDriver::stop(BrakeMode mode) {
    _speed = 0;
    if (mode == BRAKE_STOP) {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, HIGH);
    } else {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, LOW);
    }
    analogWrite(_en, 0);
}

bool MotorDriver::isRunning() const { return _speed != 0; }
