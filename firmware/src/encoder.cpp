#include "encoder.h"
#include <Arduino.h>

// Static member definitions
Encoder* Encoder::_instances[2] = {nullptr, nullptr};
int      Encoder::_idx          = 0;

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
    : _pinA(pinA), _pinB(pinB) {
    _myIdx = _idx++;
    _instances[_myIdx] = this;
}

void Encoder::begin() {
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);
    // Assign ISR based on instance index
    if (_myIdx == 0) {
        attachInterrupt(digitalPinToInterrupt(_pinA), isrA, CHANGE);
    } else {
        attachInterrupt(digitalPinToInterrupt(_pinA), isrB, CHANGE);
    }
}

long Encoder::getTicks() const {
    return _ticks;
}

void Encoder::reset() {
    _ticks = 0;
    _lastTicks = 0;
}

float Encoder::getTicksPerSec(unsigned long dtMs) {
    if (dtMs == 0) return 0.0f;
    long current = _ticks;
    long delta   = current - _lastTicks;
    _lastTicks   = current;
    // ticks/ms → revolutions/min: delta / TICKS_PER_REV * 1000 / dtMs * 60
    // We don't have TICKS_PER_REV here — caller must scale. Return ticks/s instead.
    return (delta * 1000.0f) / dtMs;  // ticks per second
}

void IRAM_ATTR Encoder::isrA() {
    Encoder* e = _instances[0];
    if (!e) return;
    bool a = digitalRead(e->_pinA);
    bool b = digitalRead(e->_pinB);
    e->_ticks += (a == b) ? 1 : -1;
}

void IRAM_ATTR Encoder::isrB() {
    Encoder* e = _instances[1];
    if (!e) return;
    bool a = digitalRead(e->_pinA);
    bool b = digitalRead(e->_pinB);
    e->_ticks += (a == b) ? 1 : -1;
}
