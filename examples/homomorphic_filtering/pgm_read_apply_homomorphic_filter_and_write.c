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
	float D_uv, H_hp;
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
			// Distance from center: D(u,v) = sqrt((u - rows/2)^2 + (v - cols/2)^2)
			D_uv = sqrt(pow((float)u - ((float)rows / 2.0), 2) + pow((float)v - ((float)cols / 2.0), 2));

			// Butterworth High-Pass: H_hp(u,v) = 1 / (1 + (D0/D(u,v))^(2n))
			// Avoid division by zero at DC component
			if (D_uv < 0.0001)
			{
				H_hp = 0.0;
			}
			else
			{
				H_hp = 1.0 / (1.0 + pow(D0 / D_uv, 2.0 * n));
			}

			// Homomorphic filter: H(u,v) = (yH - yL) * H_hp(u,v) + yL
			filter[u * cols + v].real = (yH - yL) * H_hp + yL;
			filter[u * cols + v].imaginary = 0.0;
		}
	}

	return filter;
}

/*
 * applying the log transformation
 * Eq: s = c * ln(1 + r)
 */
void
lpgm_log_transformation_image(lpgm_image_t* im, int c)
{
	int i, data_len;

	data_len = im->w * im->h;

	for (i = 0; i < data_len; ++i)
	{
		im->data[i] = c * log(1.0 + im->data[i]);
	}
}

/*
 * applying the Exponential transformation (inverse of log)
 * Eq: s = exp(p/c) - 1
 */
void
lpgm_exponential_transformation_image(lpgm_image_t* im, int c)
{
	int i, data_len;

	data_len = im->w * im->h;

	for (i = 0; i < data_len; ++i)
	{
		im->data[i] = exp(im->data[i] / c) - 1.0;
	}
}

int
homomorphic_filtering(lpgm_image_t* im, float D0, float yLow, float yHigh, float order)
{
	time_t time_start, time_end;
	int i, rows, cols, im_data_len, inverse;
	float ac_bd, ad_bc;
	lpgm_signal_t *im_signal, *im_dft_signal, *im_dft_shifted, *im_dft_filtered_signal, *im_dft_unshifted, *dft_butterworth_hpfilter;

	fprintf(stdout, "%s(): Processing .. \n", __func__);
	time_start = time(0);

	// init variables
	rows = im->h;
	cols = im->w;
	im_data_len = rows * cols;
	im_signal = lpgm_make_empty_signal(im_data_len);
	im_dft_signal = lpgm_make_empty_signal(im_data_len);
	im_dft_shifted = lpgm_make_empty_signal(im_data_len);
	im_dft_filtered_signal = lpgm_make_empty_signal(im_data_len);
	im_dft_unshifted = lpgm_make_empty_signal(im_data_len);

	dft_butterworth_hpfilter = get_butterworth_high_pass_filter(rows, cols, D0, yLow, yHigh, order);

	// Step 1: Log transformation - separates illumination and reflectance
	fprintf(stdout, "%s(): Log transformation .. \n", __func__);
	lpgm_log_transformation_image(im, 1);
	lpgm_image_to_signal(im, im_signal);

	// Step 2: DFT
	fprintf(stdout, "%s(): dft .. \n", __func__);
	inverse = 0;
	lpgm_dft2(im_signal, rows, cols, im_dft_signal, inverse);

	// Step 3: Shift DFT to center (fftshift) - DC at center
	fprintf(stdout, "%s(): shift to center .. \n", __func__);
	lpgm_circshift(im_dft_signal, rows, cols, im_dft_shifted, rows / 2, cols / 2);

	// Step 4: Apply homomorphic filter H(u,v)
	fprintf(stdout, "%s(): filtering -> F[u,v] * H[u,v] .. \n", __func__);
	for (i = 0; i < im_data_len; ++i)
	{
		// Complex multiplication: (a+jb) * (c+jd) = (ac-bd) + j(ad+bc)
		// Filter is real-valued, so d=0: (a+jb) * c = ac + jbc
		ac_bd = im_dft_shifted[i].real * dft_butterworth_hpfilter[i].real;
		ad_bc = im_dft_shifted[i].imaginary * dft_butterworth_hpfilter[i].real;

		im_dft_filtered_signal[i].real = ac_bd;
		im_dft_filtered_signal[i].imaginary = ad_bc;
	}

	// Step 5: Shift back (ifftshift)
	fprintf(stdout, "%s(): shift back .. \n", __func__);
	lpgm_circshift(im_dft_filtered_signal, rows, cols, im_dft_unshifted, rows / 2, cols / 2);

	// Step 6: Inverse DFT
	fprintf(stdout, "%s(): idft .. \n", __func__);
	inverse = 1;
	lpgm_dft2(im_dft_unshifted, rows, cols, im_signal, inverse);

	// Debug: check values after IDFT
	{
		float min_r = im_signal[0].real, max_r = im_signal[0].real;
		float min_i = im_signal[0].imaginary, max_i = im_signal[0].imaginary;
		for (i = 0; i < im_data_len; ++i) {
			if (im_signal[i].real < min_r) min_r = im_signal[i].real;
			if (im_signal[i].real > max_r) max_r = im_signal[i].real;
			if (im_signal[i].imaginary < min_i) min_i = im_signal[i].imaginary;
			if (im_signal[i].imaginary > max_i) max_i = im_signal[i].imaginary;
		}
		fprintf(stdout, "%s(): After IDFT - real: [%.4f, %.4f], imag: [%.4f, %.4f]\n", __func__, min_r, max_r, min_i, max_i);
	}

	// Step 7: Take real part and shift to positive range
	// Shift all values so minimum becomes 0 (preserves relative differences)
	fprintf(stdout, "%s(): taking real part and shifting to positive range\n", __func__);
	{
		float min_val = im_signal[0].real;
		for (i = 0; i < im_data_len; ++i)
		{
			if (im_signal[i].real < min_val) min_val = im_signal[i].real;
		}
		fprintf(stdout, "%s(): Shifting by %.4f to make all values positive\n", __func__, -min_val);
		for (i = 0; i < im_data_len; ++i)
		{
			im->data[i] = im_signal[i].real - min_val;
		}
	}

	// Step 8: Exponential - inverse of log transformation
	fprintf(stdout, "%s(): Exponential (inverse log) .. \n", __func__);
	lpgm_exponential_transformation_image(im, 1);

	// Debug: check values after exp
	{
		float min_v = im->data[0], max_v = im->data[0];
		for (i = 0; i < im_data_len; ++i) {
			if (im->data[i] < min_v) min_v = im->data[i];
			if (im->data[i] > max_v) max_v = im->data[i];
		}
		fprintf(stdout, "%s(): After exp - [%.4f, %.4f]\n", __func__, min_v, max_v);
	}

	// Step 9: Min-Max Normalization to display range [0, 255]
	fprintf(stdout, "%s(): Normalize image data [0 - 255] .. \n", __func__);
	lpgm_normalize_image_data(im, 255.0);

	// Step 10: Histogram Equalization for contrast enhancement
	fprintf(stdout, "%s(): Histogram equalization for contrast enhancement .. \n", __func__);
	{
		lpgm_image_t enhanced = lpgm_histogram_equalization(im);
		// Copy enhanced data back to original image
		for (i = 0; i < im_data_len; ++i)
		{
			im->data[i] = enhanced.data[i];
		}
		lpgm_image_destroy(&enhanced);
	}

	lpgm_destroy_signal(im_signal);
	lpgm_destroy_signal(im_dft_signal);
	lpgm_destroy_signal(im_dft_shifted);
	lpgm_destroy_signal(im_dft_filtered_signal);
	lpgm_destroy_signal(im_dft_unshifted);
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