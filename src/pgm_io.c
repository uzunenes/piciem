#include "../include/libpgm.h"
#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
* read pgm file to pgm struct (P5 binary format)
*/
int pgm_read(const char* file_name, struct pgm_s* pgm)
{
    char buffer[64];
    char c;


    // ------   open file read and binary mode  -------------
    FILE* file_ptr = fopen(file_name, "rb");
    if( file_ptr == NULL )
    {
        printf("%s(): Error opening file: [%s] . \n", __func__, file_name);
        return -1;
    }
    // ------------------------------------------------------


    // -----------  read magic number   ---------------------
    const char* default_read_type = "P5";

    pgm->magic_number[0] = fgetc(file_ptr);
    pgm->magic_number[1] = fgetc(file_ptr);
    if( fgetc( file_ptr ) != '\n' ) // end line magic number
    {
        printf("%s(): magic number len must be 2 character. \n", __func__);
        fclose(file_ptr);
        return -2;
    }
    pgm->magic_number[2] = '\0';

    if( strcmp(pgm->magic_number, default_read_type) != 0 )
    {
        printf("%s(): image type: [%s], type must be: [%s] . \n", __func__, pgm->magic_number, default_read_type);
        fclose(file_ptr);
        return -3;
    }
    printf("%s(): magic number: [%s] . \n", __func__, pgm->magic_number);
    // --------------------------------------------------------


    // --------------   read comments   -----------------------
    int comment_array_location = 0;
    bool start_line_flag = 1;

    while( 1 )
    {
        c = fgetc(file_ptr);
        if( start_line_flag == 1 && c != '#' ) // end comments
        {
            if( comment_array_location > 0 )
            {
                pgm->comment[comment_array_location - 1] = '\0'; // delete ','
            }
            fseek(file_ptr, -1L, SEEK_CUR);
            break;
        }

        if( c == '\n' ) // new line
        {
            start_line_flag = 1;
            pgm->comment[comment_array_location] = ',';
            ++comment_array_location;
            continue;
        }
        else
        {
            if( start_line_flag == 1 )
            {
                start_line_flag = 0;
            }
        }

        pgm->comment[comment_array_location] = c;
        ++comment_array_location;
    }

    printf("%s(): comment: [%s] . \n", __func__, pgm->comment);
    // -------------------------------------------------------


    // --------------   read height && weight   ------------------------
    int array_location = 0;

    while( 1 )
    {
        c = fgetc(file_ptr);

        if( c == '\n' ) // end
        {
            buffer[array_location] = '\0';
            pgm->im.w = atoi(buffer); // w
            break;
        }
        else if( c == ' ' ) // height end
        {
            buffer[array_location] = '\0';
            pgm->im.h = atoi(buffer);
            array_location = 0;
        }

        buffer[array_location] = c;
        ++array_location;
    }

    printf("%s(): height: [%d] , weight: [%d] . \n", __func__, pgm->im.h, pgm->im.w);
    // -------------------------------------------------------


    // ---------------- max pixel value read ---------------
    array_location = 0;
    while( 1 )
    {
        c = fgetc(file_ptr);

        if( c == '\n' ) // end
        {
            buffer[array_location] = '\0';
            pgm->max_val = atoi(buffer);
            break;
        }

        buffer[array_location] = c;
        ++array_location;
    }

    const int max_value_L = 255;
    if( pgm->max_val != max_value_L )
    {
        printf("%s(): max_val: [%d] max_value_L: [%d] \n", __func__, pgm->max_val, max_value_L);
        return -4;
    }

    printf("%s(): max_val: [%d] \n", __func__, pgm->max_val);
    // -------------------------------------------------------


    // ---  read data   ---------------------------------------
    pgm->im.data = (unsigned char*)malloc(pgm->im.w * pgm->im.h * sizeof(unsigned char));
    unsigned char* data = pgm->im.data;

    int len = 0;
    while( 1 )
    {
        *data = (unsigned char)fgetc(file_ptr);
        ++data;
        ++len;

        if( feof(file_ptr) )
        {
            break;
        }
    }
    printf("%s(): Readed [%d] pixels from file. \n", __func__, len);
    // ---------------------------------------------------------

    fclose(file_ptr);

    return 0;
}

/*
* write pgm struct data to file (P5 binary format)
*/
int pgm_write(const struct pgm_s* pgm, const char* file_name)
{
    // -------  check type  ---------------------------------
    const char *default_read_type = "P5";
    if( strcmp(pgm->magic_number, default_read_type) != 0 )
    {
        printf("%s(): image magic number: [%s] not supporting, type must be: [%s] format . \n", __func__, pgm->magic_number, default_read_type);
        return -1;
    }
    // -------------------------------------------------------


    // ------   open file write and binary mode  -------------
    FILE* file_ptr = fopen(file_name, "wb");
    if( file_ptr == NULL )
    {
        printf("%s(): Error opening file: [%s] . \n", __func__, file_name);
        return -2;
    }
    // -------------------------------------------------------


    // ------   write file header  ---------------------------
    fprintf(file_ptr, "%s\n%s\n%d %d\n%d\n", pgm->magic_number, pgm->comment, pgm->im.h, pgm->im.w, pgm->max_val);
    // -------------------------------------------------------


    // ------   write unsigned char data  --------------------
    int rows = pgm->im.h;
    int cols = pgm->im.w;
    int data_len = rows * cols;
    for( int i = 0; i < data_len; ++i )
    {
        fputc(pgm->im.data[i], file_ptr);
    }
    // -------------------------------------------------------

    fclose(file_ptr);

    return 0;
}

/*
* destroy pgm data
*/
void pgm_destroy(struct pgm_s* pgm)
{
    image_destroy(&pgm->im);
}
