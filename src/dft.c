#include "../include/pigiem.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Discrete Fourier Transform (DFT)
 * 
 * Forward DFT formula:
 *   X[k] = sum_{n=0}^{N-1} x[n] * e^(-j * 2 * pi * k * n / N)
 *        = sum_{n=0}^{N-1} x[n] * (cos(2*pi*k*n/N) - j*sin(2*pi*k*n/N))
 * 
 * Inverse DFT formula:
 *   x[n] = (1/N) * sum_{k=0}^{N-1} X[k] * e^(j * 2 * pi * k * n / N)
 * 
 * Complexity: O(N^2) for 1D, O(N^2 * M^2) for 2D
 */

lpgm_signal_t*
lpgm_make_empty_signal(int signal_len)
{
	lpgm_signal_t* signal;

	signal = (lpgm_signal_t*)calloc(signal_len, sizeof(lpgm_signal_t));
	if (signal == NULL)
	{
		return NULL;
	}

	return signal;
}

void
lpgm_destroy_signal(lpgm_signal_t* signal)
{
	if (signal != NULL)
	{
		free(signal);
		signal = NULL;
	}
}

lpgm_status_t
lpgm_image_to_signal(const lpgm_image_t* im, lpgm_signal_t* out_signal)
{
	int i;

	if (im->data == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}

	for (i = 0; i < (im->w * im->h); ++i)
	{
		out_signal[i].imaginary = 0;
		out_signal[i].real = im->data[i];
	}

	return LPGM_OK;
}

/*
 * 1D Discrete Fourier Transform
 * 
 * Parameters:
 *   input_signal - input complex signal array
 *   signal_len   - length of the signal (N)
 *   out_signal   - output complex signal array (must be pre-allocated)
 *   inverse      - 0 for forward DFT, 1 for inverse DFT
 * 
 * Formula: X[k] = sum_{n=0}^{N-1} x[n] * e^(-j * 2 * pi * k * n / N)
 */
lpgm_status_t
lpgm_dft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse)
{
	int i, j;
	float t, cost, sint;

	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}

	for (i = 0; i < signal_len; ++i)
	{
		out_signal[i].real = 0;
		out_signal[i].imaginary = 0;

		for (j = 0; j < signal_len; ++j)
		{
			t = -2 * M_PI * i * j / (float)signal_len;

			if (inverse == 1)
			{
				t = -t;
			}

			cost = cos(t);
			sint = sin(t);

			out_signal[i].real += input_signal[j].real * cost - input_signal[j].imaginary * sint;
			out_signal[i].imaginary += input_signal[j].real * sint + input_signal[j].imaginary * cost;
		}

		if (inverse == 1)
		{
			out_signal[i].real /= signal_len;
			out_signal[i].imaginary /= signal_len;
		}
	}

	return LPGM_OK;
}

/*
 * 2D Discrete Fourier Transform (separable, row-column decomposition)
 * 
 * Parameters:
 *   input_signal - input 2D complex signal (rows x cols)
 *   rows, cols   - dimensions of the signal
 *   out_signal   - output 2D complex signal (must be pre-allocated)
 *   inverse      - 0 for forward DFT, 1 for inverse DFT
 * 
 * Method: Apply 1D DFT to each row, then to each column
 * Complexity: O(rows * cols * (rows + cols))
 */
lpgm_status_t
lpgm_dft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse)
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

	/* Allocate temporary buffers instead of VLAs */
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

	// set zeros
	for (i = 0; i < (rows * cols); ++i)
	{
		out_signal[i].real = 0;
		out_signal[i].imaginary = 0;
	}

	// each row DFT
	for (i = 0; i < rows; ++i)
	{
		for (j = 0; j < cols; ++j)
		{
			row_signal[j].imaginary = input_signal[i * cols + j].imaginary;
			row_signal[j].real = input_signal[i * cols + j].real;
		}

		lpgm_dft(row_signal, cols, out_row_signal, inverse);

		for (j = 0; j < cols; ++j)
		{
			temp_signal[i * cols + j].imaginary = out_row_signal[j].imaginary;
			temp_signal[i * cols + j].real = out_row_signal[j].real;
		}
	}

	// each col DFT
	for (i = 0; i < cols; ++i)
	{
		for (j = 0; j < rows; ++j)
		{
			col_signal[j].imaginary = temp_signal[j * cols + i].imaginary;
			col_signal[j].real = temp_signal[j * cols + i].real;
		}

		lpgm_dft(col_signal, rows, out_col_signal, inverse);

		for (j = 0; j < rows; ++j)
		{
			out_signal[j * cols + i].imaginary = out_col_signal[j].imaginary;
			out_signal[j * cols + i].real = out_col_signal[j].real;
		}
	}

	lpgm_destroy_signal(temp_signal);
	lpgm_destroy_signal(row_signal);
	lpgm_destroy_signal(out_row_signal);
	lpgm_destroy_signal(col_signal);
	lpgm_destroy_signal(out_col_signal);

	return LPGM_OK;
}

/*
 * Circular shift of a 2D signal
 * 
 * Parameters:
 *   input_signal - input 2D signal
 *   xdim, ydim   - dimensions
 *   out_signal   - output shifted signal
 *   xshift, yshift - shift amounts
 * 
 * Formula: out[ii][jj] = in[i][j] where ii = (i + xshift) % xdim
 * 
 * Note: Supports in-place operation (input == output) by using temp buffer
 */
lpgm_status_t
lpgm_circshift(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int xshift, int yshift)
{
	int i, j, ii, jj;
	lpgm_signal_t* temp = NULL;
	const lpgm_signal_t* src;

	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}

	/* If in-place, use temporary buffer */
	if (input_signal == out_signal)
	{
		temp = lpgm_make_empty_signal(xdim * ydim);
		if (temp == NULL)
		{
			return LPGM_FAIL;
		}
		for (i = 0; i < xdim * ydim; ++i)
		{
			temp[i] = input_signal[i];
		}
		src = temp;
	}
	else
	{
		src = input_signal;
	}

	for (i = 0; i < xdim; ++i)
	{
		ii = (i + xshift) % xdim;

		if (ii < 0)
		{
			ii = xdim + ii;
		}

		for (j = 0; j < ydim; ++j)
		{
			jj = (j + yshift) % ydim;

			if (jj < 0)
			{
				jj = ydim + jj;
			}

			out_signal[ii * ydim + jj] = src[i * ydim + j];
		}
	}

	if (temp != NULL)
	{
		lpgm_destroy_signal(temp);
	}

	return LPGM_OK;
}

/*
 * Zero-pad a signal to new dimensions
 * 
 * Note: Supports in-place operation (input == output) by using temp buffer
 */
lpgm_status_t
lpgm_zero_padding_signal(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int newxdim, int newydim)
{
	int i, j;
	lpgm_signal_t* temp = NULL;
	const lpgm_signal_t* src;

	if (input_signal == NULL || out_signal == NULL || xdim > newxdim || ydim > newydim)
	{
		return LPGM_FAIL;
	}

	/* If in-place, use temporary buffer */
	if (input_signal == out_signal)
	{
		temp = lpgm_make_empty_signal(xdim * ydim);
		if (temp == NULL)
		{
			return LPGM_FAIL;
		}
		for (i = 0; i < xdim * ydim; ++i)
		{
			temp[i] = input_signal[i];
		}
		src = temp;
	}
	else
	{
		src = input_signal;
	}

	/* Initialize output to zero (important for padding area) */
	for (i = 0; i < newxdim * newydim; ++i)
	{
		out_signal[i].real = 0.0f;
		out_signal[i].imaginary = 0.0f;
	}

	/* Copy original data */
	for (i = 0; i < xdim; ++i)
	{
		for (j = 0; j < ydim; ++j)
		{
			out_signal[i * newydim + j] = src[i * ydim + j];
		}
	}

	if (temp != NULL)
	{
		lpgm_destroy_signal(temp);
	}

	return LPGM_OK;
}

lpgm_status_t
lpgm_print_signal(const lpgm_signal_t* signal, int rows, int cols)
{
	int i, j, index;

	if (signal == NULL)
	{
		return LPGM_FAIL;
	}

	fprintf(stdout, "%s(): ---- start ---- \n", __func__);

	for (i = 0; i < rows; ++i)
	{
		for (j = 0; j < cols; ++j)
		{
			index = i * cols + j;
			if (fabs(signal[index].imaginary) < 10e-7)
			{
				fprintf(stdout, "%.2f  ", signal[index].real);
			}
			else
			{
				fprintf(stdout, "%.2f j%.2f  ", signal[index].real, signal[index].imaginary);
			}
		}
		fprintf(stdout, "\n");
	}

	fprintf(stdout, "%s(): ---- end ---- \n", __func__);

	return LPGM_OK;
}
