/*
 * Otsu's Automatic Thresholding Example
 * 
 * This example demonstrates how to use lpgm_otsu_threshold() to automatically
 * find the optimal threshold value for binarizing an image.
 * 
 * Otsu's method finds the threshold that minimizes intra-class variance
 * (or equivalently, maximizes inter-class variance).
 * 
 * Usage:
 *   ./otsu_threshold.out input.pgm
 * 
 * Algorithm:
 *   1. Compute histogram of the image
 *   2. For each possible threshold t (0-255):
 *      - Calculate weights w0, w1 for background and foreground
 *      - Calculate means mu0, mu1 for each class
 *      - Calculate between-class variance: sigma_B^2 = w0 * w1 * (mu0 - mu1)^2
 *   3. Select threshold that maximizes sigma_B^2
 * 
 * This is particularly useful when you don't know the optimal threshold
 * value in advance.
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	lpgm_t pgm;
	lpgm_image_t otsu_im;
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
	
	/* Apply Otsu's thresholding (automatic threshold detection) */
	fprintf(stdout, "Applying Otsu's automatic thresholding...\n");
	otsu_im = lpgm_otsu_threshold(&pgm.im);
	
	/* Save thresholded image */
	out_pgm = pgm;
	out_pgm.im = otsu_im;
	if (lpgm_file_write(&out_pgm, "output_otsu.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_otsu.pgm\n");
		lpgm_image_destroy(&otsu_im);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	fprintf(stdout, "Saved: output_otsu.pgm\n");
	
	/* Cleanup */
	lpgm_image_destroy(&otsu_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
