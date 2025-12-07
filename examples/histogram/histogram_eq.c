/*
 * Histogram Equalization Example
 * 
 * This example demonstrates how to use lpgm_histogram_equalization() to
 * enhance the contrast of an image by redistributing pixel intensities.
 * 
 * Usage:
 *   ./histogram_eq.out input.pgm
 * 
 * Algorithm:
 *   1. Compute histogram h[i] for i = 0..255
 *   2. Compute cumulative distribution function (CDF):
 *      CDF[i] = sum(h[0..i]) / total_pixels
 *   3. Map each pixel using the equalization formula:
 *      out = round((CDF[in] - CDF_min) / (1 - CDF_min) * 255)
 * 
 * This is particularly useful for images with poor contrast, such as
 * underexposed or overexposed photographs.
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	lpgm_t pgm;
	lpgm_image_t eq_im;
	lpgm_t out_pgm;
	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s input.pgm\n", argv[0]);
		return -1;
	}
	
	input_file = argv[1];
	
	/* Read input image */
	fprintf(stdout, "Reading: %s\n", input_file);
	if (lpgm_file_read(input_file, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not read file %s\n", input_file);
		return -1;
	}
	
	/* Apply histogram equalization */
	fprintf(stdout, "Applying histogram equalization...\n");
	eq_im = lpgm_histogram_equalization(&pgm.im);
	
	/* Save equalized image */
	out_pgm = pgm;
	out_pgm.im = eq_im;
	if (lpgm_file_write(&out_pgm, "output_histogram_eq.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_histogram_eq.pgm\n");
		lpgm_image_destroy(&eq_im);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	fprintf(stdout, "Saved: output_histogram_eq.pgm\n");
	
	/* Cleanup */
	lpgm_image_destroy(&eq_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
