/*
 * Simple Binary Thresholding Example
 * 
 * This example demonstrates how to use lpgm_threshold() to convert
 * a grayscale image to a binary (black and white) image.
 * 
 * Usage:
 *   ./simple_threshold.out input.pgm [threshold_value]
 * 
 * Example:
 *   ./simple_threshold.out lena.pgm 128
 * 
 * Formula: out[i] = (in[i] > threshold) ? 255 : 0
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	float threshold_value = 128.0f;  /* Default: middle gray */
	lpgm_t pgm;
	lpgm_image_t threshold_im;
	lpgm_t out_pgm;
	
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s input.pgm [threshold_value]\n", argv[0]);
		fprintf(stderr, "  threshold_value: 0-255, default = 128\n");
		return -1;
	}
	
	input_file = argv[1];
	
	if (argc >= 3)
	{
		threshold_value = (float)atof(argv[2]);
	}
	
	/* Read input image */
	fprintf(stdout, "Reading: %s\n", input_file);
	if (lpgm_file_read(input_file, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not read file %s\n", input_file);
		return -1;
	}
	
	/* Apply thresholding */
	fprintf(stdout, "Applying threshold: %.1f\n", threshold_value);
	threshold_im = lpgm_threshold(&pgm.im, threshold_value);
	
	/* Save thresholded image */
	out_pgm = pgm;
	out_pgm.im = threshold_im;
	if (lpgm_file_write(&out_pgm, "output_threshold.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_threshold.pgm\n");
		lpgm_image_destroy(&threshold_im);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	fprintf(stdout, "Saved: output_threshold.pgm\n");
	
	/* Cleanup */
	lpgm_image_destroy(&threshold_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
