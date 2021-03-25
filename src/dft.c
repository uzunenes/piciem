#include "../include/libpgm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

lpgm_status_t
lpgm_dft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse)
{
	int i, j;
	float t, cost, sint;
	const float pi = 3.14;

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
			t = -2 * pi * i * j / (float)signal_len;

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

lpgm_status_t
lpgm_dft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse)
{
	int i, j;
	lpgm_signal_t* temp_signal;
	lpgm_signal_t row_signal[cols], out_row_signal[cols], col_signal[rows], out_col_signal[rows];

	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
	}

	temp_signal = lpgm_make_empty_signal(rows * cols);
	if (temp_signal == NULL)
	{
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

	return LPGM_OK;
}

lpgm_status_t
lpgm_circshift(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int xshift, int yshift)
{
	int i, j, ii, jj;

	if (input_signal == NULL || out_signal == NULL)
	{
		return LPGM_FAIL;
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

			out_signal[ii * ydim + jj] = input_signal[i * ydim + j];
		}
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
				fprintf(stdout, "%.1f  ", signal[index].real);
			}
			else
			{
				fprintf(stdout, "%.1f j%.1f  ", signal[index].real, signal[index].imaginary);
			}
		}
		fprintf(stdout, "\n");
	}

	fprintf(stdout, "%s(): ---- end ---- \n", __func__);

	return LPGM_OK;
}
