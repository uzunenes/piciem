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

    lpgm_status_t pgm_read(const char* file_name, lpgm_t* pgm);

    lpgm_status_t pgm_write(const lpgm_t* pgm, const char* file_name);

    void pgm_destroy(lpgm_t* pgm);

	void image_destroy(lpgm_image_t* im);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
