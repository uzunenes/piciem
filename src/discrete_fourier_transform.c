#include "../include/libpgm.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


void DFT(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* output_signal, int inverse)
{
    int w, x;
    float a, ca, sa;
	const float pi = 3.14;

	for( w = 0; w < signal_len; ++w )
	{
		output_signal[w].imaginary = 0;
		output_signal[w].real = 0;

		for( x = 0; x < signal_len; ++x )
		{
			a = -2 * pi * w * x / (float)signal_len;

			if (inverse == 1)
			{
				a = -a;
			}

			ca = cos(a);
			sa = sin(a);

			output_signal[w].real += input_signal[x].real * ca - input_signal[x].imaginary * sa;
			output_signal[w].imaginary += input_signal[x].real * sa + input_signal[x].imaginary * ca;
		}

		if( inverse == 1 )
		{
			output_signal[w].real /= signal_len;
			output_signal[w].imaginary /= signal_len;
		}
	}
}

lpgm_status_t DFT2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* output_signal, int inverse)
{
    int i, j;
    lpgm_signal_t* temp_signal;
    
    temp_signal = (lpgm_signal_t*)calloc(rows * cols * sizeof(lpgm_signal_t));
    if( temp_signal == NULL)
    {
        fprintf(stderr, "%s(): Failed to allocate memory. \n", __func__);
        return LPGM_FAIL;
    }

    // each row DFT
	for( i = 0; i < rows; ++i )
	{
		lpgm_signal_t row_signal[cols];
		for( j = 0; j < cols; ++j )
		{
			row_signal[j].imaginary = input_signal[i * cols + j].imaginary;
			row_signal[j].real = input_signal[i * cols + j].real;
		}

		lpgm_signal_t out_row_signal[cols];

		DFT(row_signal, cols, out_row_signal, inverse);

		for( j = 0; j < cols; ++j )
		{
			temp_signal[i * cols + j].imaginary = out_row_signal[j].imaginary;
			temp_signal[i * cols + j].real = out_row_signal[j].real;
		}
	}

	// each col DFT
	for( i = 0; i < cols; ++i )
	{
		lpgm_signal_t col_signal[rows];
		for( j = 0; j < rows; ++j )
		{
			col_signal[j].imaginary = temp_signal[j * cols + i].imaginary;
			col_signal[j].real = temp_signal[j * cols + i].real;
		}

		lpgm_signal_t out_col_signal[rows];

		DFT(col_signal, rows, out_col_signal, inverse);

		for( j = 0; j < rows; ++j )
		{
			output_signal[j * cols + i].imaginary = out_col_signal[j].imaginary;
			output_signal[j * cols + i].real = out_col_signal[j].real;
		}
	}

	free(temp_signal);
    
    return LPGM_OK;
}

void circshift(lpgm_signal_t* out, const lpgm_signal_t* in, int xdim, int ydim, int xshift, int yshift)
{
    int i, j, ii, jj;

	for( i = 0; i < xdim; ++i )
	{
		ii = (i + xshift) % xdim;

		if( ii < 0 )
		{
			ii = xdim + ii;
		}

		for( j = 0; j < ydim; ++j )
		{
			jj = (j + yshift) % ydim;

			if( jj < 0 )
			{
				jj = ydim + jj;
			}

			out[ii * ydim + jj] = in[i * ydim + j];
		}
	}
}

void print_signal(const lpgm_signal_t* s, int rows, int cols)
{
    int i, j, index;

	fprintf(stdout, "%s(): ---- start ---- \n", __func__);
	
    for( i = 0; i < rows; ++i)
	{
		for( j = 0; j < cols; ++j)
		{
			index = i * cols + j;
			if( fabs(s[index].imaginary) < 10e-7 )
            {
				fprintf(stdout, "%.1f  ", s[index].real);
            }
            else
            {
				fprintf(stdout, "%.1f j%.1f  ",s[index].real, s[index].imaginary);
            }
		}
		fprintf(stdout, "\n");
	}
    
	fprintf(stdout, "%s(): ---- end ---- \n", __func__);
}
