#include "../include/libpgm.h"

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

void
image_destroy(lpgm_image_t* im)
{
	free(im->data);
	im->data = NULL;
}

float
get_pixel_value(const lpgm_image_t* im, int x, int y)
{
	return im->data[x * im->w + y];
}

void
set_pixel_value(lpgm_image_t* im, int x, int y, float val)
{
	im->data[x * im->w + y] = val;
}

float
get_pixel_extend_value(const lpgm_image_t* im, int x, int y)
{
	if (x < 0 || x >= im->h || y < 0 || y >= im->w)
	{
		return 0;
	}

	return get_pixel_value(im, x, y);
}

void
print_image_pixels(const lpgm_image_t* im)
{
	int x, y;
	float val;

	for (x = 0; x < im->h; ++x)
	{
		for (y = 0; y < im->w; ++y)
		{
			val = get_pixel_value(im, x, y);
			fprintf(stdout, "%s(): (row, col), (x, y): [(%d, %d) %.1f] \n", __func__, x, y, val);
		}
	}
}

lpgm_image_t
make_empty_image(int w, int h)
{
	lpgm_image_t im;

	im.w = w;
	im.h = h;
	im.data = (float*)calloc(w * h, sizeof(float));

	return im;
}

lpgm_image_t
copy_image(const lpgm_image_t* input_im)
{
	lpgm_image_t out_im;

	out_im = make_empty_image(input_im->w, input_im->h);

	memcpy(out_im.data, input_im->data, input_im->w * input_im->h * sizeof(float));

	return out_im;
}

lpgm_image_t
border_image(const lpgm_image_t* input_im, int border_size)
{
	int i, j;
	int out_im_rows, out_im_cols;
	unsigned char val;
	lpgm_image_t out_im;

	out_im = make_empty_image(input_im->w + 2 * border_size, input_im->h + 2 * border_size);
	out_im_rows = out_im.h;
	out_im_cols = out_im.w;

	for (i = 0; i < out_im_rows; ++i)
	{
		for (j = 0; j < out_im_cols; ++j)
		{
			val = get_pixel_extend_value(input_im, i - border_size, j - border_size);
			set_pixel_value(&out_im, i, j, val);
		}
	}

	return out_im;
}

void
normalize_image_data(lpgm_image_t* im, float new_max)
{
	normalize_array(im->data, im->w * im->h, new_max);
}