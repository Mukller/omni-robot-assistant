#pragma once

class Safety {
public:
    // Call every loop iteration
    void update(float minFrontDist_m, float batteryV, float currentL_A, float currentR_A);
    bool isEStop() const { return _estop; }
    bool isBattLow() const { return _battLow; }
    bool isOvercurrent() const { return _overcurrent; }
    void clearEStop() { _estop = false; }
private:
    bool _estop       = false;
    bool _battLow     = false;
    bool _overcurrent = false;
    static constexpr float FRONT_STOP_M  = 0.15f;
    static constexpr float BATT_LOW_V    = 10.5f;
    static constexpr float OVERCURRENT_A = 3.0f;
};
