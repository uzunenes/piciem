## Piciem: Image Processing Software Library

**Piciem** is a lightweight image processing library written in C, designed for educational purposes and basic image manipulation tasks. It relies solely on the C standard library, making it portable and easy to understand for beginners.

Piciem serves as a simple resource for learning image processing concepts and building efficient software without external dependencies, ideal for those looking to deepen their understanding of C programming and fundamental image processing algorithms.

## Key Features

- **No External Libraries**: Built entirely with the C standard library for maximum portability.
- **Educational Focus**: Clear and minimal implementations of core image processing techniques.
- **Basic Applications**: Includes practical examples for fundamental operations like transformations, filtering, edge detection etc.

## Getting Started

### Prerequisites

- A C compiler (e.g., GCC, Clang)

### Installation

1. Clone the repository:
```bash
   git clone https://github.com/uzunenes/piciem.git
   cd piciem
```

2. Build and install library:
```bash
make
make uninstall
make install
```

3. Run the sample application:
```bash
cd ./examples
ls -l
cd ./pgm_io
make
./pgm_read_chance_block_write input-image.pgm
```

## Example Applications

The library provides simple examples demonstrating fundamental grayscale image processing techniques:

### Basic Image I/O Operations
- **Read and Write**: Load and save grayscale images from and to files.

### Image Transformations
- **Grayscale Conversion**: Convert a color image to grayscale using basic mathematical operations.
- **Brightness Adjustment**: Increase or decrease image brightness by modifying pixel values.
- **Contrast Adjustment**: Stretch or compress pixel intensity ranges to adjust image contrast.
- **Negative Image**: Create a negative image by inverting pixel values.
- **Power Law Transformation**: Apply a nonlinear transformation to adjust image brightness and contrast.

### Filtering and Noise Reduction
- **Basic Filters**: Apply smoothing (blur) and sharpening filters using simple convolution kernels.
- **Average Filter Image**: Apply an averaging filter (smooth the image) using the `lpgm_filter_image()` function.
- **Homomorphic Filtering**: Enhance image contrast and illumination characteristics.

### Edge Detection and Enhancement
- **Edge Detection**: Detect edges using basic operators like Sobel and Prewitt.
- **Histogram Equalization**: Improve image contrast by spreading out the most frequent intensity values.

### Image Segmentation
- **Thresholding**: Apply a binary threshold to segment images based on pixel intensity.
- **Otsu Thresholding**: Automatically segment an image into foreground and background by selecting an optimal threshold value.

All implementations focus on clarity and simplicity, using only standard C functionality.
