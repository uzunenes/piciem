#include "../include/pigiem.h"

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
	if (im.data == NULL)
	{
		fprintf(stderr, "%s(): Memory allocation failed.\n", __func__);
		im.w = 0;
		im.h = 0;
	}

	return im;
}

lpgm_image_t
lpgm_copy_image(const lpgm_image_t* input_im)
{
	lpgm_image_t out_im;

	if (input_im == NULL || input_im->data == NULL)
	{
		fprintf(stderr, "%s(): Invalid input image.\n", __func__);
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}

	out_im = lpgm_make_empty_image(input_im->w, input_im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}

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
	if (im == NULL)
	{
		return;
	}
	free(im->data);
	im->data = NULL;
	im->w = 0;
	im->h = 0;
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

/*
 * Clamp a value to the range [min, max]
 * 
 * Formula: out = min if val < min, max if val > max, else val
 */
static float
lpgm_clamp(float val, float min, float max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

/*
 * Brightness adjustment
 * 
 * Increases or decreases the brightness of an image by adding a constant value.
 * 
 * Parameters:
 *   im    - input image
 *   delta - value to add (-255 to 255). Positive = brighter, negative = darker
 * 
 * Formula: out[i] = clamp(in[i] + delta, 0, 255)
 */
lpgm_image_t
lpgm_brightness(const lpgm_image_t* im, float delta)
{
	int i, len;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	len = im->w * im->h;
	for (i = 0; i < len; ++i)
	{
		out_im.data[i] = lpgm_clamp(im->data[i] + delta, 0.0f, 255.0f);
	}
	
	return out_im;
}

/*
 * Contrast adjustment
 * 
 * Adjusts the contrast of an image by scaling pixel values around the midpoint (128).
 * 
 * Parameters:
 *   im     - input image
 *   factor - contrast factor. >1 increases contrast, <1 decreases, 1 = no change
 * 
 * Formula: out[i] = clamp((in[i] - 128) * factor + 128, 0, 255)
 */
lpgm_image_t
lpgm_contrast(const lpgm_image_t* im, float factor)
{
	int i, len;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	len = im->w * im->h;
	for (i = 0; i < len; ++i)
	{
		out_im.data[i] = lpgm_clamp((im->data[i] - 128.0f) * factor + 128.0f, 0.0f, 255.0f);
	}
	
	return out_im;
}

/*
 * Image inversion (negative)
 * 
 * Creates a negative image by inverting all pixel values.
 * 
 * Parameters:
 *   im - input image
 * 
 * Formula: out[i] = 255 - in[i]
 */
lpgm_image_t
lpgm_invert(const lpgm_image_t* im)
{
	int i, len;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	len = im->w * im->h;
	for (i = 0; i < len; ++i)
	{
		out_im.data[i] = 255.0f - im->data[i];
	}
	
	return out_im;
}

/*
 * Binary thresholding
 * 
 * Converts a grayscale image to binary (black and white) using a fixed threshold.
 * 
 * Parameters:
 *   im        - input image
 *   threshold - threshold value (0-255)
 * 
 * Formula: out[i] = (in[i] > threshold) ? 255 : 0
 */
lpgm_image_t
lpgm_threshold(const lpgm_image_t* im, float threshold)
{
	int i, len;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	len = im->w * im->h;
	for (i = 0; i < len; ++i)
	{
		out_im.data[i] = (im->data[i] > threshold) ? 255.0f : 0.0f;
	}
	
	return out_im;
}

/*
 * Otsu's thresholding method
 * 
 * Automatically calculates the optimal threshold value that minimizes 
 * intra-class variance (or equivalently, maximizes inter-class variance).
 * 
 * Algorithm:
 *   1. Compute histogram of the image
 *   2. For each possible threshold t (0-255):
 *      - Calculate weights w0, w1 (probability of each class)
 *      - Calculate means mu0, mu1 (mean of each class)
 *      - Calculate between-class variance: sigma_B^2 = w0 * w1 * (mu0 - mu1)^2
 *   3. Select threshold that maximizes sigma_B^2
 * 
 * Formula: sigma_B^2 = w0 * w1 * (mu0 - mu1)^2
 * 
 * Returns: Binary image thresholded with optimal value
 */
lpgm_image_t
lpgm_otsu_threshold(const lpgm_image_t* im)
{
	int i, t;
	int len;
	int histogram[256] = {0};
	float prob[256];
	float w0, w1;          /* Class probabilities */
	float mu0, mu1;        /* Class means */
	float sum0, sum1;
	float total_sum;
	float between_var;     /* Between-class variance */
	float max_var;
	int best_threshold;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	len = im->w * im->h;
	
	/* Step 1: Compute histogram */
	for (i = 0; i < len; ++i)
	{
		int pixel = (int)lpgm_clamp(im->data[i], 0.0f, 255.0f);
		histogram[pixel]++;
	}
	
	/* Compute probability distribution */
	for (i = 0; i < 256; ++i)
	{
		prob[i] = (float)histogram[i] / (float)len;
	}
	
	/* Compute total mean (sum of i * prob[i]) */
	total_sum = 0.0f;
	for (i = 0; i < 256; ++i)
	{
		total_sum += (float)i * prob[i];
	}
	
	/* Step 2: Find optimal threshold */
	max_var = 0.0f;
	best_threshold = 0;
	w0 = 0.0f;
	sum0 = 0.0f;
	
	for (t = 0; t < 256; ++t)
	{
		w0 += prob[t];           /* Weight of class 0 */
		w1 = 1.0f - w0;          /* Weight of class 1 */
		
		if (w0 == 0.0f || w1 == 0.0f)
		{
			sum0 += (float)t * prob[t];
			continue;
		}
		
		sum0 += (float)t * prob[t];
		sum1 = total_sum - sum0;
		
		mu0 = sum0 / w0;         /* Mean of class 0 */
		mu1 = sum1 / w1;         /* Mean of class 1 */
		
		/* Between-class variance: sigma_B^2 = w0 * w1 * (mu0 - mu1)^2 */
		between_var = w0 * w1 * (mu0 - mu1) * (mu0 - mu1);
		
		if (between_var > max_var)
		{
			max_var = between_var;
			best_threshold = t;
		}
	}
	
	fprintf(stdout, "Otsu threshold: %d\n", best_threshold);
	
	/* Step 3: Apply threshold */
	out_im = lpgm_threshold(im, (float)best_threshold);
	
	return out_im;
}

/*
 * Histogram equalization
 * 
 * Enhances image contrast by redistributing pixel intensities 
 * to produce a more uniform histogram.
 * 
 * Algorithm:
 *   1. Compute histogram h[i] for i = 0..255
 *   2. Compute cumulative distribution function: CDF[i] = sum(h[0..i]) / total_pixels
 *   3. Map each pixel: out[i] = round(CDF[in[i]] * 255)
 * 
 * Formula: out = round((CDF[in] - CDF_min) / (1 - CDF_min) * 255)
 *          where CDF_min is the minimum non-zero CDF value
 */
lpgm_image_t
lpgm_histogram_equalization(const lpgm_image_t* im)
{
	int i;
	int len;
	int histogram[256] = {0};
	float cdf[256];
	float cdf_min;
	int pixel;
	lpgm_image_t out_im;
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	len = im->w * im->h;
	
	/* Step 1: Compute histogram */
	for (i = 0; i < len; ++i)
	{
		pixel = (int)lpgm_clamp(im->data[i], 0.0f, 255.0f);
		histogram[pixel]++;
	}
	
	/* Step 2: Compute CDF (Cumulative Distribution Function) */
	cdf[0] = (float)histogram[0] / (float)len;
	for (i = 1; i < 256; ++i)
	{
		cdf[i] = cdf[i - 1] + (float)histogram[i] / (float)len;
	}
	
	/* Find minimum non-zero CDF value */
	cdf_min = 0.0f;
	for (i = 0; i < 256; ++i)
	{
		if (cdf[i] > 0.0f)
		{
			cdf_min = cdf[i];
			break;
		}
	}
	
	/* Step 3: Create equalized image */
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	for (i = 0; i < len; ++i)
	{
		pixel = (int)lpgm_clamp(im->data[i], 0.0f, 255.0f);
		/* Equalization formula: (cdf[v] - cdf_min) / (1 - cdf_min) * 255 */
		if (cdf_min < 1.0f)
		{
			out_im.data[i] = lpgm_clamp(((cdf[pixel] - cdf_min) / (1.0f - cdf_min)) * 255.0f, 0.0f, 255.0f);
		}
		else
		{
			out_im.data[i] = im->data[i];
		}
	}
	
	return out_im;
}

/*
 * Sobel edge detection
 * 
 * Detects edges in an image using the Sobel operator, which calculates
 * the gradient magnitude at each pixel.
 * 
 * Sobel kernels (3x3):
 *   Gx (horizontal):     Gy (vertical):
 *   [-1  0  1]           [-1 -2 -1]
 *   [-2  0  2]           [ 0  0  0]
 *   [-1  0  1]           [ 1  2  1]
 * 
 * Gradient magnitude: G = sqrt(Gx^2 + Gy^2)
 * 
 * Returns: Edge magnitude image (higher values = stronger edges)
 */
lpgm_image_t
lpgm_sobel(const lpgm_image_t* im)
{
	int x, y;
	int i, j;
	float gx, gy, magnitude;
	float pixel;
	lpgm_image_t out_im;
	
	/* Sobel kernels */
	const float sobel_gx[9] = {
		-1.0f, 0.0f, 1.0f,
		-2.0f, 0.0f, 2.0f,
		-1.0f, 0.0f, 1.0f
	};
	const float sobel_gy[9] = {
		-1.0f, -2.0f, -1.0f,
		 0.0f,  0.0f,  0.0f,
		 1.0f,  2.0f,  1.0f
	};
	
	if (im == NULL || im->data == NULL)
	{
		out_im.w = 0;
		out_im.h = 0;
		out_im.data = NULL;
		return out_im;
	}
	
	out_im = lpgm_make_empty_image(im->w, im->h);
	if (out_im.data == NULL)
	{
		return out_im;
	}
	
	/* Apply Sobel operator to each pixel */
	for (x = 0; x < im->h; ++x)
	{
		for (y = 0; y < im->w; ++y)
		{
			gx = 0.0f;
			gy = 0.0f;
			
			/* Convolve with 3x3 Sobel kernels */
			for (i = -1; i <= 1; ++i)
			{
				for (j = -1; j <= 1; ++j)
				{
					/* Use lpgm_get_pixel_extend_value for border handling (returns 0) */
					pixel = lpgm_get_pixel_extend_value(im, x + i, y + j);
					
					/* Kernel index: (i+1)*3 + (j+1) */
					gx += pixel * sobel_gx[(i + 1) * 3 + (j + 1)];
					gy += pixel * sobel_gy[(i + 1) * 3 + (j + 1)];
				}
			}
			
			/* Gradient magnitude: G = sqrt(Gx^2 + Gy^2) */
			magnitude = sqrtf(gx * gx + gy * gy);
			
			lpgm_set_pixel_value(&out_im, x, y, lpgm_clamp(magnitude, 0.0f, 255.0f));
		}
	}
	
	return out_im;
}

