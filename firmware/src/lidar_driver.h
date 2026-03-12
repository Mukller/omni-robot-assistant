#pragma once
#include <Arduino.h>

struct LidarSample {
    uint16_t dist_mm;
    uint8_t  quality;
    bool     valid;
};

class LidarDriver {
public:
    LidarDriver(HardwareSerial& serial, int rxPin, int txPin);
    void begin(uint32_t baud = 115200);
    bool update();
    const LidarSample* getSamples() const { return _samples; }
    uint16_t getSampleCount() const { return _count; }
    float getMinDist(int sectorDeg, int halfWidthDeg) const;
private:
    HardwareSerial& _serial;
    int _rxPin, _txPin;
    LidarSample _samples[360];
    uint16_t _count = 0;
    uint8_t _buf[10];
    int _bufIdx = 0;
    bool parseByte(uint8_t b);
};
