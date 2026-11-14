# AC Circuit Analyzer (C11)

An electrical engineering software tool and numerical engine written in standard C11 for AC circuit analysis, waveform generation, and terminal ASCII visualization.

Implemented from scratch without external mathematical libraries, without `<math.h>`, and without `-lm`.

---

## 1. Architectural Philosophy & Freestanding Implementation

To reinforce core systems programming and electrical engineering numerical methods:
1. **Zero External Math Dependencies**: No `<math.h>` header is included in the calculations. Compilation does not link `-lm`.
2. **Freestanding Core**: The numerical engine (`src/math_engine.c`) only uses compiler-provided freestanding definitions:
   - `<stdint.h>`: for fixed-width integers (`int64_t`).
   - `<stdbool.h>`: for standard boolean types (`bool`, `true`, `false`).
   - `<stddef.h>`: for `size_t` and `NULL`.
3. **Hosted Runtime Isolation**: Standard I/O (`<stdio.h>`) and memory allocations (`<stdlib.h>`) are isolated strictly to interactive CLI handling, ASCII terminal rendering, and CSV file exports.

---

## 2. Project Structure

```
.
├── Makefile                     # C11 build system (no -lm flag)
├── README.md                    # Comprehensive documentation and technical guide
├── YOUR_TASK                    # Project specification
├── ac_analyzer                  # Compiled executable
├── include/
│   ├── math_engine.h            # Freestanding mathematical library declarations
│   ├── electrical.h             # Ohm's law, power, reactance, and series RLC
│   ├── waveform.h               # AC waveform generator and metrics
│   └── graph.h                  # ASCII graph plotter and CSV exporter
├── src/
│   ├── math_engine.c            # Custom numerical algorithms (Newton, Taylor, Horner)
│   ├── electrical.c             # Electrical calculation models
│   ├── waveform.c               # AC signal generation and sampling
│   ├── graph.c                  # Terminal ASCII grid renderer
│   └── main.c                   # Interactive CLI menu
├── tests/
│   ├── test_math.c              # 71 Unit tests for math engine
│   ├── test_electrical.c        # 41 Unit tests for electrical modules
│   └── test_waveform.c          # 17 Unit tests for waveform & plotting
└── build/                       # Object files and test binaries
```

---

## 3. Mathematical Engine: Numerical Methods

### A. Square Root (`math_sqrt`) via Newton-Raphson
* **Equation**: Finds the root of $f(y) = y^2 - x = 0$:
  $$y_{k+1} = \frac{1}{2} \left(y_k + \frac{x}{y_k}\right)$$
* **Scale Normalization**: To ensure quadratic convergence across all scales (from $10^{-12}$ to $10^{12}$), $x$ is normalized into $m \in [0.25, 4.0)$ such that $x = m \cdot 4^k$. An initial linear estimate $y_0 = 0.5(1 + m)$ converges to machine double precision in 4–8 iterations. The final result is rescaled by $2^k$.

### B. Sine and Cosine (`math_sin`, `math_cos`) via Range-Reduced Taylor Series
* **Domain Reduction**:
  1. Argument $x$ is reduced modulo $2\pi$ into $[-\pi, \pi]$ using custom `math_fmod`.
  2. Quadrant symmetry reductions map $x$ to $[-\pi/2, \pi/2]$:
     $$\text{If } x > \frac{\pi}{2}: \sin(x) = \sin(\pi - x)$$
     $$\text{If } x < -\frac{\pi}{2}: \sin(x) = -\sin(\pi + x)$$
* **Recurrence Series**:
  $$\sin(x) = \sum_{n=0}^{\infty} (-1)^n \frac{x^{2n+1}}{(2n+1)!}, \quad T_n = -T_{n-1} \frac{x^2}{2n(2n+1)}$$
  Iterative recurrence avoids integer overflow in factorials and yields double-precision accuracy in $\le 10$ terms.

### C. Four-Quadrant Arc Tangent (`math_atan`, `math_atan2`)
* Evaluates $\arctan(x)$ for phase angle calculation:
  1. Inversion identity for $x > 1$: $\arctan(x) = \frac{\pi}{2} - \arctan(1/x)$.
  2. Angle addition identity for $x > \sqrt{2}-1 \approx 0.4142$:
     $$\arctan(x) = \frac{\pi}{4} + \arctan\left(\frac{x - 1}{x + 1}\right)$$
  3. Reduced argument $|u| \le 0.4142$ guarantees rapid convergence of the Maclaurin series $\sum_{k=0}^{\infty} (-1)^k \frac{u^{2k+1}}{2k+1}$.

### D. Binary Exponentiation (`math_pow_int`)
* Square-and-multiply algorithm executing in $O(\log_2 |n|)$ operations.

### E. Polynomial Evaluation (`math_taylor_eval`)
* Implements Horner's rule:
  $$P(x) = c_0 + (x - x_0)(c_1 + (x - x_0)(c_2 + \dots))$$
  Minimizes round-off error using only $n-1$ additions and multiplications.

### F. Complex Number Arithmetic (`complex_t`)
* Rectangular representation: $Z = R + jX$.
* Implements addition, subtraction, multiplication, division, magnitude ($\sqrt{R^2 + X^2}$), and phase angle ($\arctan(X / R)$).

---

## 4. Electrical Engineering Calculations

### A. Ohm's Law
* Instantaneous and DC relationships:
  $$V = I \cdot R, \quad I = \frac{V}{R}, \quad R = \frac{V}{I}$$

### B. AC Electrical Power & Power Triangle
* **Active Power (P)**: Real energy consumed by resistive elements:
  $$P = V_{\text{rms}} I_{\text{rms}} \cos(\theta) \quad [\text{Watts, W}]$$
* **Apparent Power (S)**: Total electrical capacity supplied:
  $$S = V_{\text{rms}} I_{\text{rms}} \quad [\text{Volt-Amperes, VA}]$$
* **Reactive Power (Q)**: Energy oscillating between source and fields:
  $$Q = V_{\text{rms}} I_{\text{rms}} \sin(\theta) \quad [\text{Volt-Amperes Reactive, VAR}]$$
* **Power Factor (PF)**:
  $$\text{PF} = \cos(\theta) = \frac{P}{S}, \quad S = \sqrt{P^2 + Q^2}$$

### C. Reactance
* **Inductive Reactance ($X_L$)**: Opposition to alternating current due to electromagnetic induction. Current lags voltage by $90^\circ$:
  $$X_L = 2\pi f L = \omega L \quad [\Omega]$$
* **Capacitive Reactance ($X_C$)**: Opposition to alternating current due to electric field charging. Current leads voltage by $90^\circ$:
  $$X_C = \frac{1}{2\pi f C} = \frac{1}{\omega C} \quad [\Omega]$$

### D. Series RLC Circuit Analysis
* **Complex Impedance**:
  $$\mathbf{Z} = R + j(X_L - X_C) \quad [\Omega]$$
* **Impedance Magnitude**:
  $$|Z| = \sqrt{R^2 + (X_L - X_C)^2} \quad [\Omega]$$
* **Phase Angle ($\theta$)**:
  $$\theta = \arctan\left(\frac{X_L - X_C}{R}\right)$$
* **Resonance Frequency ($f_0$)**: Frequency where $X_L = X_C$, net reactance is zero, and impedance is purely resistive ($|Z| = R$):
  $$f_0 = \frac{1}{2\pi \sqrt{LC}} \quad [\text{Hz}]$$
* **Quality Factor ($Q$) & Bandwidth ($BW$)**:
  $$Q = \frac{\omega_0 L}{R} = \frac{1}{R} \sqrt{\frac{L}{C}}, \quad BW = \frac{f_0}{Q} = \frac{R}{2\pi L} \quad [\text{Hz}]$$

---

## 5. Waveform Generation & ASCII Terminal Visualization

* Generator equation:
  $$v(t) = V_{\text{peak}} \sin(2\pi f t + \phi)$$
* Terminal ASCII graph maps time $[0, t_{\text{duration}}]$ to columns and voltage $[-V_{\text{scale}}, +V_{\text{scale}}]$ to rows, automatically interpolating slopes for smooth, unbroken sinusoidal curves.
* Export feature saves discrete $(t, v(t), i(t))$ tuples to CSV.

---

## 6. Build and Test Instructions

```bash
# Compile everything and run complete test suite (129 unit tests)
make all

# Run specific unit test suites:
make test_math          # Mathematical engine tests (71 tests)
make test_electrical    # Electrical & RLC calculation tests (41 tests)
make test_waveform      # Waveform & plotting tests (17 tests)

# Launch the interactive application:
./ac_analyzer
# or:
make run

# Clean build artifacts:
make clean
```
