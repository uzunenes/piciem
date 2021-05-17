#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pigiem.h>

lpgm_signal_t*
get_butterworth_high_pass_filter(int rows, int cols, float D0, float yL, float yH, float n)
{
	int u, v;
	float temp;
	lpgm_signal_t* filter;

	filter = lpgm_make_empty_signal(rows * cols);
	if (filter == NULL)
	{
		return NULL;
	}

	for (u = 0; u < rows; ++u)
	{
		for (v = 0; v < cols; ++v)
		{
			// (u - rows/2)^2 + (v - cols/2)^0.5
			temp = sqrt(pow((float)u - ((float)rows / 2.0), 2) + pow((float)v - ((float)cols / 2.0), 2));

			temp = 1.0 / (1.0 + pow(D0 / temp, 2.0 * n));

			filter[u * cols + v].real = 1.0 - ((yH - yL) * temp + yL);
		}
	}

	return filter;
}

/*
 * applying the log transformation
 * Eq: s = clog2(1 + r)
 */
void
lpgm_log_transformation_image(lpgm_image_t* im, int c)
{
	int i, data_len;
	float* new_data;

	data_len = im->w * im->h;
	new_data = (float*)calloc(data_len, sizeof(float));

	for (i = 0; i < data_len; ++i)
	{
		new_data[i] = c * log2(1.0 + im->data[i]);
	}

	memcpy(im->data, new_data, data_len * sizeof(float));

	free(new_data);
}

/*
 * applying the Exponential transformation
 * Eq: s = c * exp(p)
 */
void
lpgm_exponential_transformation_image(lpgm_image_t* im, int c)
{
	int i, data_len;

	data_len = im->w * im->h;

	for (i = 0; i < data_len; ++i)
	{
		im->data[i] = c * exp(im->data[i]);
	}
}

int
homomorphic_filtering(lpgm_image_t* im, float D0, float yLow, float yHigh, float c)
{
	time_t time_start, time_end;
	int i, rows, cols, im_data_len, inverse;
	float ac_bd, ad_bc;
	lpgm_signal_t *im_signal, *im_dft_signal, *im_dft_filtered_signal, *dft_butterworth_hpfilter;

	fprintf(stdout, "%s(): Processing .. \n", __func__);
	time_start = time(0);

	// init variables
	rows = im->h;
	cols = im->w;
	im_data_len = rows * cols;
	im_signal = lpgm_make_empty_signal(im_data_len);
	im_dft_signal = lpgm_make_empty_signal(im_data_len);
	im_dft_filtered_signal = lpgm_make_empty_signal(im_data_len);

	dft_butterworth_hpfilter = get_butterworth_high_pass_filter(rows, cols, D0, yLow, yHigh, c);

	fprintf(stdout, "%s(): Log transformation .. \n", __func__);
	lpgm_log_transformation_image(im, 1);
	lpgm_image_to_signal(im, im_signal);

	fprintf(stdout, "%s(): dft .. \n", __func__);
	inverse = 0;
	lpgm_dft2(im_signal, rows, cols, im_dft_signal, inverse);

	fprintf(stdout, "%s(): filtering -> F[u,v] ./ H[u,v] .. \n", __func__);
	for (i = 0; i < im_data_len; ++i)
	{
		// (a+jb) * (c+jd) = (ac-bd) + j(ad+bc)

		// ac-bd
		ac_bd = im_dft_signal[i].real * dft_butterworth_hpfilter[i].real - im_dft_signal[i].imaginary * dft_butterworth_hpfilter[i].imaginary;
		// ad+bc
		ad_bc = im_dft_signal[i].real * dft_butterworth_hpfilter[i].imaginary + im_dft_signal[i].imaginary * dft_butterworth_hpfilter[i].real;

		im_dft_filtered_signal[i].imaginary = ad_bc;
		im_dft_filtered_signal[i].real = ac_bd;
	}

	fprintf(stdout, "%s(): idft .. \n", __func__);
	inverse = 1;
	lpgm_dft2(im_dft_filtered_signal, rows, cols, im_signal, inverse);

	fprintf(stdout, "%s(): abs \n", __func__);
	for (i = 0; i < im_data_len; ++i)
	{
		im->data[i] = sqrt(pow(im_signal[i].real, 2) + pow(im_signal[i].imaginary, 2)); // abs
	}

	// exp
	fprintf(stdout, "%s(): Exponential (inverse log) .. \n", __func__);
	lpgm_exponential_transformation_image(im, 1);

	fprintf(stdout, "%s(): Normalize image data [0 - 255] .. \n", __func__);
	lpgm_normalize_image_data(im, 255.0);

	lpgm_destroy_signal(im_signal);
	lpgm_destroy_signal(im_dft_signal);
	lpgm_destroy_signal(im_dft_filtered_signal);
	lpgm_destroy_signal(dft_butterworth_hpfilter);

	time_end = time(0);
	fprintf(stdout, "%s(): It took [%.2f] second(s). \n", __func__, difftime(time_end, time_start));

	return 0;
}

int
read_image_apply_homomorphic_filter_and_save(const char* input_pgm_file_name, const char* output_pgm_file_name, float D0, float yLow, float yHigh, float c)
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
	if (lpgm_file_write(&pgm, output_pgm_file_name) != LPGM_OK)
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
	char *input_pgm_file_name, *output_pgm_file_name;
	float D0, yLow, yHigh, c;

	// homomorphic_filtering
	if (argc == 13)
	{
		if (strcmp(argv[1], "-input") == 0 && strcmp(argv[3], "-output") == 0 && strcmp(argv[5], "-D0") == 0 && strcmp(argv[7], "-yLow") == 0 && strcmp(argv[9], "-yHigh") == 0 && strcmp(argv[11], "-c") == 0)
		{
			input_pgm_file_name = argv[2];
			output_pgm_file_name = argv[4];
			D0 = atof(argv[6]);
			yLow = atof(argv[8]);
			yHigh = atof(argv[10]);
			c = atof(argv[12]);
		}
		else
		{
			fprintf(stderr, "%s(): Usage: [./pgm_read_apply_homomorphic_filter_and_write.out -input input_example1.pgm -output output_pgm_file_name.pgm -D0 10 -yLow 0.099 -yHigh 1.01 -c 2] \n", __func__);
			return -1;
		}

		fprintf(stdout, "%s(): parsed input parameter: input file: [%s] output file: [%s] D0: [%.2f] yLow[%.2f] yHigh: [%.2f] c: [%.2f]\n", __func__, input_pgm_file_name, output_pgm_file_name, D0, yLow, yHigh, c);

		read_image_apply_homomorphic_filter_and_save(input_pgm_file_name, output_pgm_file_name, D0, yLow, yHigh, c);
	}
	else
	{
		fprintf(stderr, "%s(): Usage: [./pgm_read_apply_homomorphic_filter_and_write.out -input input_example1.pgm -output output_pgm_file_name.pgm -D0 10 -yLow 0.099 -yHigh 1.01 -c 2] \n", __func__);
		return -1;
	}

	return 0;
}