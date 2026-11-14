#ifndef ELECTRICAL_H
#define ELECTRICAL_H

#include <stdbool.h>
#include "math_engine.h"

/**
 * ============================================================================
 * AC Circuit Analyzer - Electrical Calculations Module
 * ============================================================================
 * 
 * Provides equations and data structures for:
 * - Ohm's Law (DC and instantaneous AC)
 * - Electrical Power (Active, Apparent, Reactive, Power Factor)
 * - Inductive and Capacitive Reactance
 * - Series RLC Circuit Analysis and Resonance
 * ============================================================================
 */

/* Error codes for electrical calculations */
typedef enum {
    ELEC_OK = 0,
    ELEC_ERR_INVALID_ARG = -1,
    ELEC_ERR_DIV_BY_ZERO = -2,
    ELEC_ERR_NEGATIVE_VAL = -3
} elec_error_t;

/* Circuit nature based on reactance */
typedef enum {
    CIRCUIT_RESONANT = 0,
    CIRCUIT_INDUCTIVE = 1,
    CIRCUIT_CAPACITIVE = 2
} circuit_nature_t;

/* Result structure for Ohm's Law */
typedef struct {
    double voltage;    /* Volts (V) */
    double current;    /* Amperes (A) */
    double resistance; /* Ohms (Ω) */
} ohm_result_t;

/* Result structure for AC Electrical Power */
typedef struct {
    double v_rms;          /* RMS Voltage (V) */
    double i_rms;          /* RMS Current (A) */
    double phase_rad;      /* Phase angle (radians) */
    double phase_deg;      /* Phase angle (degrees) */
    double active_power;   /* Active / Real Power P (Watts, W) */
    double apparent_power; /* Apparent Power S (Volt-Amperes, VA) */
    double reactive_power; /* Reactive Power Q (Volt-Amperes Reactive, VAR) */
    double power_factor;   /* Power Factor PF = cos(theta) (0.0 to 1.0) */
    bool is_lagging;       /* True if current lags voltage (inductive), false if leading */
} power_result_t;

/* Result structure for Series RLC Circuit Analysis */
typedef struct {
    /* Inputs */
    double resistance;       /* R in Ohms (Ω) */
    double inductance;       /* L in Henries (H) */
    double capacitance;      /* C in Farads (F) */
    double frequency;        /* f in Hertz (Hz) */
    double v_rms;            /* Applied RMS Voltage (V) */

    /* Outputs */
    double angular_freq;     /* omega = 2*pi*f (rad/s) */
    double xl;               /* Inductive Reactance XL (Ω) */
    double xc;               /* Capacitive Reactance XC (Ω) */
    double net_reactance;    /* Net Reactance X = XL - XC (Ω) */
    complex_t z_complex;     /* Complex Impedance Z = R + j(XL - XC) (Ω) */
    double z_magnitude;      /* Scalar Impedance |Z| = sqrt(R^2 + X^2) (Ω) */
    double i_rms;            /* RMS Current I = V / |Z| (A) */
    double phase_rad;        /* Phase angle theta (rad) */
    double phase_deg;        /* Phase angle theta (deg) */
    circuit_nature_t nature; /* Inductive, Capacitive, or Resonant */
    double resonant_freq;    /* Resonant Frequency f0 (Hz) */
    double quality_factor;   /* Quality Factor Q */
    double bandwidth;        /* Bandwidth BW (Hz) */
    double vr_rms;           /* Voltage across Resistor (V) */
    double vl_rms;           /* Voltage across Inductor (V) */
    double vc_rms;           /* Voltage across Capacitor (V) */
} rlc_result_t;

/* --- Ohm's Law Functions --- */
elec_error_t ohm_calc_voltage(double current, double resistance, double *voltage);
elec_error_t ohm_calc_current(double voltage, double resistance, double *current);
elec_error_t ohm_calc_resistance(double voltage, double current, double *resistance);

/* --- AC Electrical Power Functions --- */
elec_error_t power_calc_from_vi_theta(double v_rms, double i_rms, double phase_rad, power_result_t *res);
elec_error_t power_calc_from_pq(double active_power, double reactive_power, power_result_t *res);

/* --- Reactance Calculations --- */
elec_error_t reactance_calc_inductive(double frequency, double inductance, double *xl);
elec_error_t reactance_calc_capacitive(double frequency, double capacitance, double *xc);

/* --- Series RLC Circuit Analysis --- */
elec_error_t rlc_analyze(double r, double l, double c, double f, double v_rms, rlc_result_t *res);

#endif /* ELECTRICAL_H */
