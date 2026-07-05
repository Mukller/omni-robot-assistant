#include "safety.h"
#include <Arduino.h>

void SafetyMonitor::update(float battVolt, float currL_A, float currR_A,
                            unsigned long lastCmdMs) {
    _battLow = (battVolt < BATT_LOW_V);

    bool overcurrent = (currL_A > OVERCURRENT_A || currR_A > OVERCURRENT_A);
    bool cmdTimeout  = (millis() - lastCmdMs > CMD_TIMEOUT_MS);

    if (overcurrent || cmdTimeout || _battLow) {
        _estop = true;
        _mL.stop(BRAKE_STOP);
        _mR.stop(BRAKE_STOP);
        _pL.reset();
        _pR.reset();
    }
}
