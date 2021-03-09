#ifndef PGM_API_H
#define PGM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		LPGM_OK = 0,
		LPGM_FAIL = -1
	} lpgm_status_t;

	
    typedef struct
    {
        char magic_number[2 + 1];
        char* comment;
        int max_val;
        lpgm_image_t im;
    } lpgm_t;

    lpgm_status_t pgm_read(const char* file_name, lpgm_t* pgm);

    lpgm_status_t pgm_write(const lpgm_t* pgm, const char* file_name);

    void pgm_destroy(lpgm_t* pgm);


	typedef struct
    {
        int w, h;
        float* data;
    } lpgm_image_t;
	
	void image_destroy(lpgm_image_t* im);
	
	float get_pixel_value(const lpgm_image_t* im, int x, int y);
	
	void set_pixel_value(lpgm_image_t* im, int x, int y, float val);
	
	float get_pixel_extend_value(const lpgm_image_t* im, int x, int y);
	
	void print_image_pixels(const lpgm_image_t* im);
	
	lpgm_image_t make_empty_image(int w, int h);
	
	lpgm_image_t copy_image(const lpgm_image_t* input_im);
	
	
	typedef struct
	{
		float imaginary;
		float real;
	} lpgm_signal_t;
	
	/*
	* 1D Discrete Fourier Transform
	* inverse flag is 1 -> IDFT 
	*/
	void DFT2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* output_signal, int inverse);

	/*
	* 2D Discrete Fourier Transform
	* inverse flag is 1 -> IDFT
	*/
	void DFT(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* output_signal, int inverse);

	// print, time or frequency domain signal imaginary and real part
	void print_signal(const lpgm_signal_t* s, int rows, int cols);

	void circshift(lpgm_signal_t* out, const lpgm_signal_t* in, int xdim, int ydim, int xshift, int yshift);
	
#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
