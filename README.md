# FPGA-Based Image Encryption Using ANN-Driven Lorenz Chaotic System

> A hardware-efficient image encryption framework that replaces the traditional Runge–Kutta (RK4) numerical solver with a trained Artificial Neural Network (ANN) to generate Lorenz chaotic sequences in real time on FPGA.

Companion repository for the B.Tech research paper *"FPGA-Based Image Encryption Using ANN-Driven Lorenz Chaotic System"* — Delhi Technological University, 2026.

---

## 📖 Overview

Chaotic systems are widely used in image cryptography due to their **sensitivity to initial conditions**, **ergodicity**, and **pseudorandom behavior**. The classical Lorenz attractor is usually implemented on FPGA with iterative solvers such as **Runge–Kutta (RK4)**, which are computationally heavy.

This project replaces RK4 with a tiny **3-5-3 feedforward ANN** that learns the Lorenz state transition `(xₜ, yₜ, zₜ) → (xₜ₊₁, yₜ₊₁, zₜ₊₁)`. The trained weights are quantized and baked into a Verilog module, producing chaotic sequences in a **single combinational cycle**. The sequences then drive a two-stage image encryption pipeline:

1. **Confusion** — Arnold Cat Map (ACM) pixel permutation
2. **Diffusion** — XOR with the ANN-derived chaotic key stream

---

## ✨ Key Features

- 🧠 **ANN-Driven Lorenz Generator** — 3-5-3 network trained via Keras Tuner
- ⚡ **Combinational Verilog** — Q?.20 fixed-point, hardcoded weights/biases, no clock
- 🔐 **Two-Stage Encryption** — Arnold Cat Map (10 iterations) + XOR diffusion
- 📉 **Low Hardware Footprint** — ~1604 slice LUTs (~1.19% utilization)
- 🔁 **Verified Lossless Decryption** — `np.array_equal(original, decrypted) == True`
- 📊 **Near-Maximum Entropy** — encrypted RGB entropies ≈ 7.998 (max = 8.0)

---

## 👥 Authors

| Name | Affiliation |
|------|-------------|
| **Dr. Kriti Suneja** *(Supervisor)* | Department of ECE, Delhi Technological University |
| **Kushagra Kishore** | Department of ECE, Delhi Technological University |
| **Ojas Kumar** | Department of ECE, Delhi Technological University |
| **Lakshay Bansal** | Department of ECE, Delhi Technological University |

---

## 📁 Repository Structure

```
Lorentz/
├── Lorentz Verilog code/
│   ├── Lorentz.v             # 3-5-3 ANN inference, fixed-point, combinational
│   └── Lorentz_tb.v          # Testbench: reads +x= +y= +z= plusargs, prints next state
│
├── Test samples/
│   ├── Capybara_encrypted.png
│   ├── Capybara_decrypted.png
│   ├── Snorlax_encrypted.png
│   └── Snorlax_decrypted.png
│
├── Lorentz - 2.ipynb         # RK4 dataset → ANN training (Keras Tuner) → weight export
├── Test Lorentz FPGA.ipynb   # Drives Lorentz_tb.v with iverilog/vvp, compares vs RK4
└── Image Encryption.ipynb    # Full ACM + XOR encryption / decryption pipeline
```

---

## 🏗️ System Architecture

The project follows a **hybrid offline–online design**:

### Offline (Python / Jupyter)
1. Generate Lorenz trajectories with RK4 (Δt = 0.01, **2 × 10,000 steps** from initial conditions `[1,1,1]` and `[4,3,5]`)
2. Train/test split: 17,998 / 2,000 samples; normalize via min-max scaling
3. **Keras Tuner (RandomSearch, 25 trials)** searches a 3 → N → 3 ANN with `relu`/`linear` activations; best architecture is 3-5-3
4. Train with **`nadam`** optimizer, **MSE** loss, 40 epochs
5. Export weights/biases, quantize to fixed-point, paste into `Lorentz.v`

### Online (FPGA / Verilog)
1. `Lorentz` module reads the current state `(x0, x1, x2)` and combinationally outputs `(y0, y1, y2)` — the next state
2. Iterating it produces the chaotic sequence
3. The sequence is mapped to a per-pixel key (see Encryption Math)
4. Image is encrypted by Arnold Cat Map confusion + XOR diffusion

---

## 🔧 Verilog Implementation

The `Lorentz` module is a single-cycle combinational realization of the trained ANN.

| Property | Value |
|---|---|
| Format | 24-bit signed fixed-point, `SHIFT = 20` (Q3.20 layout) |
| Architecture | 3 inputs → 5 ReLU hidden → 3 linear outputs |
| Multiplier | `qmul` helper (48-bit intermediate, arithmetic shift right by 20) |
| Clocking | Combinational only (`assign` statements) |
| Weights / biases | Hardcoded `parameter signed [23:0]` |

The testbench reads `+x`, `+y`, `+z` as Verilog plusargs and prints the resulting next state, so the host notebook can drive the simulator iteratively to build the chaotic sequence.

```bash
# Compile (Icarus Verilog)
iverilog -o Lorentz_TB "Lorentz Verilog code/Lorentz.v" "Lorentz Verilog code/Lorentz_tb.v"

# Run one step (values are Q?.20 fixed-point integers, e.g. 0.1 ≈ 104857)
vvp Lorentz_TB +x=104857 +y=104857 +z=104857
```

---

## 🧮 Encryption Math

**Arnold Cat Map (confusion, 10 iterations):**
```
x' = (x + y)  mod N
y' = (x + 2y) mod N
```

**Inverse Arnold Cat Map (decryption):**
```
x' = (2x − y) mod N
y' = (−x + y) mod N
```

**Chaotic key generation** (`lorenz_key` in `Image Encryption.ipynb`):
1. Build a 3D input array `arr = [values − 10.01, values, values + 10.01]`
2. Run **2 forward steps** through the ANN: `arr = model.predict(arr)` × 2
3. Mean across the 3 components → ×255 → cast to `uint8` → one key byte per pixel

**Diffusion (XOR):**
```
C(i, j) = P(i, j) ⊕ K(i, j)
```

**Decryption** = XOR with the same key, then inverse Arnold Cat Map → lossless recovery.

---

## 📊 Results

### ANN Performance
- Best validation loss from Keras Tuner: **val_loss ≈ 5.40 × 10⁻⁵**
- Best trial val_loss: ≈ 5.42 × 10⁻⁵
- ANN-generated Lorenz trajectory tracks the RK4 reference closely; butterfly attractor structure preserved
- The training notebook also produces RK4-vs-ANN line plots for x(t), y(t), z(t)

### Image Encryption — Capybara (from `Image Encryption.ipynb` outputs)

| Stage | R entropy | G entropy | B entropy |
|---|---:|---:|---:|
| Original | 7.6406 | 7.6189 | 7.6117 |
| **Encrypted** | **7.9987** | **7.9985** | **7.9979** |
| Decrypted | 7.6406 | 7.6189 | 7.6117 |

- Encrypted entropy is **essentially maximum** (max possible = 8.0) → very strong randomness
- `np.array_equal(original, decrypted)` → **`True`**
- `np.abs(original − decrypted).max()` → **`0`** → bit-exact lossless recovery

Test images produced by the pipeline are in `Test samples/`:

| Image | Encrypted | Decrypted |
|---|---|---|
| Capybara | `Capybara_encrypted.png` | `Capybara_decrypted.png` |
| Snorlax | `Snorlax_encrypted.png` | `Snorlax_decrypted.png` |

### Hardware Performance

| Design | Slice LUTs | Utilization |
|---|---:|---:|
| **Proposed (ANN-based)** | **1604** | **~1.19%** |
| Comparable design w/ extra control logic | 2345 | ~1.74% |

Significantly lower footprint than RK4-based and other FPGA chaotic encryption designs.

### ⚠️ Path Configuration
The notebooks currently contain hard-coded Windows paths, e.g.
os.chdir("E:\\Python n Verilog\\Lorentz 2 and Encryption\\Lorentz Verilog code")
Update these os.chdir(...) lines to point at your local copy of the repo before running. The image encryption notebook also expects:

a lorentz_model.keras file (the saved Keras model — exported from Lorentz - 2.ipynb)
an Example Images/ folder with a Capybara.png (or other) input image

Both are user-supplied — they are not committed to the repo.

## 🔑 Advantages

- ❌ No differential equation solving at runtime
- ⚡ Single-cycle combinational chaotic state update on FPGA
- 🔋 Lower power than RK4-based designs (no iterative floating-point)
- 📉 Reduced LUT and register utilization
- 🛡️ Suitable for embedded and FPGA-based secure imaging

---

## 📚 References

1. E. N. Lorenz, "Deterministic nonperiodic flow," *J. Atmos. Sci.*, vol. 20, pp. 130–141, 1963.
2. A. S. Panackal, A. M. N. V. Jacob, V. Vinayan, "Efficient implementation of chaotic generator on FPGA using ANN," *Proc. ACCESS*, 2025.
3. L. Zhang, "ANN model design and topology analysis for FPGA implementation of Lorenz chaotic generator," *Proc. IEEE CCECE*, 2017.
4. L. Zhang, "ANN model design of Lorenz chaotic system for EEG pattern recognition and prediction," *Proc. IEEE LSC*, 2017.
5. F. Yu et al., "Bursting firings in memristive Hopfield neural network with image encryption and hardware implementation," *IEEE TCAD*, vol. 44, no. 12, 2025.
6. M. S. Azzaz et al., "FPGA-Based ANN-Hyperchaotic PRNG for Secure Audio Applications," *ICACI*, 2025.
7. F. Ciylan, B. Ciylan, M. Atak, "FPGA-based chaotic image encryption using systolic arrays," *Electronics*, vol. 12, no. 12, 2023.
8. E. Tlelo-Cuautle et al., "Chaotic image encryption using Hopfield and Hindmarsh–Rose neurons implemented on FPGA," *Sensors*, vol. 20, no. 5, 2020.
9. G. Chen, Y. Mao, C. K. Chui, "A symmetric image encryption scheme based on 3D chaotic cat maps," *Chaos Solitons Fractals*, vol. 21, no. 3, 2004.
10. Y. Wang et al., "Adaptive fast image encryption algorithm based on three-dimensional chaotic system," *Entropy*, vol. 25, no. 10, 2023.

---
