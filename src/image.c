#include "../include/libpgm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
		y, cols, w, N
0,0 --------------------->
|
|
|
|x, rows, h, M
|
|
|
|

*/

lpgm_image_t
lpgm_make_empty_image(int w, int h)
{
	lpgm_image_t im;

	im.w = w;
	im.h = h;
	im.data = (float*)calloc(w * h, sizeof(float));

	return im;
}

lpgm_image_t
lpgm_copy_image(const lpgm_image_t* input_im)
{
	lpgm_image_t out_im;

	out_im = lpgm_make_empty_image(input_im->w, input_im->h);

	memcpy(out_im.data, input_im->data, input_im->w * input_im->h * sizeof(float));

	return out_im;
}

lpgm_image_t
lpgm_border_image(const lpgm_image_t* input_im, int border_size)
{
	int i, j;
	int out_im_rows, out_im_cols;
	unsigned char val;
	lpgm_image_t out_im;

	out_im = lpgm_make_empty_image(input_im->w + 2 * border_size, input_im->h + 2 * border_size);
	out_im_rows = out_im.h;
	out_im_cols = out_im.w;

	for (i = 0; i < out_im_rows; ++i)
	{
		for (j = 0; j < out_im_cols; ++j)
		{
			val = lpgm_get_pixel_extend_value(input_im, i - border_size, j - border_size);
			lpgm_set_pixel_value(&out_im, i, j, val);
		}
	}

	return out_im;
}

void
lpgm_image_destroy(lpgm_image_t* im)
{
	free(im->data);
	im->data = NULL;
}

float
lpgm_get_pixel_value(const lpgm_image_t* im, int x, int y)
{
	return im->data[x * im->w + y];
}

float
lpgm_get_pixel_extend_value(const lpgm_image_t* im, int x, int y)
{
	if (x < 0 || x >= im->h || y < 0 || y >= im->w)
	{
		return 0;
	}

	return lpgm_get_pixel_value(im, x, y);
}

void
lpgm_set_pixel_value(lpgm_image_t* im, int x, int y, float val)
{
	im->data[x * im->w + y] = val;
}

void
lpgm_normalize_image_data(lpgm_image_t* im, float new_max)
{
	lpgm_normalize_array(im->data, im->w * im->h, new_max);
}

lpgm_image_t
lpgm_filter_image(const lpgm_image_t* im, const float* box_kernel_data, int box_kernel_size)
{
	lpgm_image_t out_im;
	float total, kernel_value;
	int x, y, m, n, im_x, im_y, kernel_x, kernel_y, kernel_move_size;

	out_im = lpgm_make_empty_image(im->w, im->h);
	kernel_move_size = (int)floor(box_kernel_size / 2);

	for (x = 0; x < im->h; ++x)
	{
		for (y = 0; y < im->w; ++y)
		{
			total = 0.0;

			for (m = -kernel_move_size; m <= kernel_move_size; ++m)
			{
				for (n = -kernel_move_size; n <= kernel_move_size; ++n)
				{
					im_x = x + m;
					im_y = y + n;

					if (!(im_x < 0 || im_x >= im->h || im_y < 0 || im_y >= im->w)) // border zero
					{
						kernel_x = m + kernel_move_size;
						kernel_y = n + kernel_move_size;

						kernel_value = lpgm_get_2Darray_value(box_kernel_data, box_kernel_size, kernel_x, kernel_y);

						total += lpgm_get_pixel_value(im, im_x, im_y) * kernel_value;
					}
				}
			}

			lpgm_set_pixel_value(&out_im, x, y, total);
		}
	}

	return out_im;
}
