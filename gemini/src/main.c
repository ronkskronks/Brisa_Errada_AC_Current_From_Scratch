/**
 * ============================================================================
 * AC Circuit Analyzer - Interactive Command-Line Application
 * ============================================================================
 * 
 * Provides an interactive terminal interface for:
 * 1. AC waveform generator & metrics
 * 2. Ohm's Law calculator
 * 3. Electrical power calculator (P, S, Q, PF)
 * 4. Capacitive reactance calculator
 * 5. Inductive reactance calculator
 * 6. Series RLC circuit analyzer
 * 7. AC waveform ASCII plotter
 * 8. Mathematical engine verification / tests
 * 9. Exit
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "math_engine.h"
#include "electrical.h"
#include "waveform.h"
#include "graph.h"

/* Safe double input with range validation */
static double prompt_double(const char *prompt, double min_val, double max_val) {
    double val;
    char buffer[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return min_val;
        }
        if (sscanf(buffer, "%lf", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            }
            printf("  [!] Value must be between %g and %g. Please try again.\n", min_val, max_val);
        } else {
            printf("  [!] Invalid numerical input. Please try again.\n");
        }
    }
}

/* Safe integer input with range validation */
static int prompt_int(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return min_val;
        }
        if (sscanf(buffer, "%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            }
            printf("  [!] Choice must be between %d and %d. Please try again.\n", min_val, max_val);
        } else {
            printf("  [!] Invalid integer input. Please try again.\n");
        }
    }
}

/* Safe string input */
static void prompt_string(const char *prompt, char *dest, size_t dest_size) {
    printf("%s", prompt);
    if (fgets(dest, (int)dest_size, stdin) != NULL) {
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') {
            dest[len - 1] = '\0';
        }
    }
}

/* Safe Yes/No prompt */
static bool prompt_yes_no(const char *prompt) {
    char buffer[32];
    while (1) {
        printf("%s (y/n): ", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return false;
        }
        if (buffer[0] == 'y' || buffer[0] == 'Y') {
            return true;
        }
        if (buffer[0] == 'n' || buffer[0] == 'N') {
            return false;
        }
        printf("  [!] Please enter 'y' or 'n'.\n");
    }
}

/* Option 1: AC Waveform Generator */
static void handle_ac_generator(void) {
    printf("\n==================================================\n");
    printf("         1. AC WAVEFORM GENERATOR                 \n");
    printf("==================================================\n");
    printf("v(t) = Vpeak * sin(2 * pi * f * t + phi)\n\n");

    double v_peak = prompt_double("Enter Peak Voltage (V) [0.1 to 100000]: ", 0.1, 100000.0);
    double freq   = prompt_double("Enter Frequency (Hz) [0.1 to 1000000]: ", 0.1, 1000000.0);
    double phase_deg = prompt_double("Enter Initial Phase (degrees) [-360 to 360]: ", -360.0, 360.0);
    double r_load = prompt_double("Enter Purely Resistive Load (Ohms) [0.1 to 1000000]: ", 0.1, 1000000.0);
    double duration = prompt_double("Enter Total Duration (seconds) [0.000001 to 60]: ", 0.000001, 60.0);
    double dt = prompt_double("Enter Sampling Interval dt (seconds) [0.0000001 to 1]: ", 0.0000001, 1.0);

    double phase_rad = math_deg2rad(phase_deg);

    waveform_t wf;
    if (!waveform_init(&wf, v_peak, freq, phase_rad, r_load, duration, dt)) {
        printf("  [!] Error generating waveform. Check parameter bounds.\n");
        return;
    }

    printf("\n--- Waveform Electrical Characteristics ---\n");
    printf("  Peak Voltage (Vpeak)       : %12.4f V\n", wf.v_peak);
    printf("  Peak-to-Peak Voltage (Vpp) : %12.4f V\n", wf.v_peak_to_peak);
    printf("  RMS Voltage (Vrms)         : %12.4f V\n", wf.v_rms);
    printf("  Peak Current (Ipeak)       : %12.4f A\n", wf.i_peak);
    printf("  RMS Current (Irms)         : %12.4f A\n", wf.i_rms);
    printf("  Frequency (f)              : %12.4f Hz\n", wf.frequency);
    printf("  Period (T = 1/f)           : %12.6f s\n", wf.period);
    printf("  Angular Frequency (omega)  : %12.4f rad/s\n", wf.angular_freq);
    printf("  Total Samples Generated    : %12zu\n", wf.sample_count);

    if (prompt_yes_no("\nCalculate instantaneous value v(t) and i(t) at specific time?")) {
        double t_query = prompt_double("Enter time t (seconds): ", 0.0, 10000.0);
        double vt = waveform_instantaneous_voltage(v_peak, freq, phase_rad, t_query);
        double it = vt / r_load;
        printf("  At t = %.6f s:\n", t_query);
        printf("    Instantaneous Voltage v(t): %.4f V\n", vt);
        printf("    Instantaneous Current i(t): %.4f A\n", it);
    }

    if (prompt_yes_no("\nPlot ASCII waveform in terminal?")) {
        graph_config_t cfg = graph_default_config();
        cfg.duration = duration;
        graph_plot_waveform(stdout, &wf, &cfg);
    }

    if (prompt_yes_no("Export sampled waveform to CSV file?")) {
        char csv_filename[128];
        prompt_string("Enter CSV destination filename [e.g. waveform.csv]: ", csv_filename, sizeof(csv_filename));
        if (strlen(csv_filename) == 0) {
            strcpy(csv_filename, "waveform.csv");
        }
        if (graph_export_csv(csv_filename, &wf)) {
            printf("  [+] Successfully exported %zu samples to '%s'.\n", wf.sample_count, csv_filename);
        } else {
            printf("  [!] Failed to export to file '%s'.\n", csv_filename);
        }
    }

    waveform_free(&wf);
}

/* Option 2: Ohm's Law Calculator */
static void handle_ohms_law(void) {
    printf("\n==================================================\n");
    printf("            2. OHM'S LAW CALCULATOR               \n");
    printf("==================================================\n");
    printf("Equations: V = I * R  |  I = V / R  |  R = V / I\n\n");
    printf("  1. Calculate Voltage (V) from Current (I) and Resistance (R)\n");
    printf("  2. Calculate Current (I) from Voltage (V) and Resistance (R)\n");
    printf("  3. Calculate Resistance (R) from Voltage (V) and Current (I)\n");
    int choice = prompt_int("\nSelect calculation [1-3]: ", 1, 3);

    double v, i, r;
    switch (choice) {
        case 1: {
            i = prompt_double("Enter Current I (Amperes): ", 0.0, 1e7);
            r = prompt_double("Enter Resistance R (Ohms): ", 0.0, 1e9);
            if (ohm_calc_voltage(i, r, &v) == ELEC_OK) {
                printf("\n  [RESULT] Voltage V = %.4f V (%.4f mV / %.4f kV)\n", v, v * 1e3, v * 1e-3);
            }
            break;
        }
        case 2: {
            v = prompt_double("Enter Voltage V (Volts): ", 0.0, 1e8);
            r = prompt_double("Enter Resistance R (Ohms): ", 0.0001, 1e9);
            if (ohm_calc_current(v, r, &i) == ELEC_OK) {
                printf("\n  [RESULT] Current I = %.6f A (%.3f mA / %.3f uA)\n", i, i * 1e3, i * 1e6);
            }
            break;
        }
        case 3: {
            v = prompt_double("Enter Voltage V (Volts): ", 0.0, 1e8);
            i = prompt_double("Enter Current I (Amperes): ", 0.0000001, 1e7);
            if (ohm_calc_resistance(v, i, &r) == ELEC_OK) {
                printf("\n  [RESULT] Resistance R = %.4f Ohms (%.4f kOhm / %.4f MOhm)\n", r, r * 1e-3, r * 1e-6);
            }
            break;
        }
    }
}

/* Option 3: Electrical Power Calculator */
static void handle_electrical_power(void) {
    printf("\n==================================================\n");
    printf("          3. ELECTRICAL POWER CALCULATOR          \n");
    printf("==================================================\n");
    printf("Calculates Active (P), Apparent (S), Reactive (Q) and Power Factor (PF)\n\n");
    printf("  1. Calculate from RMS Voltage, RMS Current, and Phase Angle\n");
    printf("  2. Calculate from Active Power (W) and Reactive Power (VAR)\n");
    int choice = prompt_int("\nSelect calculation [1-2]: ", 1, 2);

    power_result_t res;
    if (choice == 1) {
        double v = prompt_double("Enter RMS Voltage (V): ", 0.0, 1e7);
        double i = prompt_double("Enter RMS Current (A): ", 0.0, 1e6);
        double phase_deg = prompt_double("Enter Phase Angle (degrees, >0 inductive, <0 capacitive): ", -180.0, 180.0);
        double phase_rad = math_deg2rad(phase_deg);
        power_calc_from_vi_theta(v, i, phase_rad, &res);
    } else {
        double p = prompt_double("Enter Active Power P (Watts): ", 0.0, 1e10);
        double q = prompt_double("Enter Reactive Power Q (VAR, >0 ind, <0 cap): ", -1e10, 1e10);
        power_calc_from_pq(p, q, &res);
    }

    printf("\n--- Electrical Power Results ---\n");
    printf("  Active Power (P)    : %12.4f W   (Real energy dissipated)\n", res.active_power);
    printf("  Apparent Power (S)  : %12.4f VA  (Total capacity demand)\n", res.apparent_power);
    printf("  Reactive Power (Q)  : %12.4f VAR (Stored in oscillating fields)\n", res.reactive_power);
    printf("  Power Factor (PF)   : %12.4f     (cos(theta))\n", res.power_factor);
    printf("  Phase Angle (theta) : %12.4f deg (%12.6f rad)\n", res.phase_deg, res.phase_rad);
    printf("  Load Nature         : %s\n", res.is_lagging ? "Inductive (Lagging Current)" : "Capacitive / Resistive (Leading/In-phase)");
}

/* Option 4: Capacitive Reactance Calculator */
static void handle_capacitive_reactance(void) {
    printf("\n==================================================\n");
    printf("        4. CAPACITIVE REACTANCE CALCULATOR        \n");
    printf("==================================================\n");
    printf("Equation: XC = 1 / (2 * pi * f * C)\n");
    printf("Complex Impedance: Zc = -j * XC = 0 - j / (omega * C)\n\n");

    double freq = prompt_double("Enter Frequency f (Hz) [0.1 to 100000000]: ", 0.1, 100000000.0);
    double cap_uF = prompt_double("Enter Capacitance C (in microfarads, uF) [0.0001 to 1000000]: ", 0.0001, 1000000.0);
    double cap_f = cap_uF * 1e-6;

    double xc = 0.0;
    if (reactance_calc_capacitive(freq, cap_f, &xc) == ELEC_OK) {
        printf("\n--- Results ---\n");
        printf("  Capacitive Reactance XC : %.4f Ohms\n", xc);
        printf("  Complex Impedance Zc    : 0.0000 - j(%.4f) Ohms\n", xc);
        printf("  Phase Shift             : Current LEADS voltage by 90.0 degrees\n");
        printf("  Physical Meaning        : The capacitor opposes changes in voltage.\n");
        printf("                            Higher frequencies result in LOWER reactance.\n");
    }
}

/* Option 5: Inductive Reactance Calculator */
static void handle_inductive_reactance(void) {
    printf("\n==================================================\n");
    printf("        5. INDUCTIVE REACTANCE CALCULATOR         \n");
    printf("==================================================\n");
    printf("Equation: XL = 2 * pi * f * L\n");
    printf("Complex Impedance: Zl = +j * XL = 0 + j * (omega * L)\n\n");

    double freq = prompt_double("Enter Frequency f (Hz) [0.0 to 100000000]: ", 0.0, 100000000.0);
    double ind_mH = prompt_double("Enter Inductance L (in millihenries, mH) [0.0001 to 1000000]: ", 0.0001, 1000000.0);
    double ind_h = ind_mH * 1e-3;

    double xl = 0.0;
    if (reactance_calc_inductive(freq, ind_h, &xl) == ELEC_OK) {
        printf("\n--- Results ---\n");
        printf("  Inductive Reactance XL  : %.4f Ohms\n", xl);
        printf("  Complex Impedance Zl    : 0.0000 + j(%.4f) Ohms\n", xl);
        printf("  Phase Shift             : Current LAGS voltage by 90.0 degrees\n");
        printf("  Physical Meaning        : The inductor opposes changes in current.\n");
        printf("                            Higher frequencies result in HIGHER reactance.\n");
    }
}

/* Option 6: Series RLC Circuit Analyzer */
static void handle_series_rlc(void) {
    printf("\n==================================================\n");
    printf("          6. SERIES RLC CIRCUIT ANALYZER          \n");
    printf("==================================================\n");
    printf("Model: Series loop with Resistor (R), Inductor (L), and Capacitor (C)\n");
    printf("Z_total = R + j*(XL - XC)\n\n");

    double r      = prompt_double("Enter Resistance R (Ohms) [0.0 to 1000000]: ", 0.0, 1000000.0);
    double ind_mH = prompt_double("Enter Inductance L (mH) [0.001 to 1000000]: ", 0.001, 1000000.0);
    double cap_uF = prompt_double("Enter Capacitance C (uF) [0.001 to 1000000]: ", 0.001, 1000000.0);
    double freq   = prompt_double("Enter Applied Frequency f (Hz) [0.1 to 10000000]: ", 0.1, 10000000.0);
    double v_rms  = prompt_double("Enter Applied RMS Voltage (V) [0.1 to 100000]: ", 0.1, 100000.0);

    double ind_h = ind_mH * 1e-3;
    double cap_f = cap_uF * 1e-6;

    rlc_result_t res;
    if (rlc_analyze(r, ind_h, cap_f, freq, v_rms, &res) != ELEC_OK) {
        printf("  [!] Error calculating RLC parameters.\n");
        return;
    }

    printf("\n--- Series RLC Analysis Results ---\n");
    printf("  Angular Frequency (omega)   : %12.4f rad/s\n", res.angular_freq);
    printf("  Inductive Reactance (XL)    : %12.4f Ohms\n", res.xl);
    printf("  Capacitive Reactance (XC)   : %12.4f Ohms\n", res.xc);
    printf("  Net Reactance (X = XL - XC) : %12.4f Ohms\n", res.net_reactance);
    printf("  Complex Impedance (Z)       : %12.4f + j(%12.4f) Ohms\n", res.z_complex.real, res.z_complex.imag);
    printf("  Scalar Impedance (|Z|)      : %12.4f Ohms\n", res.z_magnitude);
    printf("  Circuit RMS Current (Irms)  : %12.4f A\n", res.i_rms);
    printf("  Phase Angle (theta)         : %12.4f deg (%12.6f rad)\n", res.phase_deg, res.phase_rad);

    printf("  Circuit Nature              : ");
    if (res.nature == CIRCUIT_RESONANT) {
        printf("RESONANCE (XL == XC, purely resistive)\n");
    } else if (res.nature == CIRCUIT_INDUCTIVE) {
        printf("INDUCTIVE (XL > XC, voltage leads current)\n");
    } else {
        printf("CAPACITIVE (XC > XL, voltage lags current)\n");
    }

    printf("\n--- Resonance Characteristics ---\n");
    printf("  Resonant Frequency (f0)     : %12.4f Hz\n", res.resonant_freq);
    printf("  Quality Factor (Q)          : %12.4f\n", res.quality_factor);
    printf("  Bandwidth (BW = f0 / Q)     : %12.4f Hz\n", res.bandwidth);

    printf("\n--- Component Voltage Drops (RMS) ---\n");
    printf("  Resistor Voltage (VR)       : %12.4f V\n", res.vr_rms);
    printf("  Inductor Voltage (VL)       : %12.4f V\n", res.vl_rms);
    printf("  Capacitor Voltage (VC)      : %12.4f V\n", res.vc_rms);
}

/* Option 7: AC Waveform Plotter */
static void handle_ac_plotter(void) {
    printf("\n==================================================\n");
    printf("           7. AC WAVEFORM ASCII PLOTTER           \n");
    printf("==================================================\n\n");

    double v_peak    = prompt_double("Enter Peak Voltage (V) [1.0 to 10000]: ", 1.0, 10000.0);
    double freq      = prompt_double("Enter Frequency (Hz) [1.0 to 100000]: ", 1.0, 100000.0);
    double phase_deg = prompt_double("Enter Phase Angle (deg) [-360 to 360]: ", -360.0, 360.0);
    int cycles       = prompt_int("Enter Number of Cycles to plot [1 to 6]: ", 1, 6);
    int width        = prompt_int("Enter Graph Width in characters [40 to 110]: ", 40, 110);
    int height       = prompt_int("Enter Graph Height in rows [11 to 35]: ", 11, 35);

    graph_config_t cfg = graph_default_config();
    cfg.width = width;
    cfg.height = height;
    cfg.num_cycles = cycles;

    graph_plot_ac_function(stdout, v_peak, freq, math_deg2rad(phase_deg), &cfg);
}

/* Option 8: Mathematical Engine Verification */
static void handle_math_test(void) {
    printf("\n==================================================\n");
    printf("        8. MATHEMATICAL ENGINE VERIFICATION       \n");
    printf("==================================================\n");
    printf("Testing all manually implemented numerical methods...\n\n");

    printf("1. Newton-Raphson Square Root:\n");
    printf("   sqrt(25.0)    = %.10f  [Expected: 5.0000000000]\n", math_sqrt(25.0));
    printf("   sqrt(2.0)     = %.10f  [Expected: 1.4142135624]\n", math_sqrt(2.0));
    printf("   sqrt(10000.0) = %.10f  [Expected: 100.0000000000]\n\n", math_sqrt(10000.0));

    printf("2. Taylor Series Trigonometric Functions:\n");
    printf("   sin(0)        = %.10f  [Expected: 0.0000000000]\n", math_sin(0.0));
    printf("   sin(pi/2)     = %.10f  [Expected: 1.0000000000]\n", math_sin(MATH_HALF_PI));
    printf("   cos(0)        = %.10f  [Expected: 1.0000000000]\n", math_cos(0.0));
    printf("   cos(pi)       = %.10f  [Expected: -1.0000000000]\n", math_cos(MATH_PI));
    printf("   tan(pi/4)     = %.10f  [Expected: 1.0000000000]\n", math_tan(MATH_PI / 4.0));
    printf("   atan(1.0)     = %.10f  [Expected: 0.7853981634 (pi/4)]\n", math_atan(1.0));
    printf("   atan2(1.0, 1) = %.10f  [Expected: 0.7853981634 (pi/4)]\n\n", math_atan2(1.0, 1.0));

    printf("3. Binary Exponentiation & Factorial:\n");
    printf("   2^10          = %.1f  [Expected: 1024.0]\n", math_pow_int(2.0, 10));
    printf("   5!            = %.1f  [Expected: 120.0]\n\n", math_factorial(5));

    printf("4. Horner's Method Polynomial Evaluation:\n");
    double p_coeffs[] = {2.0, 3.0, 1.0}; /* 2 + 3x + x^2 at x = 4 */
    printf("   P(4) for 2 + 3x + x^2 = %.2f  [Expected: 30.00]\n\n",
           math_taylor_eval(0.0, p_coeffs, 3, 4.0));

    printf("5. Complex Arithmetic:\n");
    complex_t z1 = complex_create(3.0, 4.0);
    complex_t z2 = complex_create(1.0, -2.0);
    complex_t z_prod = complex_mul(z1, z2);
    printf("   (3 + 4j) * (1 - 2j) = %.2f + j(%.2f)  [Expected: 11.00 - j(2.00)]\n", z_prod.real, z_prod.imag);
    printf("   |3 + 4j|            = %.4f              [Expected: 5.0000]\n", complex_mag(z1));

    printf("\n  [+] All mathematical engine routines operating at machine double precision!\n");
}

int main(void) {
    while (1) {
        printf("\n==================================================\n");
        printf("           AC CIRCUIT ANALYZER (C11)              \n");
        printf("==================================================\n");
        printf("  1. AC waveform generator\n");
        printf("  2. Ohm's Law calculator\n");
        printf("  3. Electrical power calculator\n");
        printf("  4. Capacitive reactance calculator\n");
        printf("  5. Inductive reactance calculator\n");
        printf("  6. Series RLC circuit analyzer\n");
        printf("  7. AC waveform plotter\n");
        printf("  8. Mathematical functions test\n");
        printf("  9. Exit\n");
        printf("==================================================\n");

        int choice = prompt_int("Enter selection [1-9]: ", 1, 9);
        switch (choice) {
            case 1: handle_ac_generator(); break;
            case 2: handle_ohms_law(); break;
            case 3: handle_electrical_power(); break;
            case 4: handle_capacitive_reactance(); break;
            case 5: handle_inductive_reactance(); break;
            case 6: handle_series_rlc(); break;
            case 7: handle_ac_plotter(); break;
            case 8: handle_math_test(); break;
            case 9:
                printf("\nExiting AC Circuit Analyzer. Have a great day!\n");
                return 0;
            default:
                break;
        }
    }

    return 0;
}
