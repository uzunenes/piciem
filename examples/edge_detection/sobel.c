/*
 * Sobel Edge Detection Example
 * 
 * This example demonstrates how to use lpgm_sobel() to detect edges
 * in a grayscale image using the Sobel operator.
 * 
 * Usage:
 *   ./sobel.out input.pgm
 * 
 * Sobel Kernels (3x3):
 * 
 *   Gx (horizontal gradient):    Gy (vertical gradient):
 *   [-1  0  1]                   [-1 -2 -1]
 *   [-2  0  2]                   [ 0  0  0]
 *   [-1  0  1]                   [ 1  2  1]
 * 
 * The gradient magnitude at each pixel is calculated as:
 *   G = sqrt(Gx^2 + Gy^2)
 * 
 * Edges appear as bright pixels in the output image.
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	lpgm_t pgm;
	lpgm_image_t edges_im;
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
	
	/* Apply Sobel edge detection */
	fprintf(stdout, "Applying Sobel edge detection...\n");
	edges_im = lpgm_sobel(&pgm.im);
	
	/* Save edge-detected image */
	out_pgm = pgm;
	out_pgm.im = edges_im;
	if (lpgm_file_write(&out_pgm, "output_sobel.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_sobel.pgm\n");
		lpgm_image_destroy(&edges_im);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	fprintf(stdout, "Saved: output_sobel.pgm\n");
	
	/* Cleanup */
	lpgm_image_destroy(&edges_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
