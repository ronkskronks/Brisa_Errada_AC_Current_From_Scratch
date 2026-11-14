/**
 * ============================================================================
 * AC Circuit Analyzer - Freestanding Mathematical Engine Implementation
 * ============================================================================
 * 
 * This file implements all mathematical functions from scratch.
 * No headers from <math.h> or external math libraries are used.
 * 
 * Standard library headers included are strictly for standard types (<stddef.h>,
 * <stdint.h>, <stdbool.h>).
 * ============================================================================
 */

#include "math_engine.h"
#include <stdint.h>

/* Maximum iterations allowed in series and iterative methods */
#define MAX_SERIES_ITERATIONS 50

/**
 * Absolute value of a floating-point number.
 * Principle: |x| returns x if x >= 0, or -x if x < 0.
 */
double math_fabs(double x) {
    return (x < 0.0) ? -x : x;
}

/**
 * Absolute value of an integer.
 */
int math_abs(int x) {
    return (x < 0) ? -x : x;
}

/**
 * Floating-point comparison with configurable tolerance.
 * Principle: Two floating-point numbers a and b are considered approximately
 * equal if the absolute difference |a - b| is less than or equal to a given tolerance.
 */
bool math_approx_equal(double a, double b, double tolerance) {
    if (tolerance < 0.0) {
        tolerance = -tolerance;
    }
    return math_fabs(a - b) <= tolerance;
}

/**
 * Floating-point remainder (modulo) operation: x mod y.
 * Principle: Computes r = x - n * y, where n is the truncated integer quotient (x / y).
 * Limitations: Does not handle y = 0 (returns 0.0).
 */
double math_fmod(double x, double y) {
    if (y == 0.0) {
        return 0.0;
    }
    double abs_y = math_fabs(y);
    double abs_x = math_fabs(x);
    double quotient = abs_x / abs_y;
    int64_t i_quotient = (int64_t)quotient;
    double remainder = abs_x - (double)i_quotient * abs_y;
    if (remainder < 0.0) {
        remainder = 0.0;
    }
    return (x < 0.0) ? -remainder : remainder;
}

/**
 * Integer power calculation: base^exp using binary exponentiation (Square-and-Multiply).
 * Principle: Decomposes the exponent into powers of 2. Requires O(log2 |exp|) multiplications
 * rather than O(|exp|) multiplications, significantly reducing cumulative rounding error.
 * Limitations: For base = 0 and exp < 0, returns MATH_NAN_VAL (division by zero).
 */
double math_pow_int(double base, int exp) {
    if (exp == 0) {
        return 1.0;
    }
    if (base == 0.0) {
        if (exp < 0) {
            return MATH_NAN_VAL; /* Division by zero */
        }
        return 0.0;
    }

    int64_t e = (int64_t)exp;
    bool is_negative = false;
    if (e < 0) {
        is_negative = true;
        e = -e;
    }

    double result = 1.0;
    double current = base;

    while (e > 0) {
        if (e & 1) {
            result *= current;
        }
        current *= current;
        e >>= 1;
    }

    return is_negative ? (1.0 / result) : result;
}

/**
 * Factorial: n! = n * (n-1) * ... * 1.
 * Principle: Iterative multiplication.
 * Limitations: For n > 170, the result exceeds IEEE-754 double maximum value (~1.8e308),
 * returning MATH_NAN_VAL.
 */
double math_factorial(unsigned int n) {
    if (n > 170) {
        return MATH_NAN_VAL;
    }
    double result = 1.0;
    for (unsigned int i = 2; i <= n; i++) {
        result *= (double)i;
    }
    return result;
}

/**
 * Square root using the Newton-Raphson method with dynamic scale normalization.
 * 
 * Mathematical Principle:
 * To compute y = sqrt(x), we find the zero of f(y) = y^2 - x = 0.
 * Applying Newton-Raphson iteration:
 *   y_{k+1} = y_k - f(y_k) / f'(y_k)
 *   y_{k+1} = y_k - (y_k^2 - x) / (2 * y_k)
 *   y_{k+1} = 0.5 * (y_k + x / y_k)
 * 
 * Why it works:
 * The method exhibits quadratic convergence near the root: each iteration doubles
 * the number of significant digits.
 * 
 * Scale Normalization:
 * To avoid slow convergence for extremely large or small numbers, x is scaled to
 * m in [0.25, 4.0) such that x = m * 4^k. The initial guess y0 = 0.5 * (1.0 + m) is
 * extremely close to sqrt(m), converging in 4 to 8 iterations. Finally, y is rescaled
 * by 2^k.
 * 
 * Limitations:
 * Negative inputs produce a domain error (returns MATH_NAN_VAL).
 */
double math_sqrt(double x) {
    if (x < 0.0) {
        return MATH_NAN_VAL;
    }
    if (x == 0.0) {
        return 0.0;
    }

    /* Scale x to range [0.25, 4.0) */
    double m = x;
    int k = 0;
    while (m >= 4.0) {
        m *= 0.25;
        k++;
    }
    while (m < 0.25) {
        m *= 4.0;
        k--;
    }

    /* Initial guess: linear interpolation for sqrt(m) on [0.25, 4.0) */
    double y = 0.5 * (1.0 + m);

    /* Newton-Raphson iteration */
    for (int iter = 0; iter < 20; iter++) {
        double next_y = 0.5 * (y + m / y);
        if (math_fabs(next_y - y) <= 1e-15 * y) {
            y = next_y;
            break;
        }
        y = next_y;
    }

    /* Rescale back: sqrt(x) = sqrt(m) * 2^k */
    if (k > 0) {
        for (int i = 0; i < k; i++) {
            y *= 2.0;
        }
    } else if (k < 0) {
        for (int i = 0; i < -k; i++) {
            y *= 0.5;
        }
    }

    return y;
}

/**
 * Degrees to radians conversion.
 * rad = deg * (pi / 180).
 */
double math_deg2rad(double deg) {
    return deg * (MATH_PI / 180.0);
}

/**
 * Radians to degrees conversion.
 * deg = rad * (180 / pi).
 */
double math_rad2deg(double rad) {
    return rad * (180.0 / MATH_PI);
}

/**
 * Sine approximation using range reduction and Maclaurin/Taylor series.
 * 
 * Mathematical Principle:
 * The Taylor series of sin(x) centered at 0 is:
 *   sin(x) = x - x^3/3! + x^5/5! - x^7/7! + x^9/9! - ...
 * 
 * Why Range Reduction is Crucial:
 * Direct evaluation of Taylor series for large x (e.g. 2*pi*f*t) suffers from
 * catastrophic cancellation and divergence. Therefore:
 * 1. Reduce x modulo 2*pi into [-pi, pi].
 * 2. Use quadrant symmetry:
 *      if x > pi/2:  sin(x) = sin(pi - x)
 *      if x < -pi/2: sin(x) = sin(-pi - x)
 *    Now x lies strictly within [-pi/2, pi/2] (approx [-1.57, 1.57]).
 * 
 * On [-pi/2, pi/2], the terms shrink extremely quickly. Evaluating with recurrence:
 *   T_0 = x
 *   T_n = -T_{n-1} * (x^2) / ((2n) * (2n+1))
 * avoids computing factorials or large powers directly.
 * Reaches machine precision within 10 terms.
 */
double math_sin(double x) {
    /* Step 1: Modulo 2*PI reduction */
    x = math_fmod(x, MATH_TWO_PI);
    if (x > MATH_PI) {
        x -= MATH_TWO_PI;
    } else if (x < -MATH_PI) {
        x += MATH_TWO_PI;
    }

    /* Step 2: Symmetry reduction to [-PI/2, PI/2] */
    if (x > MATH_HALF_PI) {
        x = MATH_PI - x;
    } else if (x < -MATH_HALF_PI) {
        x = -MATH_PI - x;
    }

    /* Step 3: Taylor series evaluation */
    double x_sq = x * x;
    double term = x;
    double sum = x;

    for (int n = 1; n < MAX_SERIES_ITERATIONS; n++) {
        term = -term * x_sq / ((2.0 * n) * (2.0 * n + 1.0));
        sum += term;
        if (math_fabs(term) < 1e-16) {
            break;
        }
    }

    return sum;
}

/**
 * Cosine approximation using symmetry and Maclaurin/Taylor series.
 * 
 * Mathematical Principle:
 *   cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8! - ...
 * 
 * Range Reduction:
 * 1. Reduce x modulo 2*pi into [-pi, pi].
 * 2. Use even function symmetry: cos(x) = cos(|x|).
 * 3. If x > pi/2: cos(x) = -cos(pi - x), reducing argument to [0, pi/2].
 */
double math_cos(double x) {
    /* Step 1: Modulo 2*PI reduction */
    x = math_fmod(x, MATH_TWO_PI);
    if (x > MATH_PI) {
        x -= MATH_TWO_PI;
    } else if (x < -MATH_PI) {
        x += MATH_TWO_PI;
    }

    /* Step 2: Even symmetry cos(-x) = cos(x) */
    x = math_fabs(x);

    /* Step 3: Reduce to [0, PI/2] */
    double sign = 1.0;
    if (x > MATH_HALF_PI) {
        x = MATH_PI - x;
        sign = -1.0;
    }

    /* Step 4: Taylor series evaluation */
    double x_sq = x * x;
    double term = 1.0;
    double sum = 1.0;

    for (int n = 1; n < MAX_SERIES_ITERATIONS; n++) {
        term = -term * x_sq / ((2.0 * n - 1.0) * (2.0 * n));
        sum += term;
        if (math_fabs(term) < 1e-16) {
            break;
        }
    }

    return sign * sum;
}

/**
 * Tangent: tan(x) = sin(x) / cos(x).
 * Limitations: Near odd multiples of pi/2 (cos(x) ~ 0), returns MATH_NAN_VAL.
 */
double math_tan(double x) {
    double c = math_cos(x);
    if (math_fabs(c) < 1e-12) {
        return MATH_NAN_VAL; /* Asymptote / Division by zero */
    }
    return math_sin(x) / c;
}

/**
 * Arc tangent: atan(x) in radians [-pi/2, pi/2].
 * 
 * Mathematical Principle:
 * Maclaurin series for atan(u):
 *   atan(u) = u - u^3/3 + u^5/5 - u^7/7 + ...
 * This series converges only for |u| <= 1, and converges slowly near 1.
 * 
 * Argument Reduction:
 * 1. Odd symmetry: atan(-x) = -atan(x).
 * 2. Inversion identity: for x > 1:
 *      atan(x) = pi/2 - atan(1/x).
 * 3. Angle-addition identity: for x > (sqrt(2) - 1) approx 0.41421356:
 *      atan(x) = pi/4 + atan((x - 1) / (x + 1)).
 * 
 * This guarantees that the final reduced argument u satisfies |u| <= 0.41421356,
 * where the Maclaurin series converges in about 12-15 terms.
 */
double math_atan(double x) {
    if (x < 0.0) {
        return -math_atan(-x);
    }

    /* Inversion identity for x > 1 */
    if (x > 1.0) {
        return MATH_HALF_PI - math_atan(1.0 / x);
    }

    /* Reduction identity for x in (sqrt(2)-1, 1] */
    if (x > (MATH_SQRT2 - 1.0)) {
        double u = (x - 1.0) / (x + 1.0);
        return (MATH_PI / 4.0) + math_atan(u);
    }

    /* Series evaluation for |x| <= sqrt(2) - 1 */
    double x_sq = x * x;
    double pow_x = x;
    double sum = 0.0;

    for (int k = 0; k < MAX_SERIES_ITERATIONS; k++) {
        double divisor = 2.0 * k + 1.0;
        double term = pow_x / divisor;
        if ((k & 1) == 1) {
            sum -= term;
        } else {
            sum += term;
        }
        if (math_fabs(term) < 1e-16) {
            break;
        }
        pow_x *= x_sq;
    }

    return sum;
}

/**
 * Two-argument arc tangent atan2(y, x), returning the angle in radians in (-pi, pi].
 * Essential for AC circuit impedance phase angle determination.
 */
double math_atan2(double y, double x) {
    if (x > 0.0) {
        return math_atan(y / x);
    } else if (x < 0.0) {
        if (y >= 0.0) {
            return math_atan(y / x) + MATH_PI;
        } else {
            return math_atan(y / x) - MATH_PI;
        }
    } else { /* x == 0.0 */
        if (y > 0.0) {
            return MATH_HALF_PI;
        } else if (y < 0.0) {
            return -MATH_HALF_PI;
        } else {
            return 0.0; /* Both 0 */
        }
    }
}

/**
 * General polynomial/Taylor series evaluation using Horner's Method:
 * P(x) = c[0] + c[1]*(x - x0) + c[2]*(x - x0)^2 + ... + c[n-1]*(x - x0)^(n-1)
 * 
 * Horner's rule evaluates nested:
 * P(x) = c[0] + (x - x0) * (c[1] + (x - x0) * (c[2] + ... + (x - x0)*c[n-1]))
 * Complexity: O(n) additions and O(n) multiplications, minimizing catastrophic cancellation.
 */
double math_taylor_eval(double x0, const double *coefficients, int num_terms, double x) {
    if (coefficients == NULL || num_terms <= 0) {
        return 0.0;
    }
    double dx = x - x0;
    double result = coefficients[num_terms - 1];
    for (int i = num_terms - 2; i >= 0; i--) {
        result = result * dx + coefficients[i];
    }
    return result;
}

/**
 * Complex Number Operations for AC Impedance Calculations
 */

complex_t complex_create(double real, double imag) {
    complex_t c;
    c.real = real;
    c.imag = imag;
    return c;
}

complex_t complex_add(complex_t a, complex_t b) {
    complex_t res;
    res.real = a.real + b.real;
    res.imag = a.imag + b.imag;
    return res;
}

complex_t complex_sub(complex_t a, complex_t b) {
    complex_t res;
    res.real = a.real - b.real;
    res.imag = a.imag - b.imag;
    return res;
}

complex_t complex_mul(complex_t a, complex_t b) {
    complex_t res;
    res.real = a.real * b.real - a.imag * b.imag;
    res.imag = a.real * b.imag + a.imag * b.real;
    return res;
}

complex_t complex_div(complex_t a, complex_t b) {
    complex_t res;
    double denom = b.real * b.real + b.imag * b.imag;
    if (denom == 0.0) {
        res.real = MATH_NAN_VAL;
        res.imag = MATH_NAN_VAL;
        return res;
    }
    res.real = (a.real * b.real + a.imag * b.imag) / denom;
    res.imag = (a.imag * b.real - a.real * b.imag) / denom;
    return res;
}

double complex_mag(complex_t c) {
    return math_sqrt(c.real * c.real + c.imag * c.imag);
}

double complex_phase(complex_t c) {
    return math_atan2(c.imag, c.real);
}
