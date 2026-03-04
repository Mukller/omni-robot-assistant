#pragma once
#include <Arduino.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB);
    void begin();
    long getTicks() const;
    void reset();
    float getRPM(unsigned long dtMs);
private:
    uint8_t _pinA, _pinB;
    volatile long _ticks = 0;
    long _lastTicks = 0;
    static void IRAM_ATTR isrA();
    static void IRAM_ATTR isrB();
    static Encoder* _instances[2];
    static int _idx;
    int _myIdx;
};
