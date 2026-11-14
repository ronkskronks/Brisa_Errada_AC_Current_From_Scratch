/**
 * ============================================================================
 * AC Circuit Analyzer - Electrical Calculations Implementation
 * ============================================================================
 * 
 * Implements fundamental circuit equations from scratch:
 * - Ohm's law: V = I * R
 * - AC Power: Active (P), Apparent (S), Reactive (Q), Power Factor
 * - Reactive components: XL = 2*pi*f*L, XC = 1 / (2*pi*f*C)
 * - Series RLC: Complex impedance, resonance, quality factor, voltages
 * ============================================================================
 */

#include "electrical.h"
#include <stddef.h>

/**
 * Calculates voltage given current and resistance: V = I * R
 */
elec_error_t ohm_calc_voltage(double current, double resistance, double *voltage) {
    if (voltage == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (resistance < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }
    *voltage = current * resistance;
    return ELEC_OK;
}

/**
 * Calculates current given voltage and resistance: I = V / R
 */
elec_error_t ohm_calc_current(double voltage, double resistance, double *current) {
    if (current == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (resistance < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }
    if (resistance == 0.0) {
        return ELEC_ERR_DIV_BY_ZERO;
    }
    *current = voltage / resistance;
    return ELEC_OK;
}

/**
 * Calculates resistance given voltage and current: R = V / I
 */
elec_error_t ohm_calc_resistance(double voltage, double current, double *resistance) {
    if (resistance == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (current == 0.0) {
        return ELEC_ERR_DIV_BY_ZERO;
    }
    double r = voltage / current;
    if (r < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }
    *resistance = r;
    return ELEC_OK;
}

/**
 * Calculates AC electrical power components given RMS voltage, RMS current,
 * and phase angle between voltage and current.
 * 
 * Physical Meaning:
 * - Active / Real Power P (W): Work actually performed or dissipated as heat.
 *     P = V_rms * I_rms * cos(theta)
 * - Apparent Power S (VA): Total capacity supplied by the source.
 *     S = V_rms * I_rms
 * - Reactive Power Q (VAR): Energy alternating between magnetic (inductor)
 *   and electric (capacitor) fields without net dissipation.
 *     Q = V_rms * I_rms * sin(theta)
 * - Power Factor PF: Ratio of real power to apparent power: PF = P / S = cos(theta).
 */
elec_error_t power_calc_from_vi_theta(double v_rms, double i_rms, double phase_rad, power_result_t *res) {
    if (res == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (v_rms < 0.0 || i_rms < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }

    res->v_rms = v_rms;
    res->i_rms = i_rms;
    res->phase_rad = phase_rad;
    res->phase_deg = math_rad2deg(phase_rad);

    res->apparent_power = v_rms * i_rms;
    res->active_power = res->apparent_power * math_cos(phase_rad);
    res->reactive_power = res->apparent_power * math_sin(phase_rad);

    /* Power factor is always in range [0, 1] */
    res->power_factor = math_fabs(math_cos(phase_rad));
    res->is_lagging = (phase_rad > 0.0); /* Voltage leads current -> Inductive (current lagging) */

    return ELEC_OK;
}

/**
 * Calculates apparent power and power factor from active and reactive powers.
 * Based on the Power Triangle: S = sqrt(P^2 + Q^2).
 */
elec_error_t power_calc_from_pq(double active_power, double reactive_power, power_result_t *res) {
    if (res == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    res->active_power = active_power;
    res->reactive_power = reactive_power;
    res->apparent_power = math_sqrt(active_power * active_power + reactive_power * reactive_power);

    if (res->apparent_power > 0.0) {
        res->power_factor = active_power / res->apparent_power;
        res->phase_rad = math_atan2(reactive_power, active_power);
        res->phase_deg = math_rad2deg(res->phase_rad);
    } else {
        res->power_factor = 1.0;
        res->phase_rad = 0.0;
        res->phase_deg = 0.0;
    }
    res->is_lagging = (reactive_power > 0.0);
    return ELEC_OK;
}

/**
 * Calculates Inductive Reactance: XL = 2 * pi * f * L
 * Physical Meaning:
 * An inductor opposes changes in current by inducing a back-EMF (Faraday/Lenz law).
 * As frequency f increases, the opposition (reactance) increases linearly.
 */
elec_error_t reactance_calc_inductive(double frequency, double inductance, double *xl) {
    if (xl == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (frequency < 0.0 || inductance < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }
    *xl = MATH_TWO_PI * frequency * inductance;
    return ELEC_OK;
}

/**
 * Calculates Capacitive Reactance: XC = 1 / (2 * pi * f * C)
 * Physical Meaning:
 * A capacitor stores electrical energy across dielectric plates. At DC (f=0),
 * it acts as an open circuit (XC -> infinity). As frequency increases, charge
 * transfers faster, and reactance decreases inversely.
 */
elec_error_t reactance_calc_capacitive(double frequency, double capacitance, double *xc) {
    if (xc == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (frequency < 0.0 || capacitance < 0.0) {
        return ELEC_ERR_NEGATIVE_VAL;
    }
    double denom = MATH_TWO_PI * frequency * capacitance;
    if (denom == 0.0) {
        return ELEC_ERR_DIV_BY_ZERO;
    }
    *xc = 1.0 / denom;
    return ELEC_OK;
}

/**
 * Analyzes a Series RLC Circuit.
 * 
 * Circuit Model:
 * A single loop containing resistor R, inductor L, and capacitor C in series with AC source.
 * Total complex impedance: Z = R + j*(XL - XC).
 * 
 * Key Physical Phenomena:
 * 1. Resonance occurs when XL = XC (net reactance is zero):
 *      2*pi*f0*L = 1 / (2*pi*f0*C) => f0 = 1 / (2*pi*sqrt(L*C)).
 * 2. At resonance, impedance reaches its minimum value |Z| = R,
 *    and current reaches its maximum value I = V / R.
 * 3. Quality Factor Q represents the ratio of stored reactive energy to dissipated energy per cycle.
 */
elec_error_t rlc_analyze(double r, double l, double c, double f, double v_rms, rlc_result_t *res) {
    if (res == NULL) {
        return ELEC_ERR_INVALID_ARG;
    }
    if (r < 0.0 || l <= 0.0 || c <= 0.0 || f <= 0.0 || v_rms < 0.0) {
        return ELEC_ERR_INVALID_ARG;
    }

    res->resistance = r;
    res->inductance = l;
    res->capacitance = c;
    res->frequency = f;
    res->v_rms = v_rms;

    res->angular_freq = MATH_TWO_PI * f;
    res->xl = res->angular_freq * l;
    res->xc = 1.0 / (res->angular_freq * c);
    res->net_reactance = res->xl - res->xc;

    res->z_complex = complex_create(r, res->net_reactance);
    res->z_magnitude = complex_mag(res->z_complex);

    if (res->z_magnitude > 0.0) {
        res->i_rms = v_rms / res->z_magnitude;
    } else {
        res->i_rms = 0.0;
    }

    res->phase_rad = complex_phase(res->z_complex);
    res->phase_deg = math_rad2deg(res->phase_rad);

    /* Determine circuit nature */
    if (math_fabs(res->net_reactance) < 1e-9) {
        res->nature = CIRCUIT_RESONANT;
    } else if (res->net_reactance > 0.0) {
        res->nature = CIRCUIT_INDUCTIVE;
    } else {
        res->nature = CIRCUIT_CAPACITIVE;
    }

    /* Resonance calculations */
    double lc_product = l * c;
    double sqrt_lc = math_sqrt(lc_product);
    if (sqrt_lc > 0.0) {
        res->resonant_freq = 1.0 / (MATH_TWO_PI * sqrt_lc);
    } else {
        res->resonant_freq = 0.0;
    }

    /* Quality factor and bandwidth */
    if (r > 0.0) {
        double w0 = MATH_TWO_PI * res->resonant_freq;
        res->quality_factor = (w0 * l) / r;
        if (res->quality_factor > 0.0) {
            res->bandwidth = res->resonant_freq / res->quality_factor;
        } else {
            res->bandwidth = 0.0;
        }
    } else {
        res->quality_factor = 999999.0; /* Ideal zero-resistance infinite Q */
        res->bandwidth = 0.0;
    }

    /* Component RMS voltages */
    res->vr_rms = res->i_rms * r;
    res->vl_rms = res->i_rms * res->xl;
    res->vc_rms = res->i_rms * res->xc;

    return ELEC_OK;
}
