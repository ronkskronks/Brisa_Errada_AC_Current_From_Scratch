#ifndef MATH_ENGINE_H
#define MATH_ENGINE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * ============================================================================
 * AC Circuit Analyzer - Freestanding Mathematical Engine
 * ============================================================================
 * 
 * This module provides mathematical constants, basic arithmetic functions,
 * power and root functions, trigonometric approximations, polynomial evaluation,
 * and complex number operations implemented entirely from scratch without
 * relying on <math.h> or external math libraries.
 * 
 * Standard C headers included are strictly for type definitions (stdbool, stddef).
 * ============================================================================
 */

/* --- Fundamental Mathematical Constants --- */
#define MATH_PI          3.141592653589793238462643383279502884
#define MATH_TWO_PI      6.283185307179586476925286766559005768
#define MATH_HALF_PI     1.570796326794896619231321691639751442
#define MATH_E           2.718281828459045235360287471352662497
#define MATH_SQRT2       1.414213562373095048801688724209698078
#define MATH_EPSILON     1e-9

/* Error sentinel values */
#define MATH_NAN_VAL     (-999999999.0)

/* Status codes for mathematical operations */
typedef enum {
    MATH_OK = 0,
    MATH_ERR_INVALID_ARG = -1,
    MATH_ERR_DIV_BY_ZERO = -2,
    MATH_ERR_DOMAIN      = -3,
    MATH_ERR_OVERFLOW    = -4
} math_error_t;

/* Complex Number Structure for AC Impedance Calculations */
typedef struct {
    double real; /* Real component (e.g. Resistance R) */
    double imag; /* Imaginary component (e.g. Reactance X = XL - XC) */
} complex_t;

/* --- Basic Operations --- */
double math_fabs(double x);
int    math_abs(int x);
bool   math_approx_equal(double a, double b, double tolerance);
double math_fmod(double x, double y);

/* --- Power, Factorial, and Roots --- */
double math_pow_int(double base, int exp);
double math_factorial(unsigned int n);
double math_sqrt(double x);

/* --- Angle Conversions --- */
double math_deg2rad(double deg);
double math_rad2deg(double rad);

/* --- Trigonometric Functions (Taylor Series Approximations) --- */
double math_sin(double x);
double math_cos(double x);
double math_tan(double x);
double math_atan(double x);
double math_atan2(double y, double x);

/* --- Polynomial and Taylor Series Evaluation --- */
double math_taylor_eval(double x0, const double *coefficients, int num_terms, double x);

/* --- Complex Arithmetic Operations --- */
complex_t complex_create(double real, double imag);
complex_t complex_add(complex_t a, complex_t b);
complex_t complex_sub(complex_t a, complex_t b);
complex_t complex_mul(complex_t a, complex_t b);
complex_t complex_div(complex_t a, complex_t b);
double    complex_mag(complex_t c);
double    complex_phase(complex_t c);

#endif /* MATH_ENGINE_H */
