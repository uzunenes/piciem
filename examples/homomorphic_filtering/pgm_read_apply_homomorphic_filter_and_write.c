#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libpgm.h>

lpgm_signal_t*
gaussian_high_pass_filter(int rows, int cols, double D0, double yL, double yH, double c)
{
	int u, v;
	float square_D0, value, square_Duv;
	lpgm_signal_t* filter;

	filter = lpgm_make_empty_signal(rows * cols);

	square_D0 = pow(D0, 2);
	for (u = 0; u < rows; ++u)
	{
		for (v = 0; v < cols; ++v)
		{
			// (u - rows/2)^2 + (v - cols/2)
			square_Duv = pow((float)u - ((float)rows / 2.0), 2) + pow((float)v - ((float)cols / 2.0), 2);

			value = 1.0 - exp(-1 * c * square_Duv / square_D0);

			filter[u * cols + v].real = (yH - yL) * value + yL;
		}
	}

	return filter;
}

void
histogram_equalization(lpgm_image_t* im)
{
	const float L = 255.0;
	unsigned long int count;
	int hist[256], new_gray_level[256];
	int i, data_len;
	float old_val, new_val;

	data_len = im->w * im->h;
	for (i = 0; i < 256; ++i)
	{
		hist[i] = 0;
		new_gray_level[i] = 0;
	}

	// orijinal histogram
	for (i = 0; i < data_len; ++i)
	{
		hist[(int)im->data[i]] += 1;
	}

	// cum hist.
	count = 0;
	for (i = 0; i < 256; ++i)
	{
		count += hist[i];
		new_gray_level[i] = ((float)count * L) / data_len;
	}

	for (i = 0; i < data_len; ++i)
	{
		old_val = im->data[i];
		new_val = new_gray_level[(int)old_val];
		im->data[i] = new_val;
	}

}

int
homomorphic_filtering(lpgm_image_t* im, double D0, double yLow, double yHigh, double c)
{
	time_t time_start, time_end;
	int i, rows, cols, inverse, x_shift, y_shift;
	float L, r, ac_bd, ad_bc;
	lpgm_signal_t *im_signal, *im_dft_signal, *im_dft_shifted_signal, *dft_filter;

	fprintf(stdout, "%s(): Processing .. \n", __func__);
	time_start = time(0);

	rows = im->h;
	cols = im->w;

	im_signal = lpgm_make_empty_signal(im->w * im->h);
	lpgm_image_to_signal(im, im_signal);

	fprintf(stdout, "%s(): Log \n", __func__);
	// log
	L = 255.0;
	for (i = 0; i < (rows * cols); ++i)
	{
		r = im_signal[i].real / L;
		im_signal[i].real = log(1.0 + r);
	}

	// dft
	fprintf(stdout, "%s(): dft \n", __func__);
	im_dft_signal = lpgm_make_empty_signal(im->w * im->h);
	inverse = 0;
	lpgm_dft2(im_signal, rows, cols, im_dft_signal, inverse);

	// dft shift
	fprintf(stdout, "%s(): circshift \n", __func__);
	im_dft_shifted_signal = lpgm_make_empty_signal(im->w * im->h);
	x_shift = rows / 2;
	y_shift = cols / 2;
	lpgm_circshift(im_dft_signal, rows, cols, im_dft_shifted_signal, x_shift, y_shift);

	// gaussian high pass filter create
	fprintf(stdout, "%s(): gaussian high pass filter \n", __func__);
	dft_filter = gaussian_high_pass_filter(rows, cols, D0, yLow, yHigh, c);

	// F[u,v] ./ H[u,v]
	for (i = 0; i < rows * cols; ++i)
	{
		// (a+jb) * (c+jd) = (ac-bd) + j(ad+bc)

		// ac-bd
		ac_bd = im_dft_shifted_signal[i].real * dft_filter[i].real - im_dft_shifted_signal[i].imaginary * dft_filter[i].imaginary;
		// ad+bc
		ad_bc = im_dft_shifted_signal[i].real * dft_filter[i].imaginary + im_dft_shifted_signal[i].imaginary * dft_filter[i].real;

		im_dft_shifted_signal[i].imaginary = ad_bc;
		im_dft_shifted_signal[i].real = ac_bd;
	}

	fprintf(stdout, "%s(): circshift \n", __func__);
	lpgm_circshift(im_dft_shifted_signal, rows, cols, im_dft_signal, x_shift, y_shift);

	// IDFT
	fprintf(stdout, "%s(): idft \n", __func__);
	inverse = 1;
	lpgm_dft2(im_dft_signal, rows, cols, im_signal, inverse);

	fprintf(stdout, "%s(): abs \n", __func__);
	for (i = 0; i < rows * cols; ++i)
	{
		im->data[i] = sqrt(pow(im_signal[i].real, 2) + pow(im_signal[i].imaginary, 2)); // abs
	}

	// exp
	fprintf(stdout, "%s(): exp %f\n", __func__, im->data[0]);
	for (i = 0; i < rows * cols; ++i)
	{
		im->data[i] = exp(im->data[i]);
		im->data[i] = im->data[i] - 1;
	}

	fprintf(stdout, "%s(): normalize %f\n", __func__, im->data[0]);
	lpgm_normalize_image_data(im, 255.0);

	fprintf(stdout, "%s(): histeq %f\n", __func__, im->data[0]);
	histogram_equalization(im);

	lpgm_destroy_signal(im_signal);
	lpgm_destroy_signal(im_dft_signal);
	lpgm_destroy_signal(im_dft_shifted_signal);
	lpgm_destroy_signal(dft_filter);

	time_end = time(0);
	fprintf(stdout, "%s(): It took [%.2f] second(s). \n", __func__, difftime(time_end, time_start));

	return 0;
}

int
read_image_apply_homomorphic_filter_and_save(const char* input_pgm_file_name, const char* output_pgm_file_name, double D0, double yLow, double yHigh, double c)
{
	lpgm_t pgm;

	// Read input image file
	fprintf(stdout, "%s(): Reading input pgm image file: [%s] \n", __func__, input_pgm_file_name);
	if (lpgm_file_read(input_pgm_file_name, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "%s(): Exiting .. \n", __func__);
		return -1;
	}

	homomorphic_filtering(&pgm.im, D0, yLow, yHigh, c);

	// write_image
	if (lpgm_file_write(&pgm, output_pgm_file_name) != 0)
	{
		fprintf(stderr, "%s(): Exiting .. \n", __func__);
		return -1;
	}
	fprintf(stdout, "%s(): Saved pgm file: [%s]\n", __func__, output_pgm_file_name);

	// destroy image memory block
	lpgm_file_destroy(&pgm);

	return 0;
}

int
main(int argc, char** argv)
{
	// homomorphic_filtering
	if ((argc >= 2 && argc == 14) && (strcmp(argv[1], "-homomorphic_filtering") == 0))
	{
		const char* input_pgm_file_name = NULL;
		const char* output_pgm_file_name = NULL;
		double D0 = 0;
		double yLow = 0;
		double yHigh = 0;
		double c = 0;

		if (strcmp(argv[2], "-input") == 0 && strcmp(argv[4], "-output") == 0 && strcmp(argv[6], "-D0") == 0 && strcmp(argv[8], "-yLow") == 0 && strcmp(argv[10], "-yHigh") == 0 && strcmp(argv[12], "-c") == 0)
		{
			input_pgm_file_name = argv[3];
			output_pgm_file_name = argv[5];
			D0 = atof(argv[7]);
			yLow = atof(argv[9]);
			yHigh = atof(argv[11]);
			c = atof(argv[13]);
		}
		else
		{
			printf("%s(): Usage: [./homework4.exe -homomorphic_filtering -input input_pgm_file_name.pgm -output output_pgm_file_name.pgm -D0 40 -yLow 0.5 -yHigh 2.0 -c 1] \n", __func__);
			return -1;
		}

		printf("%s(): parsed input parameter: type: [-homomorphic_filtering] input file: [%s] output file: [%s] D0: [%.2f] yLow[%.2f] yHigh: [%.2f] c: [%.2f]\n", __func__, input_pgm_file_name, output_pgm_file_name, D0, yLow, yHigh, c);

		read_image_apply_homomorphic_filter_and_save(input_pgm_file_name, output_pgm_file_name, D0, yLow, yHigh, c);
	}
	else
	{
		printf("%s(): Usage: [./homework4.exe -homomorphic_filtering -input input_pgm_file_name.pgm -output output_pgm_file_name.pgm -D0 40 -yLow 0.5 -yHigh 2.0 -c 1] \n", __func__);
		return -1;
	}

	return 0;
}
