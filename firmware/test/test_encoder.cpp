// Host-side unit tests for encoder tick math and velocity conversion.
// The ISR/GPIO layer is stubbed — we test the computation only.
#include <unity.h>
#include <cstdint>

// ── Stub types (replaces Arduino.h on native build) ──────────────────────────
#ifndef NATIVE_BUILD
#define NATIVE_BUILD
#endif
typedef uint8_t uint8_t;

// ── Inline reimplementation of tick→velocity math (mirrors encoder.cpp) ──────
static float ticksToMs(long deltaTicks, unsigned long dtMs,
                       float wheelRadiusM, int ticksPerRev) {
    const float msPerTick = (2.0f * 3.14159265f * wheelRadiusM) / ticksPerRev;
    return (deltaTicks * 1000.0f / dtMs) * msPerTick;
}

static const float WR   = 0.0375f;
static const int   TPR  = 1920;
static const float CIRC_PER_TICK = (2.0f * 3.14159265f * WR) / TPR;  // ~0.0001228 m

// ── Tests ─────────────────────────────────────────────────────────────────────
void test_zero_delta_gives_zero_velocity() {
    float v = ticksToMs(0, 100, WR, TPR);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, v);
}

void test_one_rev_per_second_velocity() {
    // 1920 ticks in 1000 ms → one full revolution per second
    // v = CIRC / 1s = 2π × 0.0375 ≈ 0.2356 m/s
    float v = ticksToMs(TPR, 1000, WR, TPR);
    float expected = 2.0f * 3.14159265f * WR;
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected, v);
}

void test_half_rev_per_second() {
    float v = ticksToMs(TPR / 2, 1000, WR, TPR);
    float expected = 3.14159265f * WR;  // half circumference per second
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected, v);
}

void test_negative_ticks_give_negative_velocity() {
    float v = ticksToMs(-TPR, 1000, WR, TPR);
    float expected = -(2.0f * 3.14159265f * WR);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected, v);
}

void test_short_dt_scales_velocity_up() {
    // Same delta ticks but in half the time → double the velocity
    float v100 = ticksToMs(100, 1000, WR, TPR);
    float v50  = ticksToMs(100,  500, WR, TPR);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, v100 * 2.0f, v50);
}

void test_single_tick_resolution() {
    // Minimum non-zero measurement: 1 tick in 50ms (20 Hz control loop)
    float v = ticksToMs(1, 50, WR, TPR);
    TEST_ASSERT_TRUE(v > 0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, CIRC_PER_TICK * 20.0f, v);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_zero_delta_gives_zero_velocity);
    RUN_TEST(test_one_rev_per_second_velocity);
    RUN_TEST(test_half_rev_per_second);
    RUN_TEST(test_negative_ticks_give_negative_velocity);
    RUN_TEST(test_short_dt_scales_velocity_up);
    RUN_TEST(test_single_tick_resolution);
    return UNITY_END();
}
