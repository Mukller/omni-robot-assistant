#include "safety.h"

void Safety::update(float minFrontDist_m, float batteryV, float currentL_A, float currentR_A) {
    if (minFrontDist_m > 0 && minFrontDist_m < FRONT_STOP_M)
        _estop = true;
    _battLow     = (batteryV < BATT_LOW_V);
    _overcurrent = (currentL_A > OVERCURRENT_A || currentR_A > OVERCURRENT_A);
}
