#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdbool.h>
#include "waveform.h"

/**
 * ============================================================================
 * AC Circuit Analyzer - Terminal ASCII Plotter and CSV Exporter
 * ============================================================================
 */

typedef struct {
    int width;              /* Graph width in characters (time axis) */
    int height;             /* Graph height in rows (voltage axis) */
    double voltage_scale;   /* Peak voltage scale for y-axis (0 = auto) */
    double duration;        /* Duration in seconds (0 = auto from num_cycles) */
    int num_cycles;         /* Number of AC cycles to display (if duration=0) */
    char plot_char;         /* Character to draw waveform */
} graph_config_t;

/* Returns standard default configuration */
graph_config_t graph_default_config(void);

/* Plot waveform directly to given file stream (e.g. stdout) */
void graph_plot_waveform(FILE *stream, const waveform_t *wf, const graph_config_t *config);

/* Plot AC function without preallocated waveform */
void graph_plot_ac_function(FILE *stream,
                           double v_peak,
                           double frequency,
                           double phase_rad,
                           const graph_config_t *config);

/* Export waveform samples to CSV file */
bool graph_export_csv(const char *filename, const waveform_t *wf);

#endif /* GRAPH_H */
