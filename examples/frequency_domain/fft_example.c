/*
 * FFT (Fast Fourier Transform) Example
 * 
 * This example demonstrates how to use the 2D FFT functions for
 * efficient frequency domain analysis of an image.
 * 
 * Usage:
 *   ./fft_example.out input.pgm
 * 
 * FFT Algorithm: Cooley-Tukey Radix-2
 * 
 * Butterfly Operation:
 *   X[k]     = E[k] + W_N^k * O[k]
 *   X[k+N/2] = E[k] - W_N^k * O[k]
 * 
 * where W_N^k = e^(-j * 2 * pi * k / N) is the "twiddle factor"
 * 
 * Complexity: O(N log N) vs O(N^2) for DFT
 * 
 * Note: FFT requires dimensions to be powers of 2.
 * This example automatically zero-pads the image if necessary.
 * 
 * This example:
 *   1. Reads an image
 *   2. Zero-pads to next power of 2 dimensions
 *   3. Applies 2D FFT
 *   4. Calculates magnitude spectrum
 *   5. Shifts zero-frequency to center
 *   6. Saves the magnitude spectrum
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	lpgm_t pgm;
	lpgm_signal_t* signal;
	lpgm_signal_t* padded_signal;
	lpgm_signal_t* fft_signal;
	lpgm_signal_t* shifted_signal;
	lpgm_image_t magnitude_im;
	lpgm_t out_pgm;
	int rows, cols, i, j, len;
	int padded_rows, padded_cols, padded_len;
	float magnitude;
	clock_t start, end;
	double cpu_time;
	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s input.pgm\n", argv[0]);
		return -1;
	}
	
	input_file = argv[1];
	
	/* Read input image */
	fprintf(stdout, "Reading: %s\n", input_file);
	if (lpgm_file_read(input_file, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not read file %s\n", input_file);
		return -1;
	}
	
	rows = pgm.im.h;
	cols = pgm.im.w;
	len = rows * cols;
	
	fprintf(stdout, "Original image size: %d x %d\n", cols, rows);
	
	/* Calculate padded dimensions (next power of 2) */
	padded_rows = lpgm_next_power_of_two(rows);
	padded_cols = lpgm_next_power_of_two(cols);
	padded_len = padded_rows * padded_cols;
	
	fprintf(stdout, "Padded size (power of 2): %d x %d\n", padded_cols, padded_rows);
	
	/* Allocate signal arrays */
	signal = lpgm_make_empty_signal(len);
	padded_signal = lpgm_make_empty_signal(padded_len);
	fft_signal = lpgm_make_empty_signal(padded_len);
	shifted_signal = lpgm_make_empty_signal(padded_len);
	
	if (signal == NULL || padded_signal == NULL || fft_signal == NULL || shifted_signal == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed\n");
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	/* Convert image to complex signal */
	lpgm_image_to_signal(&pgm.im, signal);
	
	/* Zero-pad the signal */
	/* Initialize padded signal to zeros (already done by lpgm_make_empty_signal) */
	/* Copy original signal to top-left corner */
	for (i = 0; i < rows; ++i)
	{
		for (j = 0; j < cols; ++j)
		{
			padded_signal[i * padded_cols + j].real = signal[i * cols + j].real;
			padded_signal[i * padded_cols + j].imaginary = signal[i * cols + j].imaginary;
		}
	}
	
	/* Apply 2D FFT */
	fprintf(stdout, "Applying 2D FFT...\n");
	start = clock();
	
	if (lpgm_fft2(padded_signal, padded_rows, padded_cols, fft_signal, 0) != LPGM_OK)
	{
		fprintf(stderr, "Error: FFT failed\n");
		lpgm_destroy_signal(signal);
		lpgm_destroy_signal(padded_signal);
		lpgm_destroy_signal(fft_signal);
		lpgm_destroy_signal(shifted_signal);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	end = clock();
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	fprintf(stdout, "FFT completed in %.3f seconds\n", cpu_time);
	
	/* Shift zero-frequency to center */
	lpgm_circshift(fft_signal, padded_rows, padded_cols, shifted_signal, 
	               padded_rows / 2, padded_cols / 2);
	
	/* Calculate magnitude spectrum and create output image */
	magnitude_im = lpgm_make_empty_image(padded_cols, padded_rows);
	
	for (i = 0; i < padded_len; ++i)
	{
		/* Magnitude = sqrt(real^2 + imag^2) */
		magnitude = sqrtf(shifted_signal[i].real * shifted_signal[i].real +
		                  shifted_signal[i].imaginary * shifted_signal[i].imaginary);
		
		/* Apply log scaling for better visualization: log(1 + magnitude) */
		magnitude_im.data[i] = logf(1.0f + magnitude);
	}
	
	/* Normalize to 0-255 range */
	lpgm_normalize_image_data(&magnitude_im, 255.0f);
	
	/* Save magnitude spectrum */
	out_pgm = pgm;
	out_pgm.im = magnitude_im;
	if (lpgm_file_write(&out_pgm, "output_fft_magnitude.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_fft_magnitude.pgm\n");
	}
	else
	{
		fprintf(stdout, "Saved: output_fft_magnitude.pgm\n");
	}
	
	/* Performance comparison note */
	fprintf(stdout, "\n--- Performance Note ---\n");
	fprintf(stdout, "FFT complexity: O(N log N)\n");
	fprintf(stdout, "DFT complexity: O(N^2)\n");
	fprintf(stdout, "For a %dx%d image, FFT is approximately %.0fx faster than DFT.\n",
	        padded_cols, padded_rows, 
	        (float)(padded_rows * padded_cols) / (2.0f * log2f((float)(padded_rows * padded_cols))));
	
	/* Cleanup */
	lpgm_destroy_signal(signal);
	lpgm_destroy_signal(padded_signal);
	lpgm_destroy_signal(fft_signal);
	lpgm_destroy_signal(shifted_signal);
	lpgm_image_destroy(&magnitude_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "\nDone!\n");
	
	return 0;
}
