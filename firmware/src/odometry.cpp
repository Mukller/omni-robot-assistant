#include "odometry.h"
#include <cmath>

Odometry::Odometry(float wheelBase, float wheelRadius, int ticksPerRev)
    : _wb(wheelBase), _wr(wheelRadius) {
    _ticksToM = (2.0f * M_PI * wheelRadius) / ticksPerRev;
}

void Odometry::update(long leftTicks, long rightTicks) {
    float dL = (leftTicks  - _prevL) * _ticksToM;
    float dR = (rightTicks - _prevR) * _ticksToM;
    _prevL = leftTicks; _prevR = rightTicks;

    float dDist  = (dR + dL) / 2.0f;
    float dTheta = (dR - dL) / _wb;

    _pose.theta += dTheta;
    _pose.x     += dDist * cosf(_pose.theta);
    _pose.y     += dDist * sinf(_pose.theta);
}

void Odometry::reset() { _pose = {}; _prevL = _prevR = 0; }
