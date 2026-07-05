#include "current_sensor.h"

CurrentSensor::CurrentSensor(uint8_t addr) : _ina(addr) {}

bool CurrentSensor::begin() {
    return _ina.begin();
}

float CurrentSensor::getCurrent_mA() {
    return _ina.getCurrent_mA();
}

float CurrentSensor::getVoltage_V() {
    return _ina.getBusVoltage_V();
}

float CurrentSensor::getPower_mW() {
    return _ina.getPower_mW();
}
