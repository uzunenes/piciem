/*
 * Gaussian Blur Example
 * 
 * Demonstrates lpgm_convolve() with a 3x3 Gaussian kernel.
 * 
 * Usage: ./gaussian_blur.out input.pgm
 * Output: output_gaussian.pgm
 */

#include <pigiem.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    lpgm_t pgm;
    lpgm_image_t blurred;
    
    /* 
     * Gaussian 3x3 kernel (sigma ≈ 0.85)
     * Formula: G(x,y) = (1/2πσ²) * e^(-(x²+y²)/2σ²)
     * 
     *   1  2  1
     *   2  4  2   ×  1/16
     *   1  2  1
     */
    const float gaussian_3x3[9] = {
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
        2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f
    };
    
    if (argc < 2)
    {
        printf("Usage: %s input.pgm\n", argv[0]);
        return 1;
    }
    
    /* Read input image */
    if (lpgm_file_read(argv[1], &pgm) != LPGM_OK)
    {
        printf("Error: Cannot read %s\n", argv[1]);
        return 1;
    }
    
    printf("Image: %dx%d\n", pgm.im.w, pgm.im.h);
    
    /* Apply Gaussian blur */
    blurred = lpgm_convolve(&pgm.im, gaussian_3x3, 3);
    
    /* Save result */
    lpgm_image_destroy(&pgm.im);
    pgm.im = blurred;
    lpgm_file_write(&pgm, "output_gaussian.pgm");
    
    printf("Saved: output_gaussian.pgm\n");
    
    lpgm_file_destroy(&pgm);
    return 0;
}
