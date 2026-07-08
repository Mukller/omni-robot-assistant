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
    // Only set — never auto-clear; clearEStop() is the only path out.
    if (_battLow || overcurrent) _latchedFault = true;

    // cmd_vel timeout is non-latching: auto-clears as soon as commands resume.
    _estop = _latchedFault || cmdTimeout;

    if (_estop) {
        _mL.stop(BRAKE_STOP);
        _mR.stop(BRAKE_STOP);
        _pL.reset();
        _pR.reset();
    }
}
