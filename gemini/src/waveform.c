/**
 * ============================================================================
 * AC Circuit Analyzer - AC Sinusoidal Waveform Implementation
 * ============================================================================
 */

#include "waveform.h"
#include <stdlib.h>

#define MAX_ALLOWED_SAMPLES 500000

double waveform_instantaneous_voltage(double v_peak, double freq, double phase_rad, double t) {
    double omega = MATH_TWO_PI * freq;
    return v_peak * math_sin(omega * t + phase_rad);
}

double waveform_instantaneous_current(double v_peak, double freq, double phase_rad, double r, double t) {
    if (r <= 0.0) {
        return 0.0;
    }
    double v = waveform_instantaneous_voltage(v_peak, freq, phase_rad, t);
    return v / r;
}

bool waveform_init(waveform_t *wf,
                   double v_peak,
                   double frequency,
                   double phase_rad,
                   double resistance,
                   double duration,
                   double sample_interval) {
    if (wf == NULL) {
        return false;
    }
    if (v_peak < 0.0 || frequency <= 0.0 || resistance <= 0.0 ||
        duration <= 0.0 || sample_interval <= 0.0) {
        return false;
    }

    /* Initialize metrics */
    wf->v_peak = v_peak;
    wf->frequency = frequency;
    wf->phase_rad = phase_rad;
    wf->load_resistance = resistance;
    wf->duration = duration;
    wf->sample_interval = sample_interval;

    wf->angular_freq = MATH_TWO_PI * frequency;
    wf->period = 1.0 / frequency;
    wf->v_peak_to_peak = 2.0 * v_peak;
    wf->v_rms = v_peak / math_sqrt(2.0);
    wf->i_peak = v_peak / resistance;
    wf->i_rms = wf->i_peak / math_sqrt(2.0);

    /* Allocate sample buffer */
    size_t count = (size_t)(duration / sample_interval) + 1;
    if (count > MAX_ALLOWED_SAMPLES) {
        count = MAX_ALLOWED_SAMPLES;
    }

    wf->sample_count = count;
    wf->samples = (waveform_sample_t *)malloc(count * sizeof(waveform_sample_t));
    if (wf->samples == NULL) {
        wf->sample_count = 0;
        return false;
    }

    /* Populate discrete samples */
    for (size_t k = 0; k < count; k++) {
        double t = (double)k * sample_interval;
        wf->samples[k].time = t;
        wf->samples[k].voltage = waveform_instantaneous_voltage(v_peak, frequency, phase_rad, t);
        wf->samples[k].current = wf->samples[k].voltage / resistance;
    }

    return true;
}

void waveform_free(waveform_t *wf) {
    if (wf != NULL && wf->samples != NULL) {
        free(wf->samples);
        wf->samples = NULL;
        wf->sample_count = 0;
    }
}
