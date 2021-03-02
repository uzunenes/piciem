#ifndef PGM_API_H
#define PGM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct image_s
    {
        int w;
        int h;
        unsigned char* data;
    };

    struct pgm_s
    {
        char magic_number[2 + 1];
        char comment[2048];
        int max_val;
        struct image_s im;
    };

    void image_destroy(struct image_s* im);

    int pgm_read(const char* file_name, struct pgm_s* pgm);

    int pgm_write(const struct pgm_s* pgm, const char* file_name);

    void pgm_destroy(struct pgm_s* pgm);

#ifdef __cplusplus
}
#endif

#endif // PGM_API_H