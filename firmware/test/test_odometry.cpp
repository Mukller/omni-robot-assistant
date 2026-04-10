#include <unity.h>
#include "../src/odometry.h"
#include <cmath>

void test_straight_line() {
    Odometry odom(0.172f, 0.0375f, 1920);
    // 1920 ticks = 1 revolution = 2*pi*0.0375 = 0.2356m
    odom.update(1920, 1920);
    const Pose& p = odom.getPose();
    TEST_ASSERT_FLOAT_WITHIN(0.005f, 0.2356f, p.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.y);
}

void test_rotate_in_place() {
    Odometry odom(0.172f, 0.0375f, 1920);
    odom.update(1920, -1920);
    const Pose& p = odom.getPose();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, p.x);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_straight_line);
    RUN_TEST(test_rotate_in_place);
    return UNITY_END();
}
