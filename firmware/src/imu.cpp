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

float IMU::getHeading() {
    imu::Vector<3> euler = _bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    return euler.x();  // 0–360 degrees
}

float IMU::getPitch() {
    imu::Vector<3> euler = _bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    return euler.y();
}

float IMU::getRoll() {
    imu::Vector<3> euler = _bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    return euler.z();
}

bool IMU::isCalibrated() {
    uint8_t sys, gyr, acc, mag;
    _bno.getCalibration(&sys, &gyr, &acc, &mag);
    return sys == 3;
}
