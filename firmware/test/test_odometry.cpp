#include <unity.h>
#include "../src/odometry.h"
#include <cmath>

static const float WB   = 0.172f;
static const float WR   = 0.0375f;
static const int   TPR  = 1920;
// One revolution distance
static const float CIRC = 2.0f * M_PI * WR;  // ~0.2356 m

void test_straight_line() {
    Odometry odom(WB, WR, TPR);
    odom.update(TPR, TPR);  // both wheels: 1 revolution each
    const Pose& p = odom.getPose();
    TEST_ASSERT_FLOAT_WITHIN(0.005f, CIRC, p.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.y);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.theta);
}

void test_rotate_in_place() {
    Odometry odom(WB, WR, TPR);
    odom.update(TPR, -TPR);  // left fwd, right back
    const Pose& p = odom.getPose();
    // x, y should stay near 0 (pure rotation)
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, p.x);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, p.y);
    // Expected rotation angle: 2 * CIRC / WB (full arc both sides)
    float expected_theta = -2.0f * CIRC / WB;
    TEST_ASSERT_FLOAT_WITHIN(0.05f, expected_theta, p.theta);
}

void test_incremental_accumulation() {
    Odometry odom(WB, WR, TPR);
    // Two half-steps should equal one full step
    odom.update(TPR / 2, TPR / 2);
    odom.update(TPR,     TPR);
    const Pose& p = odom.getPose();
    TEST_ASSERT_FLOAT_WITHIN(0.005f, CIRC, p.x);
}

void test_reset() {
    Odometry odom(WB, WR, TPR);
    odom.update(TPR, TPR);
    odom.reset();
    const Pose& p = odom.getPose();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.y);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, p.theta);
}

void test_curved_path() {
    Odometry odom(WB, WR, TPR);
    // Right wheel goes twice as far → curved path
    odom.update(TPR, TPR * 2);
    const Pose& p = odom.getPose();
    // x should be positive, y should deviate
    TEST_ASSERT_TRUE(p.x > 0.0f);
    TEST_ASSERT_TRUE(p.theta > 0.0f);  // turned left
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_straight_line);
    RUN_TEST(test_rotate_in_place);
    RUN_TEST(test_incremental_accumulation);
    RUN_TEST(test_reset);
    RUN_TEST(test_curved_path);
    return UNITY_END();
}
