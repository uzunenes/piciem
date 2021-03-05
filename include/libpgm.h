#ifndef PGM_API_H
#define PGM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int w;
        int h;
        unsigned char* data;
    } image_t;

    typedef struct
    {
        char magic_number[2 + 1];
        char comment[2048];
        int max_val;
        struct image_s im;
    } pgm_t;

    void image_destroy(image_t* im);

    int pgm_read(const char* file_name, pgm_t* pgm);

    int pgm_write(const pgm_t* pgm, const char* file_name);

    void pgm_destroy(pgm_t* pgm);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H
