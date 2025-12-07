/*
 * Morphology Example
 * 
 * Demonstrates erosion, dilation, opening, closing.
 * 
 * Usage: ./morphology_example.out input.pgm
 */

#include <pigiem.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    lpgm_t pgm;
    lpgm_image_t binary, eroded, dilated, opened, closed;
    
    if (argc < 2)
    {
        printf("Usage: %s input.pgm\n", argv[0]);
        return 1;
    }
    
    if (lpgm_file_read(argv[1], &pgm) != LPGM_OK)
    {
        printf("Error: Cannot read %s\n", argv[1]);
        return 1;
    }
    
    printf("Image: %dx%d\n", pgm.im.w, pgm.im.h);
    
    /* First threshold to binary */
    binary = lpgm_otsu_threshold(&pgm.im);
    
    /* Save binary */
    lpgm_image_destroy(&pgm.im);
    pgm.im = binary;
    lpgm_file_write(&pgm, "output_binary.pgm");
    printf("Saved: output_binary.pgm\n");
    
    /* Erosion */
    eroded = lpgm_erode(&binary, 3);
    pgm.im = eroded;
    lpgm_file_write(&pgm, "output_eroded.pgm");
    printf("Saved: output_eroded.pgm\n");
    
    /* Dilation */
    dilated = lpgm_dilate(&binary, 3);
    lpgm_image_destroy(&pgm.im);
    pgm.im = dilated;
    lpgm_file_write(&pgm, "output_dilated.pgm");
    printf("Saved: output_dilated.pgm\n");
    
    /* Opening */
    opened = lpgm_opening(&binary, 3);
    lpgm_image_destroy(&pgm.im);
    pgm.im = opened;
    lpgm_file_write(&pgm, "output_opening.pgm");
    printf("Saved: output_opening.pgm\n");
    
    /* Closing */
    closed = lpgm_closing(&binary, 3);
    lpgm_image_destroy(&pgm.im);
    pgm.im = closed;
    lpgm_file_write(&pgm, "output_closing.pgm");
    printf("Saved: output_closing.pgm\n");
    
    lpgm_image_destroy(&binary);
    lpgm_file_destroy(&pgm);
    
    return 0;
}
