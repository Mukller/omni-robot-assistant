#include <unity.h>
#include "../src/pid.h"
#include <cmath>

void test_proportional() {
    PID pid(1.0f, 0.0f, 0.0f, -100, 100);
    float out = pid.compute(10.0f, 5.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, out);  // Kp*err = 1.0 * 5.0
}

void test_output_clamped_positive() {
    PID pid(1.0f, 0.0f, 0.0f, -100, 100);
    float out = pid.compute(200.0f, 0.0f, 0.1f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, out);
}

void test_output_clamped_negative() {
    PID pid(1.0f, 0.0f, 0.0f, -100, 100);
    float out = pid.compute(-200.0f, 0.0f, 0.1f);
    TEST_ASSERT_EQUAL_FLOAT(-100.0f, out);
}

void test_integral_accumulates() {
    PID pid(0.0f, 1.0f, 0.0f, -1000, 1000);  // pure I
    // 10 steps of error=1, dt=0.1 → integral = 1.0 → output = 1.0
    float out = 0.0f;
    for (int i = 0; i < 10; i++)
        out = pid.compute(1.0f, 0.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, out);
}

void test_integral_anti_windup() {
    PID pid(0.0f, 1.0f, 0.0f, -10, 10);
    // Feed huge error — integral should clamp, output shouldn't grow unbounded
    for (int i = 0; i < 100; i++)
        pid.compute(100.0f, 0.0f, 0.1f);
    float out = pid.compute(100.0f, 0.0f, 0.1f);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, out);
}

void test_derivative_brakes() {
    PID pid(0.0f, 0.0f, 1.0f, -1000, 1000);  // pure D
    pid.compute(10.0f, 0.0f, 0.1f);  // first call — sets prevErr
    float out = pid.compute(5.0f,  0.0f, 0.1f);  // err went from 10 to 5 → Δerr=-5 → deriv=-50
    TEST_ASSERT_FLOAT_WITHIN(1.0f, -50.0f, out);
}

void test_zero_setpoint_holds_zero() {
    PID pid(2.5f, 0.8f, 0.05f, -255, 255);
    for (int i = 0; i < 20; i++)
        pid.compute(0.0f, 0.0f, 0.02f);
    float out = pid.compute(0.0f, 0.0f, 0.02f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, out);
}

void test_reset_clears_state() {
    PID pid(0.0f, 1.0f, 1.0f, -1000, 1000);
    for (int i = 0; i < 10; i++)
        pid.compute(5.0f, 0.0f, 0.1f);
    pid.reset();
    float out = pid.compute(1.0f, 0.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 0.1f, out);  // should behave like first call
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_proportional);
    RUN_TEST(test_output_clamped_positive);
    RUN_TEST(test_output_clamped_negative);
    RUN_TEST(test_integral_accumulates);
    RUN_TEST(test_integral_anti_windup);
    RUN_TEST(test_derivative_brakes);
    RUN_TEST(test_zero_setpoint_holds_zero);
    RUN_TEST(test_reset_clears_state);
    return UNITY_END();
}
