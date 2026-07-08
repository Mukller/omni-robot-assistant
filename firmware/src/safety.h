#pragma once
#include "motor_driver.h"
#include "pid.h"

class SafetyMonitor {
public:
    SafetyMonitor(MotorDriver& mL, MotorDriver& mR, PID& pL, PID& pR)
        : _mL(mL), _mR(mR), _pL(pL), _pR(pR) {}

    // Call every loop. lastCmdMs = millis() of last received /cmd_vel.
    void update(float battVolt, float currL_A, float currR_A, unsigned long lastCmdMs);

    bool isEStop()   const { return _estop; }
    bool isBattLow() const { return _battLow; }
    void clearEStop()      { _latchedFault = false; _estop = false; }

private:
    MotorDriver& _mL;
    MotorDriver& _mR;
    PID&         _pL;
    PID&         _pR;

    bool _estop        = false;
    bool _battLow      = false;
    bool _latchedFault = false;  // true when battery/overcurrent latch is active

    static constexpr float         BATT_LOW_V      = 10.5f;
    static constexpr float         OVERCURRENT_A   = 2.5f;
    static constexpr unsigned long CMD_TIMEOUT_MS  = 500;   // e-stop if no cmd_vel
};
