/*
 * Image Inversion (Negative) Example
 * 
 * This example demonstrates how to use lpgm_invert() to create
 * a negative (inverted) image.
 * 
 * Usage:
 *   ./invert.out input.pgm
 * 
 * Formula: out[i] = 255 - in[i]
 */

#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

int main(int argc, char** argv)
{
	char* input_file;
	lpgm_t pgm;
	lpgm_image_t inverted_im;
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
	
	/* Apply inversion */
	fprintf(stdout, "Applying inversion (negative image)...\n");
	inverted_im = lpgm_invert(&pgm.im);
	
	/* Save inverted image */
	out_pgm = pgm;
	out_pgm.im = inverted_im;
	if (lpgm_file_write(&out_pgm, "output_inverted.pgm") != LPGM_OK)
	{
		fprintf(stderr, "Error: Could not write output_inverted.pgm\n");
		lpgm_image_destroy(&inverted_im);
		lpgm_file_destroy(&pgm);
		return -1;
	}
	
	fprintf(stdout, "Saved: output_inverted.pgm\n");
	
	/* Cleanup */
	lpgm_image_destroy(&inverted_im);
	lpgm_file_destroy(&pgm);
	
	fprintf(stdout, "Done!\n");
	
	return 0;
}
