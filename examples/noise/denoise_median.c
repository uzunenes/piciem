/*
 * Denoise with Median Filter Example
 * 
 * Demonstrates:
 * 1. Adding salt & pepper noise with lpgm_add_salt_pepper_noise()
 * 2. Removing noise with lpgm_median_filter()
 * 
 * Usage: ./denoise_median.out input.pgm
 * Output: output_noisy.pgm, output_denoised.pgm
 */

#include <pigiem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    lpgm_t pgm;
    lpgm_image_t noisy, denoised;
    
    if (argc < 2)
    {
        printf("Usage: %s input.pgm\n", argv[0]);
        return 1;
    }
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* Read input image */
    if (lpgm_file_read(argv[1], &pgm) != LPGM_OK)
    {
        printf("Error: Cannot read %s\n", argv[1]);
        return 1;
    }
    
    printf("Image: %dx%d\n", pgm.im.w, pgm.im.h);
    
    /* Add 5% salt & pepper noise */
    noisy = lpgm_add_salt_pepper_noise(&pgm.im, 0.05f);
    printf("Added 5%% salt & pepper noise\n");
    
    /* Save noisy image */
    lpgm_image_destroy(&pgm.im);
    pgm.im = noisy;
    lpgm_file_write(&pgm, "output_noisy.pgm");
    printf("Saved: output_noisy.pgm\n");
    
    /* Apply 3x3 median filter */
    denoised = lpgm_median_filter(&noisy, 3);
    printf("Applied 3x3 median filter\n");
    
    /* Save denoised image */
    lpgm_image_destroy(&pgm.im);
    pgm.im = denoised;
    lpgm_file_write(&pgm, "output_denoised.pgm");
    printf("Saved: output_denoised.pgm\n");
    
    lpgm_file_destroy(&pgm);
    return 0;
}
