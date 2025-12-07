/*
 * Brightness and Contrast Adjustment Example
 * 
 * This example demonstrates how to use lpgm_brightness() and lpgm_contrast()
 * functions to adjust the brightness and contrast of a grayscale image.
 * 
 * Usage:
 *   ./brightness_contrast.out input.pgm [brightness_delta] [contrast_factor]
 * 
 * Example:
 *   ./brightness_contrast.out lena.pgm 50 1.5
 *   (increases brightness by 50 and contrast by 1.5x)
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	float brightness_delta = 30.0f;    /* Default: increase brightness by 30 */
	float contrast_factor = 1.2f;       /* Default: increase contrast by 20% */
	lpgm_t pgm;
	lpgm_image_t bright_im, contrast_im;
	lpgm_t out_pgm;
	
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s input.pgm [brightness_delta] [contrast_factor]\n", argv[0]);
		fprintf(stderr, "  brightness_delta: value to add (-255 to 255), default = 30\n");
		fprintf(stderr, "  contrast_factor: multiplier (>1 = more contrast), default = 1.2\n");
		return -1;
	}
	
	input_file = argv[1];
	
	if (argc >= 3)
	{
		brightness_delta = (float)atof(argv[2]);
	}
	if (argc >= 4)
	{
		contrast_factor = (float)atof(argv[3]);
	}
	
	/* Read input image */
	fprintf(stdout, "Reading: %s\n", input_file);
	if (lpgm_file_read(input_file, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not read file %s\n", input_file);
		return -1;
	}
	
	/* Apply brightness adjustment */
	fprintf(stdout, "Applying brightness adjustment: delta = %.1f\n", brightness_delta);
	bright_im = lpgm_brightness(&pgm.im, brightness_delta);
	
	/* Save brightness-adjusted image */
	out_pgm = pgm;
	out_pgm.im = bright_im;
	if (lpgm_file_write(&out_pgm, "output_brightness.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_brightness.pgm\n");
	}
	else
	{
		fprintf(stdout, "Saved: output_brightness.pgm\n");
	}
	
	/* Apply contrast adjustment to original image */
	fprintf(stdout, "Applying contrast adjustment: factor = %.2f\n", contrast_factor);
	contrast_im = lpgm_contrast(&pgm.im, contrast_factor);
	
	/* Save contrast-adjusted image */
	out_pgm.im = contrast_im;
	if (lpgm_file_write(&out_pgm, "output_contrast.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_contrast.pgm\n");
	}
	else
	{
		fprintf(stdout, "Saved: output_contrast.pgm\n");
	}
	
	/* Cleanup */
	lpgm_image_destroy(&bright_im);
	lpgm_image_destroy(&contrast_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
