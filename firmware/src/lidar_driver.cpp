#include "lidar_driver.h"

LidarDriver::LidarDriver(HardwareSerial& serial, int rxPin, int txPin)
    : _serial(serial), _rxPin(rxPin), _txPin(txPin) {}

void LidarDriver::begin(uint32_t baud) {
    _serial.begin(baud, SERIAL_8N1, _rxPin, _txPin);
    memset(_samples, 0, sizeof(_samples));
}

bool LidarDriver::update() {
    bool newScan = false;
    while (_serial.available()) {
        uint8_t b = _serial.read();
        if (parseByte(b)) newScan = true;
    }
    return newScan;
}

bool LidarDriver::parseByte(uint8_t b) {
    // Minimal packet accumulator — subclass overrides for device-specific framing
    _buf[_bufIdx++] = b;
    if (_bufIdx >= 9) {
        _bufIdx = 0;
        return true;
    }
    return false;
}

float LidarDriver::getMinDist(int sectorDeg, int halfWidthDeg) const {
    float minD = 99999;
    for (int a = sectorDeg - halfWidthDeg; a <= sectorDeg + halfWidthDeg; ++a) {
        int idx = ((a % 360) + 360) % 360;
        if (_samples[idx].valid && _samples[idx].dist_mm > 0)
            minD = min(minD, (float)_samples[idx].dist_mm);
    }
    return minD;
}
