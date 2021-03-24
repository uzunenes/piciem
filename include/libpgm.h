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


	// pgmio.h
	lpgm_status_t lpgm_file_read(const char* file_name, lpgm_t* pgm);

	lpgm_status_t lpgm_file_write(const lpgm_t* pgm, const char* file_name);

	void lpgm_file_destroy(lpgm_t* pgm);


	// utils.h
	float lpgm_get_2Darray_value(const float* data, int cols, int x, int y);

	lpgm_status_t lpgm_normalize_array(float* data, int len, float new_max);


	// image.h
	lpgm_image_t lpgm_make_empty_image(int w, int h);

	lpgm_image_t lpgm_copy_image(const lpgm_image_t* input_im);

	lpgm_image_t lpgm_border_image(const lpgm_image_t* input_im, int border_size);

	void lpgm_image_destroy(lpgm_image_t* im);

	float lpgm_get_pixel_value(const lpgm_image_t* im, int x, int y);

	float lpgm_get_pixel_extend_value(const lpgm_image_t* im, int x, int y);

	void lpgm_set_pixel_value(lpgm_image_t* im, int x, int y, float val);

	void lpgm_normalize_image_data(lpgm_image_t* im, float new_max);

	lpgm_image_t lpgm_filter_image(const lpgm_image_t* im, const float* box_kernel_data, int box_kernel_size);


	// dft.h
	lpgm_signal_t* lpgm_make_empty_signal(int signal_len);

	void lpgm_destroy_signal(lpgm_signal_t* signal);

	lpgm_status_t lpgm_image_to_signal(const lpgm_image_t* im, lpgm_signal_t* out_signal);

	lpgm_status_t lpgm_dft(const lpgm_signal_t* input_signal, int signal_len, lpgm_signal_t* out_signal, int inverse);

	lpgm_status_t lpgm_dft2(const lpgm_signal_t* input_signal, int rows, int cols, lpgm_signal_t* out_signal, int inverse);

	lpgm_status_t lpgm_circshift(const lpgm_signal_t* input_signal, int xdim, int ydim, lpgm_signal_t* out_signal, int xshift, int yshift);

	lpgm_status_t lpgm_print_signal(const lpgm_signal_t* signal, int rows, int cols);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
