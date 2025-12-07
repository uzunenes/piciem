# Frequency Domain Filters

Low pass filter example using FFT.

## Usage

```bash
make
./lowpass_example.out ../pgm_io/lena_ascii.pgm
```

## Homework

### 1. High Pass Filter
Replace `lpgm_filter_butterworth_lowpass` with `lpgm_filter_butterworth_highpass`.

### 2. Different Cutoff Values
Try cutoff = 10, 30, 50, 100. Observe the difference.

### 3. Compare Filter Types

```c
/* Ideal - sharp cutoff, ringing artifacts */
lpgm_filter_ideal_lowpass(freq, rows, cols, 30.0f);

/* Butterworth - smooth transition */
lpgm_filter_butterworth_lowpass(freq, rows, cols, 30.0f, 2);

/* Gaussian - smoothest, no ringing */
lpgm_filter_gaussian_lowpass(freq, rows, cols, 30.0f);
```

### 4. Band Pass Filter
Combine high pass and low pass:
```c
lpgm_filter_butterworth_highpass(freq, rows, cols, 10.0f, 2);
lpgm_filter_butterworth_lowpass(freq, rows, cols, 50.0f, 2);
```

## Available Filters

| Function | Formula |
|----------|---------|
| `lpgm_filter_ideal_lowpass` | H = 1 if D <= cutoff |
| `lpgm_filter_ideal_highpass` | H = 1 if D > cutoff |
| `lpgm_filter_butterworth_lowpass` | H = 1/(1+(D/c)^2n) |
| `lpgm_filter_butterworth_highpass` | H = 1/(1+(c/D)^2n) |
| `lpgm_filter_gaussian_lowpass` | H = e^(-D²/2σ²) |
| `lpgm_filter_gaussian_highpass` | H = 1 - e^(-D²/2σ²) |
