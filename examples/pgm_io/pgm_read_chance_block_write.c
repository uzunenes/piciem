#include <stdio.h>
#include <stdlib.h>

#include <libpgm.h>

// ---------------------------------------------------------------
struct point_s
{
    int x, y; // row, col
};

struct block_s
{
    struct point_s point[2];
};

void change_image_block_pixel_value(struct pgm_s* pgm, struct block_s block, int new_value);
// ---------------------------------------------------------------


int main(int argc, char **argv)
{
    // ------  chechk input parameter    ---------
    if (argc != 2)
    {
        printf("%s(): Usage: [./pgm_read_chance_block_write.out image_file.pgm] \n", __func__);
        return -1;
    }
    // -------------------------------------------


    // ------   read file   ----------------------
    const char *file_name = argv[1];
    printf("%s(): Reading pgm file: [%s] \n", __func__, file_name);

    struct pgm_s pgm;
    if( pgm_read(file_name, &pgm) != 0 )
    {
        printf("%s(): Exiting .. \n", __func__);
        return -1;;
    }
    // -------------------------------------------


    // ------    change block pixel value   ------
    const int new_value = 10;
    struct block_s block;
    block.point[0].x = 0;
    block.point[0].y = 0;
    block.point[1].x = 100;
    block.point[1].y = 100;

    change_image_block_pixel_value(&pgm, block, new_value);
    // -------------------------------------------


    // -------  write_image ---------------------
    const char* out_file_name = "output.pgm";
    if( pgm_write(&pgm, out_file_name) != 0 )
    {
        printf("%s(): Exiting .. \n", __func__);
        return -1;;
    }
    printf("%s(): Saved pgm file: [%s]\n", __func__, out_file_name);
    // -------------------------------------------

    
    pgm_destroy(&pgm);

    return 0;
}

void change_image_block_pixel_value(struct pgm_s* pgm, struct block_s block, int new_value)
{
    int rows = pgm->im.w;
    int cols = pgm->im.h;
    unsigned char* data_ptr = pgm->im.data;

    for( int i = 0; i < rows; ++i )
    {
        for( int j = 0; j < cols; ++j )
        {
            if( i >= block.point[0].x && i < block.point[1].x && j >= block.point[0].y && j < block.point[1].y )
            {
                *data_ptr = new_value;
            }
            ++data_ptr;
        }
    }
}