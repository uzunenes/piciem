## Piciem: Image Processing Software Library

**Piciem** is a lightweight image processing library written in C, designed for educational purposes and basic image manipulation tasks. It relies solely on the C standard library, making it portable and easy to understand for beginners.

Piciem serves as a simple resource for learning image processing concepts and building efficient software without external dependencies, ideal for those looking to deepen their understanding of C programming and fundamental image processing algorithms.

## Key Features

- **No External Libraries**: Built entirely with the C standard library for maximum portability.
- **Educational Focus**: Clear and minimal implementations of core image processing techniques.
- **Basic Applications**: Includes practical examples for fundamental operations like transformations, filtering, and edge detection.

## Getting Started

### Prerequisites

- A C compiler (e.g., GCC, Clang)

### Installation

1. Clone the repository:
```bash
   git clone https://github.com/uzunenes/piciem.git
   cd piciem
```

2. Build the library and examples:
```bash
make
```

3. Run the sample application:
```bash
./bin/sample_application
```

## Example Applications

The library provides simple examples demonstrating fundamental grayscale image processing techniques:

- **Read and Write**: Load and save grayscale images from and to files.
- **Grayscale Conversion**: Convert a color image to grayscale using basic mathematical operations.
- **Brightness Adjustment**: Increase or decrease image brightness by modifying pixel values.
- **Contrast Adjustment**: Stretch or compress pixel intensity ranges to adjust image contrast.
- **Thresholding**: Apply a binary threshold to segment images based on pixel intensity.
- **Inversion**: Create negative images by inverting pixel values.
- **Basic Filters**: Apply smoothing (blur) and sharpening filters using simple convolution kernels.
- **Edge Detection**: Detect edges using basic operators like Sobel and Prewitt.
- **Homomorphic Filtering**: Enhance image contrast and illumination characteristics.

All implementations focus on clarity and simplicity, using only standard C functionality.

