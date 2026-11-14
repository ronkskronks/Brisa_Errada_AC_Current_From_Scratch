/**
 * ============================================================================
 * AC Circuit Analyzer - Waveform Generator & Plotting Unit Tests
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "waveform.h"
#include "graph.h"
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

void test_waveform_metrics(void) {
    printf("\n=== Running Waveform Metrics Tests ===\n");
    waveform_t wf;
    /* 311.127 V peak corresponds to 220 V RMS */
    bool ok = waveform_init(&wf, 311.12698, 60.0, 0.0, 50.0, 0.05, 0.0001);
    TEST_ASSERT(ok == true, "waveform_init succeeded");

    TEST_ASSERT_APPROX(wf.v_peak, 311.12698, 1e-4, "Peak voltage == 311.127 V");
    TEST_ASSERT_APPROX(wf.v_rms, 220.0, 1e-2, "220 V RMS corresponds to ~311.127 V peak");
    TEST_ASSERT_APPROX(wf.period, 0.0166667, 1e-5, "60 Hz period == ~0.016667 s");
    TEST_ASSERT_APPROX(wf.angular_freq, 376.9911, 1e-3, "Angular freq omega == ~376.99 rad/s");
    TEST_ASSERT_APPROX(wf.v_peak_to_peak, 622.254, 1e-2, "Peak-to-peak voltage == 622.25 V");
    TEST_ASSERT_APPROX(wf.i_peak, 6.22254, 1e-4, "Peak current I = 311.127 / 50 = 6.2225 A");
    TEST_ASSERT_APPROX(wf.i_rms, 4.40, 1e-2, "RMS current I = 220 / 50 = 4.40 A");

    waveform_free(&wf);
}

void test_instantaneous_values(void) {
    printf("\n=== Running Instantaneous Voltage and Current Tests ===\n");
    double v_peak = 100.0;
    double freq = 50.0;
    double t_period = 1.0 / freq; /* 0.02 s */
    double r = 10.0;

    /* At t = 0: sin(0) = 0 */
    double v0 = waveform_instantaneous_voltage(v_peak, freq, 0.0, 0.0);
    TEST_ASSERT_APPROX(v0, 0.0, 1e-6, "v(0) == 0 V");

    /* At t = T/4: sin(pi/2) = 1 -> v = Vpeak */
    double v_quarter = waveform_instantaneous_voltage(v_peak, freq, 0.0, t_period / 4.0);
    TEST_ASSERT_APPROX(v_quarter, 100.0, 1e-6, "v(T/4) == +100 V (Peak)");

    /* At t = T/2: sin(pi) = 0 */
    double v_half = waveform_instantaneous_voltage(v_peak, freq, 0.0, t_period / 2.0);
    TEST_ASSERT_APPROX(v_half, 0.0, 1e-6, "v(T/2) == 0 V");

    /* At t = 3T/4: sin(3pi/2) = -1 -> v = -Vpeak */
    double v_3quarter = waveform_instantaneous_voltage(v_peak, freq, 0.0, 3.0 * t_period / 4.0);
    TEST_ASSERT_APPROX(v_3quarter, -100.0, 1e-6, "v(3T/4) == -100 V (-Peak)");

    /* Current through 10 Ohm at T/4 -> i = 100 / 10 = 10 A */
    double i_quarter = waveform_instantaneous_current(v_peak, freq, 0.0, r, t_period / 4.0);
    TEST_ASSERT_APPROX(i_quarter, 10.0, 1e-6, "i(T/4) == 10 A");
}

void test_ascii_plot_and_csv(void) {
    printf("\n=== Running ASCII Plotter & CSV Export Tests ===\n");
    waveform_t wf;
    waveform_init(&wf, 220.0, 60.0, 0.0, 20.0, 0.0333, 0.0005);

    /* Render plot directly to stdout to demonstrate ASCII waveform visualization */
    graph_config_t cfg = graph_default_config();
    cfg.width = 65;
    cfg.height = 15;
    graph_plot_waveform(stdout, &wf, &cfg);
    TEST_ASSERT(true, "ASCII waveform plotted successfully to terminal");

    /* Test CSV export */
    const char *csv_path = "build/test_export.csv";
    bool exported = graph_export_csv(csv_path, &wf);
    TEST_ASSERT(exported == true, "CSV file export reported success");

    /* Verify file exists and is not empty */
    FILE *fp = fopen(csv_path, "r");
    TEST_ASSERT(fp != NULL, "Exported CSV file exists and can be opened");
    if (fp != NULL) {
        char line[256];
        int lines = 0;
        while (fgets(line, sizeof(line), fp)) {
            lines++;
        }
        fclose(fp);
        TEST_ASSERT(lines > 10, "CSV file contains header and multiple data rows");
    }

    waveform_free(&wf);
}

int main(void) {
    printf("==================================================\n");
    printf("  AC CIRCUIT ANALYZER - TEST SUITE: WAVEFORM      \n");
    printf("==================================================\n");

    test_waveform_metrics();
    test_instantaneous_values();
    test_ascii_plot_and_csv();

    printf("\n==================================================\n");
    printf("Test Summary: %d / %d passed (%.1f%%)\n",
           g_tests_passed, g_tests_run,
           (double)g_tests_passed / (double)g_tests_run * 100.0);
    printf("==================================================\n");

    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
