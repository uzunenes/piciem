#ifndef PGM_API_H
#define PGM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

	/*
	 * ============================================================================
	 * Piciem: Lightweight Image Processing Library
	 * ============================================================================
	 * 
	 * A simple C library for grayscale image processing, designed for educational
	 * purposes. Uses only standard C libraries (no external dependencies).
	 * 
	 * Features:
	 *   - PGM file I/O (P2 ASCII and P5 binary formats)
	 *   - Basic image operations (brightness, contrast, invert, threshold)
	 *   - Histogram equalization and Otsu's thresholding
	 *   - Edge detection (Sobel operator)
	 *   - Discrete Fourier Transform (DFT) and Fast Fourier Transform (FFT)
	 * 
	 * ============================================================================
	 */

	/* Status codes for function return values */
	typedef enum
	{
		LPGM_OK = 0,      /* Operation successful */
		LPGM_FAIL = -1    /* Operation failed */
	} lpgm_status_t;

	/* Complex number for DFT/FFT operations */
	typedef struct
	{
		float imaginary;  /* Imaginary part */
		float real;       /* Real part */
	} lpgm_signal_t;

	/* Grayscale image structure */
	typedef struct
	{
		int w, h;         /* Width (columns) and height (rows) */
		float* data;      /* Pixel data in row-major order: data[row * w + col] */
	} lpgm_image_t;

	/* PGM file structure */
	typedef struct
	{
		char magic_number[2 + 1];  /* "P2" (ASCII) or "P5" (binary) */
		char* comment;            /* Optional comment string */
		int max_val;              /* Maximum pixel value (usually 255) */
		lpgm_image_t im;          /* Image data */
	} lpgm_t;

	/* ========================================================================
	 * PGM File I/O Functions (pgm_io.c)
	 * ======================================================================== */

	/* Read a PGM image from file. Supports P2 (ASCII) and P5 (binary) formats. */
	lpgm_status_t lpgm_file_read(const char* file_name, lpgm_t* pgm);

	/* Write a PGM image to file. Uses the same format as the last read file. */
	lpgm_status_t lpgm_file_write(const lpgm_t* pgm, const char* file_name);

	/* Free memory allocated for a PGM structure. */
	void lpgm_file_destroy(lpgm_t* pgm);

	/* ========================================================================
	 * Utility Functions (utils.c)
	 * ======================================================================== */

	/* Get value from a 2D array stored as 1D. Formula: data[x * cols + y] */
	float lpgm_get_2Darray_value(const float* data, int cols, int x, int y);

	/* Normalize array values to range [0, new_max]. */
	lpgm_status_t lpgm_normalize_array(float* data, int len, float new_max);

	/* ========================================================================
	 * Image Operations (image.c)
	 * ======================================================================== */

	/* Create an empty image with given dimensions. Pixels initialized to 0. */
	lpgm_image_t lpgm_make_empty_image(int w, int h);

	/* Create a deep copy of an image. */
	lpgm_image_t lpgm_copy_image(const lpgm_image_t* input_im);

	/* Add a border of given size around the image. */
	lpgm_image_t lpgm_border_image(const lpgm_image_t* input_im, int border_size);

	/* Free memory allocated for an image. */
	void lpgm_image_destroy(lpgm_image_t* im);

	/* Get pixel value at (x, y). x = row, y = column. */
	float lpgm_get_pixel_value(const lpgm_image_t* im, int x, int y);

	/* Get pixel value with bounds checking. Returns 0 for out-of-bounds. */
	float lpgm_get_pixel_extend_value(const lpgm_image_t* im, int x, int y);

	/* Set pixel value at (x, y). */
	void lpgm_set_pixel_value(lpgm_image_t* im, int x, int y, float val);

	/* Normalize image data to range [0, new_max]. */
	void lpgm_normalize_image_data(lpgm_image_t* im, float new_max);

	/* Apply a convolution filter. box_kernel_size must be odd (e.g., 3, 5, 7). */
	lpgm_image_t lpgm_filter_image(const lpgm_image_t* im, const float* box_kernel_data, int box_kernel_size);

	/* 
	 * Brightness adjustment. Formula: out = in + delta
	 * delta > 0: brighter, delta < 0: darker
	 */
	lpgm_image_t lpgm_brightness(const lpgm_image_t* im, float delta);

	/* 
	 * Contrast adjustment. Formula: out = (in - 128) * factor + 128
	 * factor > 1: more contrast, factor < 1: less contrast
	 */
	lpgm_image_t lpgm_contrast(const lpgm_image_t* im, float factor);

	/* 
	 * Image inversion (negative). Formula: out = 255 - in
	 */
	lpgm_image_t lpgm_invert(const lpgm_image_t* im);

	/* 
	 * Binary thresholding. Formula: out = (in > threshold) ? 255 : 0
	 */
	lpgm_image_t lpgm_threshold(const lpgm_image_t* im, float threshold);

	/* 
	 * Otsu's automatic thresholding.
	 * Finds optimal threshold by maximizing between-class variance.
	 * Formula: sigma_B^2 = w0 * w1 * (mu0 - mu1)^2
	 */
	lpgm_image_t lpgm_otsu_threshold(const lpgm_image_t* im);

	/* 
	 * Histogram equalization for contrast enhancement.
	 * Formula: out = (CDF[in] - CDF_min) / (1 - CDF_min) * 255
	 */
	lpgm_image_t lpgm_histogram_equalization(const lpgm_image_t* im);

	/* 
	 * Sobel edge detection.
	 * Uses 3x3 Sobel kernels for Gx and Gy gradients.
	 * Formula: G = sqrt(Gx^2 + Gy^2)
	 */
	lpgm_image_t lpgm_sobel(const lpgm_image_t* im);

	/* 
	 * Generic NxN convolution with zero-padding.
	 * Formula: out[x,y] = sum_{i,j} in[x+i, y+j] * kernel[i,j]
	 * ksize must be odd (3, 5, 7, ...).
	 */
	lpgm_image_t lpgm_convolve(const lpgm_image_t* im, const float* kernel, int ksize);

	/* 
	 * Median filter for noise removal.
	 * Replaces each pixel with median of its NxN neighborhood.
	 * ksize must be odd (3, 5, 7, ...).
	 */
	lpgm_image_t lpgm_median_filter(const lpgm_image_t* im, int ksize);

	/* 
	 * Add salt & pepper noise.
	 * density: fraction of pixels to corrupt (0.0 to 1.0).
	 * Example: density = 0.05 corrupts 5% of pixels.
	 */
	lpgm_image_t lpgm_add_salt_pepper_noise(const lpgm_image_t* im, float density);

	/* 
	 * Gamma correction.
	 * Formula: out = 255 * (in / 255) ^ gamma
	 * gamma < 1: brighter, gamma > 1: darker
	 */
	lpgm_image_t lpgm_gamma(const lpgm_image_t* im, float gamma);

	/* ========================================================================
	 * DFT Functions (dft.c) - O(N^2) complexity
	 * ======================================================================== */

	/* Allocate a complex signal array of given length. */
	lpgm_signal_t* lpgm_make_empty_signal(int signal_len);

	/* Free memory allocated for a signal. */
	void lpgm_destroy_signal(lpgm_signal_t* signal);

	/* Convert image to complex signal (imaginary parts set to 0). */
	lpgm_status_t lpgm_image_to_signal(const lpgm_image_t* im, lpgm_signal_t* out_signal);

	/* 
	 * 1D Discrete Fourier Transform.
	 * Formula: X[k] = sum_{n=0}^{N-1} x[n] * e^(-j*2*pi*k*n/N)
	 * inverse = 0: forward DFT, inverse = 1: inverse DFT
	 */
	lpgm_status_t lpgm_dft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse);

	/* 
	 * 2D DFT using row-column decomposition.
	 * Complexity: O(rows * cols * (rows + cols))
	 */
	lpgm_status_t lpgm_dft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse);

	/* Circular shift of a 2D signal. */
	lpgm_status_t lpgm_circshift(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int xshift, int yshift);

	/* Zero-pad a signal to new dimensions. */
	lpgm_status_t lpgm_zero_padding_signal(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int newxdim, int newydim);

	/* Print signal values to stdout (for debugging). */
	lpgm_status_t lpgm_print_signal(const lpgm_signal_t* signal, int rows, int cols);

	/* ========================================================================
	 * FFT Functions (fft.c) - O(N log N) complexity, Cooley-Tukey algorithm
	 * ======================================================================== */

	/* 
	 * Calculate next power of 2 >= n.
	 * Use for zero-padding before FFT.
	 * Example: lpgm_next_power_of_two(5) = 8
	 */
	int lpgm_next_power_of_two(int n);

	/* 
	 * 1D Fast Fourier Transform (Cooley-Tukey radix-2).
	 * signal_len MUST be a power of 2.
	 * inverse = 0: forward FFT, inverse = 1: inverse FFT
	 */
	lpgm_status_t lpgm_fft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse);

	/* 
	 * 2D FFT using row-column decomposition.
	 * rows and cols MUST be powers of 2.
	 * Complexity: O(rows * cols * log(rows * cols))
	 */
	lpgm_status_t lpgm_fft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse);

	/* ========================================================================
	 * Frequency Domain Filters (fft.c)
	 * Apply in frequency domain after FFT, before inverse FFT
	 * ======================================================================== */

	/* 
	 * Ideal Low Pass Filter
	 * H(u,v) = 1 if D(u,v) <= cutoff, 0 otherwise
	 * D(u,v) = distance from center
	 */
	void lpgm_filter_ideal_lowpass(lpgm_signal_t* signal, int rows, int cols, float cutoff);

	/* 
	 * Ideal High Pass Filter
	 * H(u,v) = 0 if D(u,v) <= cutoff, 1 otherwise
	 */
	void lpgm_filter_ideal_highpass(lpgm_signal_t* signal, int rows, int cols, float cutoff);

	/* 
	 * Butterworth Low Pass Filter
	 * H(u,v) = 1 / (1 + (D(u,v)/cutoff)^(2*order))
	 * Smoother transition than ideal filter
	 */
	void lpgm_filter_butterworth_lowpass(lpgm_signal_t* signal, int rows, int cols, float cutoff, int order);

	/* 
	 * Butterworth High Pass Filter
	 * H(u,v) = 1 / (1 + (cutoff/D(u,v))^(2*order))
	 */
	void lpgm_filter_butterworth_highpass(lpgm_signal_t* signal, int rows, int cols, float cutoff, int order);

	/* 
	 * Gaussian Low Pass Filter
	 * H(u,v) = e^(-D(u,v)^2 / (2*sigma^2))
	 * No ringing artifacts
	 */
	void lpgm_filter_gaussian_lowpass(lpgm_signal_t* signal, int rows, int cols, float sigma);

	/* 
	 * Gaussian High Pass Filter
	 * H(u,v) = 1 - e^(-D(u,v)^2 / (2*sigma^2))
	 */
	void lpgm_filter_gaussian_highpass(lpgm_signal_t* signal, int rows, int cols, float sigma);

	/* ========================================================================
	 * Morphological Operations (image.c)
	 * Binary image operations using structuring element
	 * ======================================================================== */

	/* 
	 * Erosion - shrinks white regions
	 * Minimum value in neighborhood
	 */
	lpgm_image_t lpgm_erode(const lpgm_image_t* im, int ksize);

	/* 
	 * Dilation - expands white regions
	 * Maximum value in neighborhood
	 */
	lpgm_image_t lpgm_dilate(const lpgm_image_t* im, int ksize);

	/* 
	 * Opening - erosion followed by dilation
	 * Removes small white spots
	 */
	lpgm_image_t lpgm_opening(const lpgm_image_t* im, int ksize);

	/* 
	 * Closing - dilation followed by erosion
	 * Fills small black holes
	 */
	lpgm_image_t lpgm_closing(const lpgm_image_t* im, int ksize);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
