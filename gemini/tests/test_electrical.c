/**
 * ============================================================================
 * AC Circuit Analyzer - Electrical Calculations Unit Tests
 * ============================================================================
 */

#include <stdio.h>
#include "electrical.h"
#include "math_engine.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tests_run++; \
    if (cond) { \
        g_tests_passed++; \
        printf("  [PASS] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
    } \
} while(0)

#define TEST_ASSERT_APPROX(val, expected, tol, msg) do { \
    g_tests_run++; \
    double _v = (val); \
    double _exp = (expected); \
    if (math_approx_equal(_v, _exp, tol)) { \
        g_tests_passed++; \
        printf("  [PASS] %s (Got: %.6f, Exp: %.6f)\n", msg, _v, _exp); \
    } else { \
        printf("  [FAIL] %s (Got: %.6f, Exp: %.6f, Tol: %.6f, Line %d)\n", msg, _v, _exp, tol, __LINE__); \
    } \
} while(0)

void test_ohms_law(void) {
    printf("\n=== Running Ohm's Law Tests ===\n");
    double v = 0.0, i = 0.0, r = 0.0;

    /* 12 V / 6 Ω = 2 A */
    TEST_ASSERT(ohm_calc_current(12.0, 6.0, &i) == ELEC_OK, "Calc current: 12V / 6 Ohm");
    TEST_ASSERT_APPROX(i, 2.0, 1e-9, "Current == 2.0 A");

    /* 100 Ω connected to 10 V produces 0.1 A */
    TEST_ASSERT(ohm_calc_current(10.0, 100.0, &i) == ELEC_OK, "Calc current: 10V / 100 Ohm");
    TEST_ASSERT_APPROX(i, 0.1, 1e-9, "Current == 0.1 A");

    /* 5 A through 10 Ω produces 50 V */
    TEST_ASSERT(ohm_calc_voltage(5.0, 10.0, &v) == ELEC_OK, "Calc voltage: 5A * 10 Ohm");
    TEST_ASSERT_APPROX(v, 50.0, 1e-9, "Voltage == 50.0 V");

    /* 120 V / 15 A = 8 Ω */
    TEST_ASSERT(ohm_calc_resistance(120.0, 15.0, &r) == ELEC_OK, "Calc resistance: 120V / 15A");
    TEST_ASSERT_APPROX(r, 8.0, 1e-9, "Resistance == 8.0 Ohm");

    /* Error handling */
    TEST_ASSERT(ohm_calc_current(10.0, 0.0, &i) == ELEC_ERR_DIV_BY_ZERO, "Division by zero resistance detected");
    TEST_ASSERT(ohm_calc_resistance(10.0, 0.0, &r) == ELEC_ERR_DIV_BY_ZERO, "Division by zero current detected");
    TEST_ASSERT(ohm_calc_current(10.0, -5.0, &i) == ELEC_ERR_NEGATIVE_VAL, "Negative resistance rejected");
}

void test_electrical_power(void) {
    printf("\n=== Running Electrical Power Tests ===\n");
    power_result_t p_res;

    /* Purely resistive: 220 V RMS, 10 A RMS, phase 0 */
    TEST_ASSERT(power_calc_from_vi_theta(220.0, 10.0, 0.0, &p_res) == ELEC_OK, "Calc power: 220V, 10A, theta=0");
    TEST_ASSERT_APPROX(p_res.apparent_power, 2200.0, 1e-6, "Apparent power S == 2200 VA");
    TEST_ASSERT_APPROX(p_res.active_power, 2200.0, 1e-6, "Active power P == 2200 W");
    TEST_ASSERT_APPROX(p_res.reactive_power, 0.0, 1e-6, "Reactive power Q == 0 VAR");
    TEST_ASSERT_APPROX(p_res.power_factor, 1.0, 1e-6, "Power factor PF == 1.0");

    /* Inductive load: 220 V, 10 A, theta = 60 deg (pi/3 rad) */
    TEST_ASSERT(power_calc_from_vi_theta(220.0, 10.0, MATH_PI / 3.0, &p_res) == ELEC_OK, "Calc power: 220V, 10A, theta=60 deg");
    TEST_ASSERT_APPROX(p_res.power_factor, 0.5, 1e-6, "PF == cos(60 deg) == 0.5");
    TEST_ASSERT_APPROX(p_res.active_power, 1100.0, 1e-6, "Active power P == 1100 W");
    TEST_ASSERT(p_res.is_lagging == true, "Current is lagging (inductive)");

    /* Power triangle from P = 300 W, Q = 400 VAR -> S = 500 VA, PF = 0.6 */
    TEST_ASSERT(power_calc_from_pq(300.0, 400.0, &p_res) == ELEC_OK, "Power triangle P=300, Q=400");
    TEST_ASSERT_APPROX(p_res.apparent_power, 500.0, 1e-6, "S == sqrt(300^2 + 400^2) == 500 VA");
    TEST_ASSERT_APPROX(p_res.power_factor, 0.6, 1e-6, "PF == 300 / 500 == 0.6");
}

void test_reactance(void) {
    printf("\n=== Running Reactance Tests ===\n");
    double xl = 0.0, xc = 0.0;

    /* Inductive reactance: f = 60 Hz, L = 0.1 H -> XL = 2*pi*60*0.1 = 37.69911 Ohm */
    TEST_ASSERT(reactance_calc_inductive(60.0, 0.1, &xl) == ELEC_OK, "Calc XL: 60Hz, 0.1H");
    TEST_ASSERT_APPROX(xl, 37.6991118, 1e-5, "XL approx 37.6991 Ohm");

    /* Capacitive reactance: f = 60 Hz, C = 100 uF (1e-4 F) -> XC = 1 / (2*pi*60*1e-4) = 26.5258 Ohm */
    TEST_ASSERT(reactance_calc_capacitive(60.0, 1e-4, &xc) == ELEC_OK, "Calc XC: 60Hz, 100uF");
    TEST_ASSERT_APPROX(xc, 26.5258238, 1e-5, "XC approx 26.5258 Ohm");

    /* Error handling */
    TEST_ASSERT(reactance_calc_capacitive(0.0, 1e-4, &xc) == ELEC_ERR_DIV_BY_ZERO, "DC frequency on capacitor returns div by zero");
}

void test_series_rlc(void) {
    printf("\n=== Running Series RLC Circuit Tests ===\n");
    rlc_result_t rlc;
    /* R = 10 Ohm, L = 0.1 H, C = 100 uF, f = 60 Hz, V = 120 V */
    TEST_ASSERT(rlc_analyze(10.0, 0.1, 1e-4, 60.0, 120.0, &rlc) == ELEC_OK, "Analyze RLC circuit");
    TEST_ASSERT_APPROX(rlc.xl, 37.69911, 1e-4, "RLC XL approx 37.6991 Ohm");
    TEST_ASSERT_APPROX(rlc.xc, 26.52582, 1e-4, "RLC XC approx 26.5258 Ohm");
    TEST_ASSERT_APPROX(rlc.net_reactance, 11.17329, 1e-4, "Net reactance X = XL - XC approx 11.1733 Ohm");
    TEST_ASSERT(rlc.nature == CIRCUIT_INDUCTIVE, "Circuit nature is INDUCTIVE");
    TEST_ASSERT_APPROX(rlc.z_magnitude, 14.9947, 1e-3, "Impedance magnitude |Z| approx 14.995 Ohm");
    TEST_ASSERT_APPROX(rlc.i_rms, 8.0028, 1e-3, "RMS Current I approx 8.003 A");
    TEST_ASSERT_APPROX(rlc.resonant_freq, 50.3292, 1e-3, "Resonant frequency f0 approx 50.329 Hz");

    /* Test circuit at resonance: set f = f0 = 50.329212 Hz */
    TEST_ASSERT(rlc_analyze(10.0, 0.1, 1e-4, rlc.resonant_freq, 120.0, &rlc) == ELEC_OK, "Analyze at resonance f0");
    TEST_ASSERT(rlc.nature == CIRCUIT_RESONANT, "Circuit nature at f0 is RESONANT");
    TEST_ASSERT_APPROX(rlc.net_reactance, 0.0, 1e-3, "Net reactance at resonance approx 0.0 Ohm");
    TEST_ASSERT_APPROX(rlc.z_magnitude, 10.0, 1e-3, "Impedance at resonance |Z| == R == 10 Ohm");
    TEST_ASSERT_APPROX(rlc.i_rms, 12.0, 1e-3, "Current at resonance I = 120V / 10 Ohm = 12 A");
}

int main(void) {
    printf("==================================================\n");
    printf("  AC CIRCUIT ANALYZER - TEST SUITE: ELECTRICAL    \n");
    printf("==================================================\n");

    test_ohms_law();
    test_electrical_power();
    test_reactance();
    test_series_rlc();

    printf("\n==================================================\n");
    printf("Test Summary: %d / %d passed (%.1f%%)\n",
           g_tests_passed, g_tests_run,
           (double)g_tests_passed / (double)g_tests_run * 100.0);
    printf("==================================================\n");

    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
