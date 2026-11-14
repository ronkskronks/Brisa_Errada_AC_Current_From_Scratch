# Brisa_Errada_AC_Current_From_Scratch

# ⚡ AC Circuit Analyzer

<p align="center">
  <img src="https://img.shields.io/badge/C-11-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C11 Standard" />
  <img src="https://img.shields.io/badge/Dependencies-Zero%20(No%20math.h)-success?style=for-the-badge" alt="Zero Dependencies" />
  <img src="https://img.shields.io/badge/Architecture-Freestanding%20Engine-blueviolet?style=for-the-badge" alt="Freestanding Engine" />
  <img src="https://img.shields.io/badge/Unit%20Tests-129%2F129%20Passed-brightgreen?style=for-the-badge" alt="Unit Tests" />
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20GCC%20%7C%20Clang-orange?style=for-the-badge" alt="Linux GCC Clang" />
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="MIT License" />
</p>

<p align="center">
  <strong>A high-precision Alternating Current (AC) Circuit Analyzer, Mathematical Engine, and Terminal Waveform Visualizer built from scratch in C11.</strong>
</p>

---

## 🌟 Highlights & Philosophy

- **Zero External Math Dependencies**: Written entirely without `<math.h>` or the `-lm` linker flag. Every single mathematical operation is implemented manually from fundamental arithmetic.
- **Freestanding Core**: The computational core (`math_engine.c`) relies only on compiler-provided standard integer and boolean types (`<stdint.h>`, `<stdbool.h>`, `<stddef.h>`), completely isolated from hosted OS libraries.
- **Numerical Methods from Scratch**:
  - **Newton-Raphson Square Root** with scale normalization ($x = m \cdot 4^k$) for quadratic convergence in 4–8 iterations.
  - **Taylor/Maclaurin Series with Range Reduction** for sine and cosine (modulo $2\pi$ and quadrant symmetry reduction to $[-\pi/2, \pi/2]$).
  - **Four-Quadrant Arctangent** (`atan`, `atan2`) using inversion and angle-addition identities.
  - **Horner's Method** for polynomial evaluations.
  - **Binary Exponentiation** ($O(\log_2 n)$ power function).
  - **Full Complex Number Arithmetic** for AC phasor and impedance calculations.
- **Terminal ASCII Oscilloscope**: Plots continuous AC voltage and current waveforms directly inside your shell without GUI or plotting libraries.
- **CSV Data Exporter**: Exports discrete $(t, v(t), i(t))$ samples for further processing or spreadsheet plotting.
- **Exhaustive Automated Test Suite**: 129 automated unit tests verifying double-precision mathematical accuracy ($< 10^{-15}$) and circuit theory laws.

---

## 📸 Terminal Waveform Visualizer Preview

```text
--- AC Waveform Plot (Peak: 220.0 V, Freq: 60.0 Hz, Duration: 0.0333 s) ---

   220.0 V |       ***                             ***                       
         |     **   **                         **   **                     
         |    *       *                       *       *                    
         |   *         *                     *         *                   
         |  *           *                   *           *                  
         |  /            \                  /            \                 
         | *             *                 *             *                 
     0.0 V |*---------------*---------------*---------------*---------------*
         |                 *             *                 *             * 
         |                  \            /                  \            / 
         |                  *           *                   *           *  
         |                   *         *                     *         *   
         |                    *       *                       *       *    
         |                     **   **                         **   **     
  -220.0 V |                       ***                             ***       
         +-----------------------------------------------------------------> Time
         0.0s                            0.0167s                         0.0333s
```

---

## 📑 Table of Contents

- [Highlights & Philosophy](#-highlights--philosophy)
- [Terminal Waveform Visualizer Preview](#-terminal-waveform-visualizer-preview)
- [Quick Start](#-quick-start)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Running the Application](#running-the-application)
  - [Running Unit Tests](#running-unit-tests)
- [Interactive Features](#-interactive-features)
- [Mathematical Engine & Algorithms](#-mathematical-engine--algorithms)
- [Electrical Engineering Theory](#-electrical-engineering-theory)
- [Project Architecture](#-project-architecture)
- [Contributing & License](#-contributing--license)

---

## 🚀 Quick Start

### Prerequisites

- A Linux environment (Ubuntu, Debian, Fedora, Arch, WSL2, etc.)
- Standard C11 compiler (`gcc` >= 4.9 or `clang` >= 3.5)
- GNU `make`

### Compilation

Compile the entire project and run all 129 unit tests in one command:

```bash
make all
```

*Notice that the Makefile compiles strictly with `-std=c11 -Wall -Wextra -Wpedantic -O2` with **no `-lm` flag**.*

### Running the Application

```bash
./ac_analyzer
# or via make:
make run
```

### Running Unit Tests

```bash
make test               # Runs all test suites
make test_math          # Mathematical engine unit tests (71 tests)
make test_electrical    # Circuit calculations & RLC tests (41 tests)
make test_waveform      # Waveform generator & plotting tests (17 tests)
```

---

## 🎛️ Interactive Features

Upon launching `./ac_analyzer`, you are presented with an interactive command-line interface:

```text
==================================================
           AC CIRCUIT ANALYZER (C11)              
==================================================
  1. AC waveform generator
  2. Ohm's Law calculator
  3. Electrical power calculator
  4. Capacitive reactance calculator
  5. Inductive reactance calculator
  6. Series RLC circuit analyzer
  7. AC waveform plotter
  8. Mathematical functions test
  9. Exit
==================================================
```

1. **AC Waveform Generator**: Configure peak voltage, frequency, phase shift, resistive load, duration, and time step $dt$. Calculates peak, peak-to-peak, RMS values, period, angular frequency, and evaluates instantaneous $v(t)$ and $i(t)$. Includes terminal ASCII plotting and CSV export.
2. **Ohm's Law Calculator**: Solves for $V$, $I$, or $R$ with input validation and zero-division protection.
3. **Electrical Power Calculator**: Calculates Active Power ($P$), Apparent Power ($S$), Reactive Power ($Q$), and Power Factor ($\text{PF} = \cos\theta$), identifying inductive vs. capacitive loading.
4. **Capacitive Reactance Calculator**: Computes $X_C = \frac{1}{2\pi f C}$ and complex impedance $Z_C = -j X_C$.
5. **Inductive Reactance Calculator**: Computes $X_L = 2\pi f L$ and complex impedance $Z_L = +j X_L$.
6. **Series RLC Circuit Analyzer**: Performs full impedance phasor analysis ($\mathbf{Z} = R + j(X_L - X_C)$), scalar magnitude $|Z|$, loop RMS current, phase angle, resonance frequency $f_0$, Quality Factor $Q$, bandwidth $BW$, and individual voltage drops ($V_R, V_L, V_C$).
7. **AC Waveform ASCII Plotter**: Interactive oscilloscope renderer allowing custom width, height, scale, and cycle count.
8. **Mathematical Functions Test**: Live benchmark and verification of the underlying custom numerical methods.
9. **Exit**: Graceful termination.

---

## 🧮 Mathematical Engine & Algorithms

All algorithms are implemented in [`src/math_engine.c`](src/math_engine.c) without `<math.h>`:

### 1. Square Root via Newton-Raphson (`math_sqrt`)
To compute $y = \sqrt{x}$, we find the positive zero of $f(y) = y^2 - x = 0$:
$$y_{k+1} = \frac{1}{2}\left(y_k + \frac{x}{y_k}\right)$$
- **Scale Normalization**: To prevent divergence or slow convergence with very large or very small numbers, $x$ is normalized into $m \in [0.25, 4.0)$ such that $x = m \cdot 4^k$. An initial estimate $y_0 = 0.5(1 + m)$ guarantees quadratic convergence to double precision ($10^{-16}$) in only 4–8 iterations. The final result is rescaled by $2^k$.

### 2. Trigonometric Sine & Cosine (`math_sin`, `math_cos`)
- **Range Reduction**: For large arguments $2\pi f t$, evaluation of Taylor polynomials directly causes catastrophic cancellation.
  1. $x$ is reduced modulo $2\pi$ to $[-\pi, \pi]$ using custom `math_fmod`.
  2. Quadrant symmetry maps $x$ strictly into $[-\pi/2, \pi/2]$:
     $$\text{if } x > \frac{\pi}{2}: \sin(x) = \sin(\pi - x), \quad \text{if } x < -\frac{\pi}{2}: \sin(x) = -\sin(\pi + x)$$
- **Recurrence Formulation**:
  $$\sin(x) = \sum_{n=0}^{\infty} (-1)^n \frac{x^{2n+1}}{(2n+1)!}, \quad T_n = -T_{n-1} \cdot \frac{x^2}{2n(2n+1)}$$
  Recurrence term computation avoids computing huge powers or large factorials, maintaining full 64-bit IEEE-754 precision.

### 3. Four-Quadrant Arctangent (`math_atan`, `math_atan2`)
- **Argument Reduction**:
  1. If $x > 1$: $\arctan(x) = \frac{\pi}{2} - \arctan(1/x)$.
  2. If $x > \sqrt{2} - 1 \approx 0.4142$: $\arctan(x) = \frac{\pi}{4} + \arctan\left(\frac{x - 1}{x + 1}\right)$.
- Confines the Maclaurin argument $u$ to $[-0.4142, 0.4142]$, reaching convergence in 12–15 terms.

### 4. Polynomial Evaluation via Horner's Method (`math_taylor_eval`)
$$P(x) = c_0 + (x - x_0)(c_1 + (x - x_0)(c_2 + \dots))$$
Requires only $n-1$ multiplications and additions, preventing numerical instability.

### 5. Complex Number Arithmetic (`complex_t`)
Custom rectangular structure:
```c
typedef struct {
    double real; /* Resistance / Real component */
    double imag; /* Reactance / Imaginary component */
} complex_t;
```
Supports addition, subtraction, multiplication, complex division, magnitude, and phase calculation.

---

## ⚡ Electrical Engineering Theory

### 1. Ohm's Law & Power Triangle
- **Instantaneous AC Law**: $v(t) = i(t) \cdot R$
- **RMS Power Components**:
  - **Active Power ($P$)**: $P = V_{\text{rms}} I_{\text{rms}} \cos(\theta)$ [Watts, W]
  - **Apparent Power ($S$)**: $S = V_{\text{rms}} I_{\text{rms}}$ [Volt-Amperes, VA]
  - **Reactive Power ($Q$)**: $Q = V_{\text{rms}} I_{\text{rms}} \sin(\theta)$ [Volt-Amperes Reactive, VAR]
  - **Power Factor ($\text{PF}$)**: $\text{PF} = \frac{P}{S} = \cos(\theta), \quad S = \sqrt{P^2 + Q^2}$

### 2. Reactance & Impedance
- **Inductive Reactance**: $X_L = 2\pi f L = \omega L \quad [\Omega]$. (Voltage leads current by $90^\circ$).
- **Capacitive Reactance**: $X_C = \frac{1}{2\pi f C} = \frac{1}{\omega C} \quad [\Omega]$. (Current leads voltage by $90^\circ$).

### 3. Series RLC Circuit Analysis
- **Total Complex Impedance**: $\mathbf{Z} = R + j(X_L - X_C) \quad [\Omega]$
- **Scalar Impedance Magnitude**: $|Z| = \sqrt{R^2 + (X_L - X_C)^2} \quad [\Omega]$
- **Series RMS Current**: $I_{\text{rms}} = \frac{V_{\text{rms}}}{|Z|} \quad [\text{A}]$
- **Impedance Phase Angle**: $\theta = \arctan\left(\frac{X_L - X_C}{R}\right)$
- **Resonant Frequency ($f_0$)**: When $X_L = X_C$, net reactance is zero and impedance is minimum ($|Z| = R$):
  $$f_0 = \frac{1}{2\pi \sqrt{L \cdot C}} \quad [\text{Hz}]$$
- **Quality Factor ($Q$) & Bandwidth ($BW$)**:
  $$Q = \frac{\omega_0 L}{R} = \frac{1}{R} \sqrt{\frac{L}{C}}, \quad BW = \frac{f_0}{Q} = \frac{R}{2\pi L} \quad [\text{Hz}]$$
- **Component Voltages (Phasor Conservation)**:
  $$V_R = I \cdot R, \quad V_L = I \cdot X_L, \quad V_C = I \cdot X_C, \quad V_{\text{source}} = \sqrt{V_R^2 + (V_L - V_C)^2}$$

---

## 📂 Project Architecture

```
.
├── Makefile                     # Build system with strict C11 flags (No -lm)
├── README.md                    # Project documentation
├── include/
│   ├── math_engine.h            # Freestanding mathematical engine declarations
│   ├── electrical.h             # Ohm's law, power, reactance & RLC declarations
│   ├── waveform.h               # Waveform generator and sample buffer definitions
│   └── graph.h                  # Terminal ASCII plotter and CSV exporter headers
├── src/
│   ├── math_engine.c            # Custom numerical engine implementation
│   ├── electrical.c             # Electrical equations & RLC circuit logic
│   ├── waveform.c               # AC signal generation & sampling routines
│   ├── graph.c                  # Terminal ASCII grid renderer & CSV writer
│   └── main.c                   # Interactive CLI menu application
└── tests/
    ├── test_math.c              # 71 Unit tests for mathematical engine
    ├── test_electrical.c        # 41 Unit tests for electrical calculation models
    └── test_waveform.c          # 17 Unit tests for waveform generation & plotting
```

---

## 🧪 Test Results Summary

```text
==================================================
Unit Test Suites Execution Report
==================================================
  [PASS] test_math.c       : 71 / 71 tests passed (100.0%)
  [PASS] test_electrical.c : 41 / 41 tests passed (100.0%)
  [PASS] test_waveform.c   : 17 / 17 tests passed (100.0%)
--------------------------------------------------
  TOTAL                    : 129 / 129 tests passed (100.0%)
==================================================
```

---

## 📜 License

This project is licensed under the **MIT License**. Free for educational, academic, and commercial use.
