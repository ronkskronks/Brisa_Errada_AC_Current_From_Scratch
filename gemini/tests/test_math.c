/**
 * ============================================================================
 * AC Circuit Analyzer - Mathematical Engine Unit Tests
 * ============================================================================
 */

#include <stdio.h>
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
        printf("  [PASS] %s (Got: %.8f, Exp: %.8f)\n", msg, _v, _exp); \
    } else { \
        printf("  [FAIL] %s (Got: %.8f, Exp: %.8f, Tol: %.8f, Line %d)\n", msg, _v, _exp, tol, __LINE__); \
    } \
} while(0)

void test_basic_ops(void) {
    printf("\n=== Running Basic Operations Tests ===\n");
    TEST_ASSERT(math_abs(5) == 5, "math_abs(5) == 5");
    TEST_ASSERT(math_abs(-5) == 5, "math_abs(-5) == 5");
    TEST_ASSERT(math_abs(0) == 0, "math_abs(0) == 0");

    TEST_ASSERT(math_fabs(3.1415) == 3.1415, "math_fabs(3.1415) == 3.1415");
    TEST_ASSERT(math_fabs(-3.1415) == 3.1415, "math_fabs(-3.1415) == 3.1415");
    TEST_ASSERT(math_fabs(0.0) == 0.0, "math_fabs(0.0) == 0.0");

    TEST_ASSERT(math_approx_equal(1.0000001, 1.0000002, 1e-6), "math_approx_equal within 1e-6");
    TEST_ASSERT(!math_approx_equal(1.0, 1.1, 1e-3), "!math_approx_equal outside tolerance");

    TEST_ASSERT_APPROX(math_fmod(7.0, 3.0), 1.0, 1e-9, "math_fmod(7.0, 3.0) == 1.0");
    TEST_ASSERT_APPROX(math_fmod(6.2831853, 3.14159265), 0.0, 1e-6, "math_fmod(2*pi, pi) == 0.0");
    TEST_ASSERT_APPROX(math_fmod(-7.0, 3.0), -1.0, 1e-9, "math_fmod(-7.0, 3.0) == -1.0");
}

void test_power_and_factorial(void) {
    printf("\n=== Running Power and Factorial Tests ===\n");
    TEST_ASSERT_APPROX(math_pow_int(2.0, 10), 1024.0, 1e-9, "2^10 == 1024");
    TEST_ASSERT_APPROX(math_pow_int(3.0, 0), 1.0, 1e-9, "3^0 == 1");
    TEST_ASSERT_APPROX(math_pow_int(5.0, 3), 125.0, 1e-9, "5^3 == 125");
    TEST_ASSERT_APPROX(math_pow_int(2.0, -3), 0.125, 1e-9, "2^-3 == 0.125");
    TEST_ASSERT_APPROX(math_pow_int(-2.0, 3), -8.0, 1e-9, "(-2)^3 == -8");
    TEST_ASSERT_APPROX(math_pow_int(-2.0, 4), 16.0, 1e-9, "(-2)^4 == 16");

    TEST_ASSERT_APPROX(math_factorial(0), 1.0, 1e-9, "0! == 1");
    TEST_ASSERT_APPROX(math_factorial(1), 1.0, 1e-9, "1! == 1");
    TEST_ASSERT_APPROX(math_factorial(5), 120.0, 1e-9, "5! == 120");
    TEST_ASSERT_APPROX(math_factorial(10), 3628800.0, 1e-9, "10! == 3628800");
    TEST_ASSERT(math_factorial(200) == MATH_NAN_VAL, "Factorial > 170 returns overflow sentinel");
}

void test_sqrt(void) {
    printf("\n=== Running Square Root (Newton-Raphson) Tests ===\n");
    TEST_ASSERT_APPROX(math_sqrt(0.0), 0.0, 1e-9, "sqrt(0) == 0");
    TEST_ASSERT_APPROX(math_sqrt(1.0), 1.0, 1e-9, "sqrt(1) == 1");
    TEST_ASSERT_APPROX(math_sqrt(4.0), 2.0, 1e-9, "sqrt(4) == 2");
    TEST_ASSERT_APPROX(math_sqrt(25.0), 5.0, 1e-9, "sqrt(25) == 5");
    TEST_ASSERT_APPROX(math_sqrt(2.0), 1.41421356237, 1e-9, "sqrt(2) == 1.41421356...");
    TEST_ASSERT_APPROX(math_sqrt(10000.0), 100.0, 1e-9, "sqrt(10000) == 100");
    TEST_ASSERT_APPROX(math_sqrt(0.0001), 0.01, 1e-9, "sqrt(0.0001) == 0.01");
    TEST_ASSERT(math_sqrt(-4.0) == MATH_NAN_VAL, "sqrt(-4) returns domain error sentinel");
}

void test_angles_and_trig(void) {
    printf("\n=== Running Trigonometric Tests ===\n");
    TEST_ASSERT_APPROX(math_deg2rad(180.0), MATH_PI, 1e-9, "180 deg to rad == pi");
    TEST_ASSERT_APPROX(math_rad2deg(MATH_PI), 180.0, 1e-9, "pi rad to deg == 180");
    TEST_ASSERT_APPROX(math_deg2rad(90.0), MATH_HALF_PI, 1e-9, "90 deg to rad == pi/2");

    /* Sine tests */
    TEST_ASSERT_APPROX(math_sin(0.0), 0.0, 1e-9, "sin(0) == 0");
    TEST_ASSERT_APPROX(math_sin(MATH_PI / 6.0), 0.5, 1e-9, "sin(pi/6) == 0.5");
    TEST_ASSERT_APPROX(math_sin(MATH_PI / 4.0), 0.707106781, 1e-8, "sin(pi/4) == sqrt(2)/2");
    TEST_ASSERT_APPROX(math_sin(MATH_HALF_PI), 1.0, 1e-9, "sin(pi/2) == 1");
    TEST_ASSERT_APPROX(math_sin(MATH_PI), 0.0, 1e-9, "sin(pi) == 0");
    TEST_ASSERT_APPROX(math_sin(3.0 * MATH_PI / 2.0), -1.0, 1e-9, "sin(3*pi/2) == -1");
    TEST_ASSERT_APPROX(math_sin(MATH_TWO_PI), 0.0, 1e-9, "sin(2*pi) == 0");
    TEST_ASSERT_APPROX(math_sin(-MATH_HALF_PI), -1.0, 1e-9, "sin(-pi/2) == -1");
    TEST_ASSERT_APPROX(math_sin(100.0 * MATH_PI + MATH_HALF_PI), 1.0, 1e-8, "sin(100*pi + pi/2) == 1 (Range reduction)");

    /* Cosine tests */
    TEST_ASSERT_APPROX(math_cos(0.0), 1.0, 1e-9, "cos(0) == 1");
    TEST_ASSERT_APPROX(math_cos(MATH_PI / 3.0), 0.5, 1e-9, "cos(pi/3) == 0.5");
    TEST_ASSERT_APPROX(math_cos(MATH_HALF_PI), 0.0, 1e-9, "cos(pi/2) == 0");
    TEST_ASSERT_APPROX(math_cos(MATH_PI), -1.0, 1e-9, "cos(pi) == -1");
    TEST_ASSERT_APPROX(math_cos(3.0 * MATH_PI / 2.0), 0.0, 1e-9, "cos(3*pi/2) == 0");
    TEST_ASSERT_APPROX(math_cos(MATH_TWO_PI), 1.0, 1e-9, "cos(2*pi) == 1");
    TEST_ASSERT_APPROX(math_cos(100.0 * MATH_PI), 1.0, 1e-8, "cos(100*pi) == 1 (Range reduction)");

    /* Tangent tests */
    TEST_ASSERT_APPROX(math_tan(0.0), 0.0, 1e-9, "tan(0) == 0");
    TEST_ASSERT_APPROX(math_tan(MATH_PI / 4.0), 1.0, 1e-8, "tan(pi/4) == 1");
    TEST_ASSERT_APPROX(math_tan(-MATH_PI / 4.0), -1.0, 1e-8, "tan(-pi/4) == -1");
    TEST_ASSERT(math_tan(MATH_HALF_PI) == MATH_NAN_VAL, "tan(pi/2) detects asymptote / division by zero");

    /* Arctangent and atan2 tests */
    TEST_ASSERT_APPROX(math_atan(0.0), 0.0, 1e-9, "atan(0) == 0");
    TEST_ASSERT_APPROX(math_atan(1.0), MATH_PI / 4.0, 1e-9, "atan(1) == pi/4");
    TEST_ASSERT_APPROX(math_atan(-1.0), -MATH_PI / 4.0, 1e-9, "atan(-1) == -pi/4");

    TEST_ASSERT_APPROX(math_atan2(0.0, 1.0), 0.0, 1e-9, "atan2(0, 1) == 0");
    TEST_ASSERT_APPROX(math_atan2(1.0, 1.0), MATH_PI / 4.0, 1e-9, "atan2(1, 1) == pi/4");
    TEST_ASSERT_APPROX(math_atan2(1.0, 0.0), MATH_HALF_PI, 1e-9, "atan2(1, 0) == pi/2");
    TEST_ASSERT_APPROX(math_atan2(-1.0, -1.0), -3.0 * MATH_PI / 4.0, 1e-9, "atan2(-1, -1) == -3*pi/4");
}

void test_taylor_and_complex(void) {
    printf("\n=== Running Horner/Taylor Evaluation and Complex Numbers Tests ===\n");
    /* Test polynomial: P(x) = 2 + 3*x + 1*x^2 at x = 4 -> 2 + 12 + 16 = 30 */
    double coeffs[] = {2.0, 3.0, 1.0};
    double p_val = math_taylor_eval(0.0, coeffs, 3, 4.0);
    TEST_ASSERT_APPROX(p_val, 30.0, 1e-9, "math_taylor_eval: 2 + 3*x + x^2 at x=4 is 30");

    /* Complex operations */
    complex_t c1 = complex_create(3.0, 4.0);
    complex_t c2 = complex_create(1.0, -2.0);

    complex_t sum = complex_add(c1, c2);
    TEST_ASSERT_APPROX(sum.real, 4.0, 1e-9, "(3+4j) + (1-2j) real == 4");
    TEST_ASSERT_APPROX(sum.imag, 2.0, 1e-9, "(3+4j) + (1-2j) imag == 2");

    complex_t diff = complex_sub(c1, c2);
    TEST_ASSERT_APPROX(diff.real, 2.0, 1e-9, "(3+4j) - (1-2j) real == 2");
    TEST_ASSERT_APPROX(diff.imag, 6.0, 1e-9, "(3+4j) - (1-2j) imag == 6");

    complex_t prod = complex_mul(c1, c2); /* (3+4j)*(1-2j) = 3 - 6j + 4j + 8 = 11 - 2j */
    TEST_ASSERT_APPROX(prod.real, 11.0, 1e-9, "(3+4j) * (1-2j) real == 11");
    TEST_ASSERT_APPROX(prod.imag, -2.0, 1e-9, "(3+4j) * (1-2j) imag == -2");

    complex_t quot = complex_div(prod, c2); /* (11-2j)/(1-2j) should equal c1 = 3+4j */
    TEST_ASSERT_APPROX(quot.real, 3.0, 1e-9, "(11-2j) / (1-2j) real == 3");
    TEST_ASSERT_APPROX(quot.imag, 4.0, 1e-9, "(11-2j) / (1-2j) imag == 4");

    double mag = complex_mag(c1); /* sqrt(3^2 + 4^2) = 5 */
    TEST_ASSERT_APPROX(mag, 5.0, 1e-9, "Magnitude of 3+4j == 5");

    complex_t c_diag = complex_create(1.0, 1.0);
    double phase_deg = math_rad2deg(complex_phase(c_diag));
    TEST_ASSERT_APPROX(phase_deg, 45.0, 1e-9, "Phase of 1+1j == 45 degrees");
}

int main(void) {
    printf("==================================================\n");
    printf("  AC CIRCUIT ANALYZER - TEST SUITE: MATH ENGINE   \n");
    printf("==================================================\n");

    test_basic_ops();
    test_power_and_factorial();
    test_sqrt();
    test_angles_and_trig();
    test_taylor_and_complex();

    printf("\n==================================================\n");
    printf("Test Summary: %d / %d passed (%.1f%%)\n",
           g_tests_passed, g_tests_run,
           (double)g_tests_passed / (double)g_tests_run * 100.0);
    printf("==================================================\n");

    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
