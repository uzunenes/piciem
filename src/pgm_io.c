#include "../include/libpgm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


lpgm_status_t pgm_read(const char* file_name, lpgm_t* pgm)
{
    // ------   open file read and binary mode  -------------
    FILE* file_ptr = fopen(file_name, "rb");
    if( file_ptr == NULL )
    {
        fprintf(stderr, "%s(): Error opening file: [%s]. \n", __func__, file_name);
        return LPGM_FAIL;
    }
    // ------------------------------------------------------


    // -----------  read magic number   ---------------------
    pgm->magic_number[0] = fgetc(file_ptr);
    pgm->magic_number[1] = fgetc(file_ptr);
    if( fgetc( file_ptr ) != '\n' ) // end line magic number
    {
        fprintf(stderr, "%s(): Magic number len must be 2 character. \n", __func__);
        fclose(file_ptr);
        return LPGM_FAIL;
    }
    pgm->magic_number[2] = '\0';

	const char* default_read_type = "P5";
    if( strcmp(pgm->magic_number, default_read_type) != 0 )
    {
        fprintf(stderr, "%s(): Your image type: [%s], type must be: [%s]. \n", __func__, pgm->magic_number, default_read_type);
        fclose(file_ptr);
        return LPGM_FAIL;
    }
    fprintf(stdout, "%s(): Magic number: [%s]. \n", __func__, pgm->magic_number);
    // --------------------------------------------------------


    // --------------   read comments   -----------------------
    int comment_array_location = 0;
    int start_line_flag = 1;
	pgm->comment = NULL;
    while( 1 )
    {
        char c = fgetc(file_ptr);

        if( start_line_flag == 1 && c != '#' ) // end comments || no comments
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

			pgm->comment = realloc(pgm->comment, (comment_array_location + 1) * sizeof(char));
            pgm->comment[comment_array_location++] = ',';

            continue;
        }
        else
        {
            if( start_line_flag == 1 )
            {
                start_line_flag = 0;
            }
        }

		pgm->comment = realloc(pgm->comment, (comment_array_location + 1) * sizeof(char));
        pgm->comment[comment_array_location++] = c;
    }
	if( pgm->comment != NULL )
	{
    	fprintf(stdout, "%s(): Comment: [%s]. \n", __func__, pgm->comment);
	}
    // -------------------------------------------------------


    // --------------   read height && weight   ------------------------
    char buffer[64];
    int array_location = 0;

    while( 1 )
    {
        char c = fgetc(file_ptr);
        
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
    fprintf(stdout, "%s(): Height: [%d] , weight: [%d]. \n", __func__, pgm->im.h, pgm->im.w);
    // -------------------------------------------------------


    // ---------------- max pixel value read ---------------
    array_location = 0;
    while( 1 )
    {
        char c = fgetc(file_ptr);

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
        fprintf(stderr, "%s(): Max_val: [%d] max_value_L: [%d]. \n", __func__, pgm->max_val, max_value_L);
        return LPGM_FAIL;
    }
    fprintf(stdout, "%s(): Max_val: [%d] \n", __func__, pgm->max_val);
    // -------------------------------------------------------


    // ---  read data   ---------------------------------------
    pgm->im.data = (float*)calloc(pgm->im.w * pgm->im.h, sizeof(float));
    
    float* data = pgm->im.data;
    int len = 0;
    while( 1 )
    {
        *data = (float)fgetc(file_ptr);
        ++data;
        ++len;

        if( feof(file_ptr) )
        {
            break;
        }
    }
    fprintf(stdout, "%s(): Readed [%d] pixels from file. \n", __func__, len);
    // ---------------------------------------------------------

    fclose(file_ptr);

    return LPGM_OK;
}

/*
* write pgm struct data to file (P5 binary format)
*/
lpgm_status_t pgm_write(const lpgm_t* pgm, const char* file_name)
{
    // -------  check type  ---------------------------------
    const char* default_read_type = "P5";
    if( strcmp(pgm->magic_number, default_read_type) != 0 )
    {
        fprintf(stderr, "%s(): Image magic number: [%s] not supporting, type must be: [%s] format. \n", __func__, pgm->magic_number, default_read_type);
        return LPGM_FAIL;
    }
    // -------------------------------------------------------


    // ------   open file write and binary mode  -------------
    FILE* file_ptr = fopen(file_name, "wb");
    if( file_ptr == NULL )
    {
        fprintf(stderr, "%s(): Error opening file: [%s]. \n", __func__, file_name);
        return LPGM_FAIL;
    }
    // -------------------------------------------------------


    // ------   write file header  ---------------------------
	if( pgm->comment == NULL )
	{
		fprintf(file_ptr, "%s\n%d %d\n%d\n", pgm->magic_number, pgm->im.h, pgm->im.w, pgm->max_val);
	}
	else
	{
		fprintf(file_ptr, "%s\n#%s\n%d %d\n%d\n", pgm->magic_number, pgm->comment, pgm->im.h, pgm->im.w, pgm->max_val);
	}
    // -------------------------------------------------------


    // ------   write image data  ----------------------------
    for( int i = 0; i < (pgm->im.h * pgm->im.w); ++i )
    {
		unsigned char c = (unsigned char)pgm->im.data[i];
        fputc(c, file_ptr);
    }
    // -------------------------------------------------------

    fclose(file_ptr);

    return LPGM_OK;
}

void pgm_destroy(lpgm_t* pgm)
{
	if( pgm->comment != NULL ) 
	{
		free(pgm->comment);
		pgm->comment = NULL;
	}
    image_destroy(&pgm->im);
}
