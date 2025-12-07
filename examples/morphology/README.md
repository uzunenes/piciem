# Morphology

Erosion, dilation, opening, closing on binary images.

## Usage

```bash
make
./morphology_example.out ../pgm_io/lena_ascii.pgm
```

Output:
- `output_binary.pgm` - Otsu thresholded
- `output_eroded.pgm` - Erosion (shrinks white)
- `output_dilated.pgm` - Dilation (expands white)
- `output_opening.pgm` - Opening (removes small white spots)
- `output_closing.pgm` - Closing (fills small black holes)

## Homework

### 1. Different Kernel Sizes
Try ksize = 3, 5, 7. Compare results.

### 2. Multiple Iterations
Apply erosion multiple times:
```c
eroded = lpgm_erode(&binary, 3);
eroded2 = lpgm_erode(&eroded, 3);
```

### 3. Edge Detection with Morphology
```c
dilated = lpgm_dilate(&binary, 3);
/* edge = dilated - eroded */
for (i = 0; i < size; i++)
    edge.data[i] = dilated.data[i] - eroded.data[i];
```

### 4. Noise Removal
Add noise, then apply opening to remove:
```c
noisy = lpgm_add_salt_pepper_noise(&binary, 0.05f);
cleaned = lpgm_opening(&noisy, 3);
```

## Operations

| Operation | Effect |
|-----------|--------|
| Erosion | Minimum in window, shrinks white |
| Dilation | Maximum in window, expands white |
| Opening | Erosion + Dilation, removes spots |
| Closing | Dilation + Erosion, fills holes |
