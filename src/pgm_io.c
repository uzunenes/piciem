#include "../include/pigiem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum lpgm_e_file_formats
{
	lpgm_e_file_formats_P2_ascii,
	lpgm_e_file_formats_P5_binary
};

static enum lpgm_e_file_formats g_pgm_file_format_flag;

static void
go_new_line(FILE* file_ptr)
{
	char c;

	while (1)
	{
		c = getc(file_ptr);
		if (c == '\n')
		{
			break;
		}
	}
}

static int
read_magic_number(FILE* file_ptr, lpgm_t* pgm)
{
	pgm->magic_number[0] = fgetc(file_ptr);
	pgm->magic_number[1] = fgetc(file_ptr);
	pgm->magic_number[2] = '\0';

	if (strcmp(pgm->magic_number, "P5") == 0)
	{
		g_pgm_file_format_flag = lpgm_e_file_formats_P5_binary;
	}
	else if (strcmp(pgm->magic_number, "P2") == 0)
	{
		g_pgm_file_format_flag = lpgm_e_file_formats_P2_ascii;
	}
	else
	{
		fprintf(stderr, "%s(): Your image type: [%s], type must be: [%s] or [%s]. \n", __func__, pgm->magic_number, "P5", "P2");
		return -1;
	}

	fprintf(stdout, "%s(): Magic number: [%s]. \n", __func__, pgm->magic_number);

	go_new_line(file_ptr);

	return 0;
}

static int
read_comments(FILE* file_ptr, lpgm_t* pgm)
{
	char c;
	int i, start_line_flag;

	pgm->comment = NULL;
	i = 0;
	start_line_flag = 1;
	while (1)
	{
		c = fgetc(file_ptr);

		if (start_line_flag == 1 && c == '#') // start read comment line
		{
			start_line_flag = 0;
		}
		else if (start_line_flag == 1 && c != '#') // end comments || no comments
		{
			if (i > 0)
			{
				pgm->comment[i - 1] = '\0'; // delete ' '
			}
			fseek(file_ptr, -1L, SEEK_CUR); // -1 back, go start
			break;
		}
		else if (start_line_flag == 0 && c == '\n') // new line
		{
			start_line_flag = 1;

			pgm->comment = realloc(pgm->comment, (i + 1) * sizeof(char));
			pgm->comment[i] = ' ';
			++i;
		}
		else if (start_line_flag == 0 && c != '\n') // store comments
		{
			pgm->comment = realloc(pgm->comment, (i + 1) * sizeof(char));
			pgm->comment[i] = c;
			++i;
		}
	}

	if (i > 0)
	{
		fprintf(stdout, "%s(): Comment: [%s]. \n", __func__, pgm->comment);
	}

	return 0;
}

static int
read_height_and_weight(FILE* file_ptr, lpgm_t* pgm)
{
	char c;
	char buffer[64];
	int array_location;

	array_location = 0;
	while (1)
	{
		c = fgetc(file_ptr);

		if (c == '\n') // end
		{
			buffer[array_location] = '\0';
			pgm->im.w = atoi(buffer); // w
			break;
		}
		else if (c == ' ') // height end
		{
			buffer[array_location] = '\0';
			pgm->im.h = atoi(buffer);
			array_location = 0;
		}

		buffer[array_location] = c;
		++array_location;
	}
	fprintf(stdout, "%s(): Height: [%d] , weight: [%d]. \n", __func__, pgm->im.h, pgm->im.w);

	return 0;
}

static int
read_max_pixel_value(FILE* file_ptr, lpgm_t* pgm)
{
	char c;
	int array_location;
	char buffer[64];
	const int max_value_L = 255;

	array_location = 0;
	while (1)
	{
		c = fgetc(file_ptr);

		if (c == '\n') // end
		{
			buffer[array_location] = '\0';
			pgm->max_val = atoi(buffer);
			break;
		}

		buffer[array_location] = c;
		++array_location;
	}

	if (pgm->max_val != max_value_L)
	{
		fprintf(stderr, "%s(): Max_val: [%d] max_value_L: [%d]. \n", __func__, pgm->max_val, max_value_L);
		return -1;
	}
	fprintf(stdout, "%s(): Max_val: [%d] \n", __func__, pgm->max_val);

	return 0;
}

static int
read_pixel_data(FILE* file_ptr, lpgm_t* pgm)
{
	int i;

	pgm->im.data = (float*)calloc(pgm->im.w * pgm->im.h, sizeof(float));

	i = 0;
	while (1)
	{
		if (g_pgm_file_format_flag == lpgm_e_file_formats_P2_ascii)
		{
			fscanf(file_ptr, "%f", &pgm->im.data[i]);
		}
		else if (g_pgm_file_format_flag == lpgm_e_file_formats_P5_binary)
		{
			pgm->im.data[i] = (float)fgetc(file_ptr);
		}
		++i;

		if (feof(file_ptr))
		{
			break;
		}
	}
	fprintf(stdout, "%s(): Readed [%d] pixels from file. \n", __func__, i);

	return 0;
}

lpgm_status_t
lpgm_file_read(const char* file_name, lpgm_t* pgm)
{
	FILE* file_ptr;

	file_ptr = fopen(file_name, "r");
	if (file_ptr == NULL)
	{
		fprintf(stderr, "%s(): Error opening file: [%s]. \n", __func__, file_name);
		return LPGM_FAIL;
	}

	if (read_magic_number(file_ptr, pgm) != 0)
	{
		fprintf(stderr, "%s(): Error reading magic number from file: [%s]. \n", __func__, file_name);
		fclose(file_ptr);
		return LPGM_FAIL;
	}

	if (read_comments(file_ptr, pgm) != 0)
	{
		fprintf(stderr, "%s(): Error reading comments from file: [%s]. \n", __func__, file_name);
		fclose(file_ptr);
		return LPGM_FAIL;
	}

	if (read_height_and_weight(file_ptr, pgm) != 0)
	{
		fprintf(stderr, "%s(): Error reading height and weight from file: [%s]. \n", __func__, file_name);
		fclose(file_ptr);
		return LPGM_FAIL;
	}

	if (read_max_pixel_value(file_ptr, pgm) != 0)
	{
		fprintf(stderr, "%s(): Error reading max pixel value from file: [%s]. \n", __func__, file_name);
		fclose(file_ptr);
		return LPGM_FAIL;
	}

	if (read_pixel_data(file_ptr, pgm) != 0)
	{
		fprintf(stderr, "%s(): Error reading pixel data from file: [%s]. \n", __func__, file_name);
		fclose(file_ptr);
		return LPGM_FAIL;
	}

	// close file
	fclose(file_ptr);

	return LPGM_OK;
}

/*
 * write pgm struct data to file
 */
lpgm_status_t
lpgm_file_write(const lpgm_t* pgm, const char* file_name)
{
	FILE* file_ptr;
	int i, j;

	if (g_pgm_file_format_flag == lpgm_e_file_formats_P5_binary)
	{
		file_ptr = fopen(file_name, "wb");
	}
	else if (g_pgm_file_format_flag == lpgm_e_file_formats_P2_ascii)
	{
		file_ptr = fopen(file_name, "w");
	}

	if (file_ptr == NULL)
	{
		fprintf(stderr, "%s(): Error opening file: [%s]. \n", __func__, file_name);
		return LPGM_FAIL;
	}

	// write file header
	if (pgm->comment == NULL)
	{
		fprintf(file_ptr, "%s\n%d %d\n%d\n", pgm->magic_number, pgm->im.h, pgm->im.w, pgm->max_val);
	}
	else
	{
		fprintf(file_ptr, "%s\n#%s\n%d %d\n%d\n", pgm->magic_number, pgm->comment, pgm->im.h, pgm->im.w, pgm->max_val);
	}

	// write image data
	for (i = 0; i < pgm->im.h; ++i)
	{
		for (j = 0; j < pgm->im.w; ++j)
		{
			if (g_pgm_file_format_flag == lpgm_e_file_formats_P5_binary)
			{
				fputc((unsigned char)pgm->im.data[i * pgm->im.h + j], file_ptr);
			}
			else if (g_pgm_file_format_flag == lpgm_e_file_formats_P2_ascii)
			{
				fprintf(file_ptr, "%d ", (int)pgm->im.data[i * pgm->im.h + j]);
			}
		}
		if (g_pgm_file_format_flag == lpgm_e_file_formats_P2_ascii)
		{
			fprintf(file_ptr, "%c", '\n');
		}
	}

	// close file
	fclose(file_ptr);

	return LPGM_OK;
}

void
lpgm_file_destroy(lpgm_t* pgm)
{
	if (pgm->comment != NULL)
	{
		free(pgm->comment);
		pgm->comment = NULL;
	}
	lpgm_image_destroy(&pgm->im);
}
