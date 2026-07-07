#include "imu.h"
#include <math.h>

bool IMU::begin() {
    if (!_bno.begin()) return false;
    _bno.setExtCrystalUse(true);
    // NDOF fusion mode: gyro + accel + mag
    _bno.setMode(OPERATION_MODE_NDOF);
    return true;
}

IMUData IMU::read() {
    IMUData d;

    // Linear acceleration (gravity-compensated)
    imu::Vector<3> accel = _bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    d.ax = accel.x();
    d.ay = accel.y();
    d.az = accel.z();

    // Angular velocity (rad/s)
    imu::Vector<3> gyro = _bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    d.gx = gyro.x();
    d.gy = gyro.y();
    d.gz = gyro.z();

    // Orientation quaternion
    imu::Quaternion q = _bno.getQuat();
    d.qw = (float)q.w();
    d.qx = (float)q.x();
    d.qy = (float)q.y();
    d.qz = (float)q.z();

    return d;
}

imu::Vector<3> IMU::getEuler() {
    return _bno.getVector(Adafruit_BNO055::VECTOR_EULER);
}

float IMU::getHeading() { return getEuler().x(); }  // 0–360 degrees
float IMU::getPitch()   { return getEuler().y(); }
float IMU::getRoll()    { return getEuler().z(); }

bool IMU::isCalibrated() {
    uint8_t sys, gyr, acc, mag;
    _bno.getCalibration(&sys, &gyr, &acc, &mag);
    return sys == 3;
}
