/*
 * Low Pass Filter Example
 * 
 * Demonstrates frequency domain filtering with FFT.
 * Applies Butterworth low pass filter for image smoothing/blurring.
 * 
 * Low pass filter passes low frequencies (smooth areas) and
 * attenuates high frequencies (edges, details, noise).
 * 
 * Usage: ./lowpass_example.out input.pgm
 */

#include <pigiem.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
    lpgm_t pgm;
    lpgm_signal_t *signal, *freq;
    int rows, cols, padded_rows, padded_cols;
    int i;
    float cutoff = 30.0f;  /* Cutoff frequency - lower = more blur */
    int order = 2;         /* Butterworth order */
    
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
    
    rows = pgm.im.h;
    cols = pgm.im.w;
    
    /* Zero-pad to power of 2 for FFT */
    padded_rows = lpgm_next_power_of_two(rows);
    padded_cols = lpgm_next_power_of_two(cols);
    
    printf("Padded: %dx%d\n", padded_cols, padded_rows);
    
    /* Allocate signals */
    signal = lpgm_make_empty_signal(padded_rows * padded_cols);
    freq = lpgm_make_empty_signal(padded_rows * padded_cols);
    
    /* Convert image to signal with zero-padding */
    for (i = 0; i < padded_rows * padded_cols; ++i)
    {
        signal[i].real = 0.0f;
        signal[i].imaginary = 0.0f;
    }
    lpgm_image_to_signal(&pgm.im, signal);
    
    /* Zero-pad signal */
    lpgm_zero_padding_signal(signal, rows, cols, signal, padded_rows, padded_cols);
    
    /* Forward FFT */
    printf("Computing FFT...\n");
    lpgm_fft2(signal, padded_rows, padded_cols, freq, 0);
    
    /* Shift zero frequency to center */
    lpgm_circshift(freq, padded_rows, padded_cols, freq, padded_rows/2, padded_cols/2);
    
    /* Apply Butterworth low pass filter */
    printf("Applying Butterworth LPF (cutoff=%.1f, order=%d)...\n", cutoff, order);
    lpgm_filter_butterworth_lowpass(freq, padded_rows, padded_cols, cutoff, order);
    
    /* Shift back */
    lpgm_circshift(freq, padded_rows, padded_cols, freq, padded_rows/2, padded_cols/2);
    
    /* Inverse FFT */
    printf("Computing inverse FFT...\n");
    lpgm_fft2(freq, padded_rows, padded_cols, signal, 1);
    
    /* Copy result back to image */
    for (i = 0; i < rows * cols; ++i)
    {
        int x = i / cols;
        int y = i % cols;
        float val = signal[x * padded_cols + y].real;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        pgm.im.data[i] = val;
    }
    
    lpgm_file_write(&pgm, "output_lowpass.pgm");
    printf("Saved: output_lowpass.pgm\n");
    
    lpgm_destroy_signal(signal);
    lpgm_destroy_signal(freq);
    lpgm_file_destroy(&pgm);
    
    return 0;
}
