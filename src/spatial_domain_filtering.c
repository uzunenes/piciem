#include "../include/libpgm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
		y, cols, w, N
0,0--------------------->
|
|
|
|x, rows, h, M
|
|
|
|

*/

/*
 * applying the log transformation
 * Eq: s = clog(1 + r)
 */
void
log_transformation_image(lpgm_image_t* im, float max_val, int c)
{
	int i, data_len;
	float r;
	float* new_data;

	data_len = im->w * im->h;
	new_data = (float*)calloc(data_len, sizeof(float));

	for (i = 0; i < data_len; ++i)
	{
		r = im->data[i] / max_val; // normalize [0 1]
		new_data[i] = c * log(1.0 + r);
	}

	normalize_array(new_data, data_len, 255.0);

	set_image_data(im, new_data, data_len);

	free(new_data);
}

/*
applying power law (gamma) transformations
Eq: s = c*r^y
*/
float*
power_law_transformation_data(float* im_data, int data_len, int c, float y)
{
	float* new_data = (float*)malloc(data_len * sizeof(float));

	for (int i = 0; i < data_len; ++i)
	{
		new_data[i] = c * pow(im_data[i], y);
	}

	return new_data;
}

/*
Average filtering with a box kernel
*/
void
average_filter_pgm_image(pgm_t* im, int box_size)
{
	// create filter
	int average_box_filter_len = box_size * box_size;
	float* average_box_filter = (float*)(average_box_filter_len * sizeof(float));
	for (int i = 0; i < average_box_filter_len; ++i)
	{
		average_box_filter[i] = (float)1.0 / (float)(average_box_filter_len);
	}

	// unsigned char to float
	float* im_data = get_image_data_float(im);

	// filtered data
	float* filtered_im_data = filter2D(im_data, im->M, im->N, average_box_filter, box_size);

	int data_len = im->M * im->N;
	set_image_data(im, filtered_im_data, data_len);

	free(im_data);
	free(filtered_im_data);
	free(average_box_filter);
}

void
histogram_equalization(lpgm_image_t* im)
{
	int i, L, data_len;
	unsigned long count;
	int hist[256] = { 0 };
	int new_gray_level[256] = { 0 };
	int old_val, new_val;

	data_len = im->w * im->h;

	// orjinal histogram
	for (i = 0; i < data_len; ++i)
	{
		hist[im->data[i]] += 1;
	}

	// cum hist.
	count = 0;
	L = 255;
	for (i = 0; i < 256; ++i)
	{
		count += hist[i];
		new_gray_level[i] = round(((float)count * L) / data_len);
	}

	for (i = 0; i < data_len; ++i)
	{
		old_val = im->data[i];
		new_val = new_gray_level[old_val];
		im->data[i] = (unsigned char)new_val;
	}
}

/*
 * set unsigned char image data via float array
 */
void
set_image_data(pgm_t* im, const float* data, int data_len)
{
	if ((im->N * im->M) != data_len)
	{
		printf("%s(): image size must be equal to data len! \n", __func__);
		return;
	}

	for (int i = 0; i < data_len; ++i)
	{
		if (data[i] > 255)
		{
			im->data[i] = 255;
		}
		else if (data[i] < 0)
		{
			im->data[i] = 0;
		}
		else
		{
			im->data[i] = (unsigned char)(data[i]);
		}
	}
}

/*
 * obtain negative pgm image
 * Eq: s = (L - 1) - r
 * range [0, L - 1]
 */
void
obtain_negative_image(lpgm_image_t* im, float intensity_level_max)
{
	int i;

	for (i = 0; i < (im->w * im->h); ++i)
	{
		im->data[i] = intensity_level_max - im->data[i];
	}
}

float*
filter2D(const float* im_data, int im_rows, int im_cols, float* box_kernel_data, int box_kernel_size)
{
	int kernel_move_size = (int)floor(box_kernel_size / 2);

	float* new_data = (float*)malloc(im_rows * im_cols * sizeof(float));
	int new_data_counter = 0;

	for (int x = 0; x < im_rows; ++x)
	{
		for (int y = 0; y < im_cols; ++y)
		{
			float total = 0.0;

			for (int m = -kernel_move_size; m <= kernel_move_size; ++m)
			{
				for (int n = -kernel_move_size; n <= kernel_move_size; ++n)
				{
					int im_x = x + m;
					int im_y = y + n;

					if (!(im_x < 0 || im_x >= im_rows || im_y < 0 || im_y >= im_cols)) // border zero
					{
						int kernel_x = m + kernel_move_size;
						int kernel_y = n + kernel_move_size;

						float kernel_value = get_2Darray_value(box_kernel_data, box_kernel_size, kernel_x, kernel_y);

						total += get_2Darray_value(im_data, im_cols, im_x, im_y) * kernel_value;
					}
				}
			}

			new_data[new_data_counter] = total;
			++new_data_counter;
		}
	}

	return new_data;
}