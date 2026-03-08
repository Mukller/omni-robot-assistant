#pragma once

struct Pose {
    float x = 0, y = 0, theta = 0;
};

class Odometry {
public:
    Odometry(float wheelBase, float wheelRadius, int ticksPerRev);
    void update(long leftTicks, long rightTicks);
    const Pose& getPose() const { return _pose; }
    void reset();
private:
    float _wb, _wr;
    float _ticksToM;
    long _prevL = 0, _prevR = 0;
    Pose _pose;
};
