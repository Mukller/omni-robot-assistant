#include <unity.h>
#include "../src/pid.h"

PID pid(1.0f, 0.0f, 0.0f, -100, 100);

void test_proportional() {
    pid.reset();
    float out = pid.compute(10.0f, 5.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, out);
}

void test_output_clamped() {
    pid.reset();
    float out = pid.compute(200.0f, 0.0f, 0.1f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, out);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_proportional);
    RUN_TEST(test_output_clamped);
    return UNITY_END();
}
