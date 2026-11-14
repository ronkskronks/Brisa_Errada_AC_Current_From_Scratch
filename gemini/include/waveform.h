#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <stddef.h>
#include <stdbool.h>
#include "math_engine.h"

/**
 * ============================================================================
 * AC Circuit Analyzer - AC Sinusoidal Waveform Generator
 * ============================================================================
 * 
 * Implements generation and sampling of AC voltage and current waveforms:
 *   v(t) = Vpeak * sin(2 * pi * f * t + phi)
 *   i(t) = v(t) / R  (purely resistive load)
 * ============================================================================
 */

typedef struct {
    double time;             /* Time in seconds (t) */
    double voltage;          /* Instantaneous voltage v(t) in Volts (V) */
    double current;          /* Instantaneous current i(t) in Amperes (A) */
} waveform_sample_t;

typedef struct {
    /* Configuration */
    double v_peak;           /* Peak Voltage Vpeak (V) */
    double frequency;        /* Frequency f (Hz) */
    double phase_rad;        /* Phase angle phi (radians) */
    double load_resistance;  /* Purely resistive load R (Ω) */
    double duration;         /* Total duration in seconds */
    double sample_interval;  /* Time step dt between samples (seconds) */

    /* Calculated Metrics */
    double angular_freq;     /* Angular frequency omega = 2*pi*f (rad/s) */
    double period;           /* Period T = 1 / f (s) */
    double v_peak_to_peak;   /* Vpp = 2 * Vpeak (V) */
    double v_rms;            /* Vrms = Vpeak / sqrt(2) (V) */
    double i_peak;           /* Ipeak = Vpeak / R (A) */
    double i_rms;            /* Irms = Ipeak / sqrt(2) (A) */

    /* Sample buffer */
    size_t sample_count;
    waveform_sample_t *samples;
} waveform_t;

/* --- Generator Functions --- */
bool waveform_init(waveform_t *wf,
                   double v_peak,
                   double frequency,
                   double phase_rad,
                   double resistance,
                   double duration,
                   double sample_interval);

void waveform_free(waveform_t *wf);

double waveform_instantaneous_voltage(double v_peak, double freq, double phase_rad, double t);
double waveform_instantaneous_current(double v_peak, double freq, double phase_rad, double r, double t);

#endif /* WAVEFORM_H */
