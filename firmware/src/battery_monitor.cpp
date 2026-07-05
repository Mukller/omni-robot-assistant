#include "battery_monitor.h"
#include <Arduino.h>

BatteryMonitor::BatteryMonitor(uint8_t adcPin, float dividerRatio)
    : _pin(adcPin), _ratio(dividerRatio) {}

void BatteryMonitor::begin() {
    pinMode(_pin, INPUT);
    analogSetAttenuation(ADC_11db);   // ESP32: 0–3.9V range
}

float BatteryMonitor::getVoltage() {
    // Average 8 samples to reduce ADC noise
    uint32_t raw = 0;
    for (int i = 0; i < 8; i++) raw += analogRead(_pin);
    raw /= 8;
    // ESP32 ADC: 12-bit, 3.3V reference
    float vAdc = (raw / 4095.0f) * 3.3f;
    return vAdc * _ratio;
}

int BatteryMonitor::getPercent() {
    float v = getVoltage();
    if (v >= FULL_V)  return 100;
    if (v <= EMPTY_V) return 0;
    return (int)(100.0f * (v - EMPTY_V) / (FULL_V - EMPTY_V));
}

bool BatteryMonitor::isLow(float threshold) {
    return getVoltage() < threshold;
}
