/*
 * DFT (Discrete Fourier Transform) Example
 * 
 * This example demonstrates how to use the 2D DFT functions for
 * frequency domain analysis of an image.
 * 
 * Usage:
 *   ./dft_example.out input.pgm
 * 
 * DFT Formula:
 *   X[k] = sum_{n=0}^{N-1} x[n] * e^(-j * 2 * pi * k * n / N)
 * 
 * Note: DFT has O(N^2) complexity, which is slow for large images.
 * For better performance, use FFT (see fft_example.c).
 * 
 * This example:
 *   1. Reads an image and converts to complex signal
 *   2. Applies 2D DFT
 *   3. Calculates magnitude spectrum
 *   4. Shifts zero-frequency to center (for visualization)
 *   5. Saves the magnitude spectrum as an image
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
	lpgm_signal_t* dft_signal;
	lpgm_signal_t* shifted_signal;
	lpgm_image_t magnitude_im;
	lpgm_t out_pgm;
	int rows, cols, i, len;
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
	
	fprintf(stdout, "Image size: %d x %d\n", cols, rows);
	
	/* Allocate signal arrays */
	signal = lpgm_make_empty_signal(len);
	dft_signal = lpgm_make_empty_signal(len);
	shifted_signal = lpgm_make_empty_signal(len);
	
	if (signal == NULL || dft_signal == NULL || shifted_signal == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed\n");
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	/* Convert image to complex signal */
	lpgm_image_to_signal(&pgm.im, signal);
	
	/* Apply 2D DFT */
	fprintf(stdout, "Applying 2D DFT (this may take a while for large images)...\n");
	start = clock();
	
	if (lpgm_dft2(signal, rows, cols, dft_signal, 0) != LPGM_OK)
	{
		fprintf(stderr, "Error: DFT failed\n");
		lpgm_destroy_signal(signal);
		lpgm_destroy_signal(dft_signal);
		lpgm_destroy_signal(shifted_signal);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	end = clock();
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	fprintf(stdout, "DFT completed in %.3f seconds\n", cpu_time);
	
	/* Shift zero-frequency to center */
	lpgm_circshift(dft_signal, rows, cols, shifted_signal, rows / 2, cols / 2);
	
	/* Calculate magnitude spectrum and create output image */
	magnitude_im = lpgm_make_empty_image(cols, rows);
	
	for (i = 0; i < len; ++i)
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
	if (lpgm_file_write(&out_pgm, "output_dft_magnitude.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_dft_magnitude.pgm\n");
	}
	else
	{
		fprintf(stdout, "Saved: output_dft_magnitude.pgm\n");
	}
	
	/* Cleanup */
	lpgm_destroy_signal(signal);
	lpgm_destroy_signal(dft_signal);
	lpgm_destroy_signal(shifted_signal);
	lpgm_image_destroy(&magnitude_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
