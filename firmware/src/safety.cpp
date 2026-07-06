#include "safety.h"
#include <Arduino.h>

void SafetyMonitor::update(float battVolt, float currL_A, float currR_A,
                            unsigned long lastCmdMs) {
    _battLow = (battVolt > 0.5f) && (battVolt < BATT_LOW_V);  // ignore 0V (ADC not ready)

    bool overcurrent = (currL_A > OVERCURRENT_A || currR_A > OVERCURRENT_A);

    // cmd_vel timeout: only enforce if we've received at least one command
    // (lastCmdMs == 0 means no command yet — grace period at startup)
    bool cmdTimeout = (lastCmdMs > 0) && (millis() - lastCmdMs > CMD_TIMEOUT_MS);

    // Latching faults (battery + overcurrent) require clearEStop() to recover.
    // cmd_vel timeout auto-clears as soon as commands resume.
    if (_battLow || overcurrent) {
        _estop = true;
    } else if (cmdTimeout) {
        _estop = true;
    } else {
        // No active fault — clear cmd_vel timeout e-stop (non-latching)
        if (!_latchedFault) _estop = false;
    }

    _latchedFault = _battLow || overcurrent;

    if (_estop) {
        _mL.stop(BRAKE_STOP);
        _mR.stop(BRAKE_STOP);
        _pL.reset();
        _pR.reset();
    }
}
