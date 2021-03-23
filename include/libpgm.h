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
		float imaginary;
		float real;
	} lpgm_signal_t;

	typedef struct
	{
		int w, h;
		float* data;
	} lpgm_image_t;

	typedef struct
	{
		char magic_number[2 + 1];
		char* comment;
		int max_val;
		lpgm_image_t im;
	} lpgm_t;


	lpgm_status_t lpgm_file_read(const char* file_name, lpgm_t* pgm);

	lpgm_status_t lpgm_file_write(const lpgm_t* pgm, const char* file_name);

	void lpgm_file_destroy(lpgm_t* pgm);


	float lpgm_get_2Darray_value(const float* data, int cols, int x, int y);

	void lpgm_normalize_array(float* data, int len, float new_max);


	void lpgm_image_destroy(lpgm_image_t* im);

	float lpgm_get_pixel_value(const lpgm_image_t* im, int x, int y);

	void lpgm_set_pixel_value(lpgm_image_t* im, int x, int y, float val);

	float lpgm_get_pixel_extend_value(const lpgm_image_t* im, int x, int y);

	void lpgm_print_image_pixels(const lpgm_image_t* im);

	lpgm_image_t lpgm_make_empty_image(int w, int h);

	lpgm_image_t lpgm_copy_image(const lpgm_image_t* input_im);

	void lpgm_normalize_image_data(lpgm_image_t* im, float new_max);

	lpgm_image_t lpgm_filter_image(const lpgm_image_t* im, float* box_kernel_data, int box_kernel_size);

	void lpgm_histogram_equalization(lpgm_image_t* im);

	void lpgm_obtain_negative_image(lpgm_image_t* im, float intensity_level_max);

	void lpgm_log_transformation_image(lpgm_image_t* im, float max_val, int c);

	void lpgm_power_law_transformation_image(lpgm_image_t* im, int c, float y);

	lpgm_image_t lpgm_average_filter_image(lpgm_image_t* im, int box_size);

	/*
	 * 2D Discrete Fourier Transform
	 * inverse flag is 1 -> IDFT
	 */
	lpgm_status_t lpgm_dft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* output_signal, int inverse);

	/*
	 * 1D Discrete Fourier Transform
	 * inverse flag is 1 -> IDFT
	 */
	void lpgm_dft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* output_signal, int inverse);

	// print, time or frequency domain signal imaginary and real part
	void lpgm_print_signal(const lpgm_signal_t* s, int rows, int cols);

	void lpgm_circshift(lpgm_signal_t* out, const lpgm_signal_t* in, int xdim, int ydim, int xshift, int yshift);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
