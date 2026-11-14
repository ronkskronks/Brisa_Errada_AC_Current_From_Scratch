/**
 * ============================================================================
 * AC Circuit Analyzer - Terminal ASCII Plotter & CSV Exporter Implementation
 * ============================================================================
 */

#include "graph.h"
#include "math_engine.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_WIDTH  70
#define DEFAULT_HEIGHT 21

graph_config_t graph_default_config(void) {
    graph_config_t cfg;
    cfg.width = DEFAULT_WIDTH;
    cfg.height = DEFAULT_HEIGHT;
    cfg.voltage_scale = 0.0; /* Auto */
    cfg.duration = 0.0;      /* Auto */
    cfg.num_cycles = 2;      /* Default to 2 full cycles */
    cfg.plot_char = '*';
    return cfg;
}

void graph_plot_ac_function(FILE *stream,
                           double v_peak,
                           double frequency,
                           double phase_rad,
                           const graph_config_t *config) {
    if (stream == NULL) {
        stream = stdout;
    }

    graph_config_t cfg = (config != NULL) ? *config : graph_default_config();

    if (cfg.width < 20) cfg.width = 20;
    if (cfg.width > 120) cfg.width = 120;
    if (cfg.height < 9) cfg.height = 9;
    if (cfg.height > 51) cfg.height = 51;
    /* Ensure odd height so center row is exact 0V */
    if ((cfg.height % 2) == 0) {
        cfg.height++;
    }

    double v_scale = (cfg.voltage_scale > 0.0) ? cfg.voltage_scale : v_peak;
    if (v_scale <= 0.0) {
        v_scale = 1.0;
    }

    double period = 1.0 / frequency;
    double t_duration = cfg.duration;
    if (t_duration <= 0.0) {
        int cycles = (cfg.num_cycles > 0) ? cfg.num_cycles : 2;
        t_duration = cycles * period;
    }

    int width = cfg.width;
    int height = cfg.height;

    /* Allocate grid on heap */
    char *grid = (char *)malloc(height * width);
    if (grid == NULL) {
        return;
    }
    memset(grid, ' ', height * width);

    /* Draw zero-voltage axis */
    int zero_row = height / 2;
    for (int x = 0; x < width; x++) {
        grid[zero_row * width + x] = '-';
    }

    /* Plot curve across time columns */
    int prev_row = -1;
    for (int x = 0; x < width; x++) {
        double t = (double)x * (t_duration / (double)(width - 1));
        double v = waveform_instantaneous_voltage(v_peak, frequency, phase_rad, t);

        /* Map voltage [-v_scale, +v_scale] to row [height-1, 0] */
        double normalized = (v - (-v_scale)) / (2.0 * v_scale);
        if (normalized < 0.0) normalized = 0.0;
        if (normalized > 1.0) normalized = 1.0;

        int row = (height - 1) - (int)(normalized * (height - 1) + 0.5);
        if (row < 0) row = 0;
        if (row >= height) row = height - 1;

        grid[row * width + x] = cfg.plot_char;

        /* Interpolate vertical gaps to make continuous curve */
        if (prev_row != -1 && math_abs(row - prev_row) > 1) {
            int step = (row > prev_row) ? 1 : -1;
            for (int r = prev_row + step; r != row; r += step) {
                if (grid[r * width + x] == ' ' || grid[r * width + x] == '-') {
                    grid[r * width + x] = (step > 0) ? '\\' : '/';
                }
            }
        }
        prev_row = row;
    }

    /* Render graph to stream */
    fprintf(stream, "\n--- AC Waveform Plot (Peak: %.1f V, Freq: %.1f Hz, Duration: %.4f s) ---\n\n",
            v_peak, frequency, t_duration);

    for (int r = 0; r < height; r++) {
        if (r == 0) {
            fprintf(stream, "%8.1f V |", v_scale);
        } else if (r == zero_row) {
            fprintf(stream, "%8.1f V |", 0.0);
        } else if (r == height - 1) {
            fprintf(stream, "%8.1f V |", -v_scale);
        } else {
            fprintf(stream, "         |");
        }

        for (int c = 0; c < width; c++) {
            fputc(grid[r * width + c], stream);
        }
        fputc('\n', stream);
    }

    /* Bottom time axis */
    fprintf(stream, "         +");
    for (int c = 0; c < width; c++) {
        fputc('-', stream);
    }
    fprintf(stream, "> Time\n");

    /* Time axis markings */
    fprintf(stream, "         0.0s");
    int mid_spaces = width / 2 - 4;
    for (int i = 0; i < mid_spaces; i++) fputc(' ', stream);
    fprintf(stream, "%.4fs", t_duration / 2.0);
    int end_spaces = width - (width / 2) - 8;
    for (int i = 0; i < end_spaces; i++) fputc(' ', stream);
    fprintf(stream, "%.4fs\n\n", t_duration);

    free(grid);
}

void graph_plot_waveform(FILE *stream, const waveform_t *wf, const graph_config_t *config) {
    if (wf == NULL) {
        return;
    }
    graph_config_t cfg = (config != NULL) ? *config : graph_default_config();
    if (cfg.duration <= 0.0 && wf->duration > 0.0) {
        cfg.duration = wf->duration;
    }
    graph_plot_ac_function(stream, wf->v_peak, wf->frequency, wf->phase_rad, &cfg);
}

bool graph_export_csv(const char *filename, const waveform_t *wf) {
    if (filename == NULL || wf == NULL || wf->samples == NULL || wf->sample_count == 0) {
        return false;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }

    /* Write CSV Header */
    fprintf(fp, "time_seconds,voltage_volts,current_amperes\n");

    /* Write data rows */
    for (size_t i = 0; i < wf->sample_count; i++) {
        fprintf(fp, "%.8e,%.6f,%.6f\n",
                wf->samples[i].time,
                wf->samples[i].voltage,
                wf->samples[i].current);
    }

    fclose(fp);
    return true;
}
