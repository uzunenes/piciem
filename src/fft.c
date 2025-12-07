/*
 * Fast Fourier Transform (FFT) - Cooley-Tukey Radix-2 Algorithm
 * 
 * FFT is an efficient algorithm to compute DFT in O(N log N) instead of O(N^2).
 * 
 * Cooley-Tukey Algorithm (Decimation in Time):
 *   The DFT of size N is recursively broken into two DFTs of size N/2:
 *   - Even-indexed elements: x[0], x[2], x[4], ...
 *   - Odd-indexed elements:  x[1], x[3], x[5], ...
 * 
 * Butterfly Operation:
 *   X[k]     = E[k] + W_N^k * O[k]
 *   X[k+N/2] = E[k] - W_N^k * O[k]
 * 
 * where W_N^k = e^(-j * 2 * pi * k / N) is the "twiddle factor"
 * 
 * Requirement: N must be a power of 2. Use zero-padding if necessary.
 */

#include "../include/pigiem.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Calculate the next power of two >= n
 * 
 * Example: next_power_of_two(5) = 8, next_power_of_two(8) = 8
 * Used for zero-padding before FFT
 */
int
lpgm_next_power_of_two(int n)
{
	int power = 1;
	while (power < n)
	{
		power *= 2;
	}
	return power;
}

/*
 * Bit-reversal permutation
 * 
 * FFT requires input to be in bit-reversed order for in-place computation.
 * Example for N=8: 0,1,2,3,4,5,6,7 -> 0,4,2,6,1,5,3,7
 */
static void
bit_reverse_copy(const lpgm_signal_t* input, lpgm_signal_t* output, int n)
{
	int i, j, k;
	int log2n = 0;
	int temp = n;
	
	/* Calculate log2(n) */
	while (temp > 1)
	{
		temp /= 2;
		log2n++;
	}
	
	for (i = 0; i < n; ++i)
	{
		/* Reverse bits of i */
		j = 0;
		temp = i;
		for (k = 0; k < log2n; ++k)
		{
			j = (j << 1) | (temp & 1);
			temp >>= 1;
		}
		output[j].real = input[i].real;
		output[j].imaginary = input[i].imaginary;
	}
}

/*
 * 1D Fast Fourier Transform (Cooley-Tukey Radix-2)
 * 
 * Parameters:
 *   input_signal - input complex signal array
 *   signal_len   - length of the signal (MUST be power of 2)
 *   out_signal   - output complex signal array (must be pre-allocated)
 *   inverse      - 0 for forward FFT, 1 for inverse FFT
 * 
 * Complexity: O(N log N)
 * 
 * Butterfly formula:
 *   t = W * odd
 *   even' = even + t
 *   odd'  = even - t
 * 
 * where W = e^(-j * 2 * pi * k / N) for forward, e^(+j * ...) for inverse
 */
lpgm_status_t
lpgm_fft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse)
{
	int i, j, k;
	int m, m2;
	float angle;
	float t_real, t_imag;
	float u_real, u_imag;
	float w_real, w_imag;
	
	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}
	
	/* Check if signal_len is power of 2 */
	if ((signal_len & (signal_len - 1)) != 0)
	{
		fprintf(stderr, "%s(): signal_len must be power of 2, got %d\n", __func__, signal_len);
		return LPGM_FAIL;
	}
	
	/* Bit-reversal permutation */
	bit_reverse_copy(input_signal, out_signal, signal_len);
	
	/* Iterative FFT (bottom-up) */
	/* m = current transform size, starting from 2 and doubling each stage */
	for (m = 2; m <= signal_len; m *= 2)
	{
		m2 = m / 2;
		
		/* Twiddle factor angle step */
		angle = -2.0f * M_PI / (float)m;
		if (inverse)
		{
			angle = -angle;
		}
		
		/* W_m = e^(j * angle) - principal root of unity */
		w_real = cos(angle);
		w_imag = sin(angle);
		
		/* Process each group of m elements */
		for (k = 0; k < signal_len; k += m)
		{
			/* Initialize twiddle factor W = 1 */
			u_real = 1.0f;
			u_imag = 0.0f;
			
			/* Butterfly operations within this group */
			for (j = 0; j < m2; ++j)
			{
				/* t = W * out[k + j + m2] */
				t_real = u_real * out_signal[k + j + m2].real - u_imag * out_signal[k + j + m2].imaginary;
				t_imag = u_real * out_signal[k + j + m2].imaginary + u_imag * out_signal[k + j + m2].real;
				
				/* Butterfly: 
				 * out[k + j]      = out[k + j] + t
				 * out[k + j + m2] = out[k + j] - t
				 */
				out_signal[k + j + m2].real = out_signal[k + j].real - t_real;
				out_signal[k + j + m2].imaginary = out_signal[k + j].imaginary - t_imag;
				
				out_signal[k + j].real = out_signal[k + j].real + t_real;
				out_signal[k + j].imaginary = out_signal[k + j].imaginary + t_imag;
				
				/* Update twiddle factor: W = W * W_m */
				t_real = u_real * w_real - u_imag * w_imag;
				t_imag = u_real * w_imag + u_imag * w_real;
				u_real = t_real;
				u_imag = t_imag;
			}
		}
	}
	
	/* Normalize for inverse FFT */
	if (inverse)
	{
		for (i = 0; i < signal_len; ++i)
		{
			out_signal[i].real /= signal_len;
			out_signal[i].imaginary /= signal_len;
		}
	}
	
	return LPGM_OK;
}

/*
 * 2D Fast Fourier Transform (separable, row-column decomposition)
 * 
 * Parameters:
 *   input_signal - input 2D complex signal (rows x cols)
 *   rows, cols   - dimensions of the signal (MUST be powers of 2)
 *   out_signal   - output 2D complex signal (must be pre-allocated)
 *   inverse      - 0 for forward FFT, 1 for inverse FFT
 * 
 * Method: Apply 1D FFT to each row, then to each column
 * Complexity: O(rows * cols * log(rows * cols))
 * 
 * Note: If dimensions are not power of 2, use lpgm_next_power_of_two() 
 *       and zero-pad before calling this function.
 */
lpgm_status_t
lpgm_fft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse)
{
	int i, j;
	lpgm_signal_t* temp_signal;
	lpgm_signal_t* row_signal;
	lpgm_signal_t* out_row_signal;
	lpgm_signal_t* col_signal;
	lpgm_signal_t* out_col_signal;
	
	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}
	
	/* Check if dimensions are power of 2 */
	if ((rows & (rows - 1)) != 0 || (cols & (cols - 1)) != 0)
	{
		fprintf(stderr, "%s(): rows and cols must be powers of 2, got %dx%d\n", __func__, rows, cols);
		return LPGM_FAIL;
	}
	
	/* Allocate temporary buffers */
	temp_signal = lpgm_make_empty_signal(rows * cols);
	row_signal = lpgm_make_empty_signal(cols);
	out_row_signal = lpgm_make_empty_signal(cols);
	col_signal = lpgm_make_empty_signal(rows);
	out_col_signal = lpgm_make_empty_signal(rows);
	
	if (temp_signal == NULL || row_signal == NULL || out_row_signal == NULL ||
	    col_signal == NULL || out_col_signal == NULL)
	{
		lpgm_destroy_signal(temp_signal);
		lpgm_destroy_signal(row_signal);
		lpgm_destroy_signal(out_row_signal);
		lpgm_destroy_signal(col_signal);
		lpgm_destroy_signal(out_col_signal);
		return LPGM_FAIL;
	}
	
	/* Initialize output to zeros */
	for (i = 0; i < (rows * cols); ++i)
	{
		out_signal[i].real = 0;
		out_signal[i].imaginary = 0;
	}
	
	/* Step 1: Apply 1D FFT to each row */
	for (i = 0; i < rows; ++i)
	{
		/* Copy row i to row_signal */
		for (j = 0; j < cols; ++j)
		{
			row_signal[j].imaginary = input_signal[i * cols + j].imaginary;
			row_signal[j].real = input_signal[i * cols + j].real;
		}
		
		/* FFT of the row */
		lpgm_fft(row_signal, cols, out_row_signal, inverse);
		
		/* Store result in temp_signal */
		for (j = 0; j < cols; ++j)
		{
			temp_signal[i * cols + j].imaginary = out_row_signal[j].imaginary;
			temp_signal[i * cols + j].real = out_row_signal[j].real;
		}
	}
	
	/* Step 2: Apply 1D FFT to each column */
	for (i = 0; i < cols; ++i)
	{
		/* Copy column i to col_signal */
		for (j = 0; j < rows; ++j)
		{
			col_signal[j].imaginary = temp_signal[j * cols + i].imaginary;
			col_signal[j].real = temp_signal[j * cols + i].real;
		}
		
		/* FFT of the column */
		lpgm_fft(col_signal, rows, out_col_signal, inverse);
		
		/* Store result in out_signal */
		for (j = 0; j < rows; ++j)
		{
			out_signal[j * cols + i].imaginary = out_col_signal[j].imaginary;
			out_signal[j * cols + i].real = out_col_signal[j].real;
		}
	}
	
	/* Free allocated buffers */
	lpgm_destroy_signal(temp_signal);
	lpgm_destroy_signal(row_signal);
	lpgm_destroy_signal(out_row_signal);
	lpgm_destroy_signal(col_signal);
	lpgm_destroy_signal(out_col_signal);
	
	return LPGM_OK;
}
