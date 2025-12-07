#include <stdio.h>
#include <stdlib.h>

#include <pigiem.h>

struct point_s
{
	int x, y; // row, col
};

struct block_s
{
	struct point_s point[2];
};

void
change_image_block_pixel_value(lpgm_t* pgm, struct block_s block, float new_value)
{
	int i, j, rows, cols;
	float* data_ptr;

	rows = pgm->im.h;
	cols = pgm->im.w;
	data_ptr = pgm->im.data;

	for (i = 0; i < rows; ++i)
	{
		for (j = 0; j < cols; ++j)
		{
			if (i >= block.point[0].x && i < block.point[1].x && j >= block.point[0].y && j < block.point[1].y)
			{
				*data_ptr = new_value;
			}
			++data_ptr;
		}
	}
}

int
main(int argc, char** argv)
{
	char* input_file_name;
	const char* out_file_name = "output.pgm";
	lpgm_t pgm;
	int new_value;
	struct block_s block;

	if (argc != 2)
	{
		fprintf(stderr, "%s(): Usage: [./pgm_read_chance_block_write.out image_file.pgm] \n", __func__);
		return -1;
	}

	input_file_name = argv[1];
	fprintf(stderr, "%s(): Reading pgm file: [%s] \n", __func__, input_file_name);
	if (lpgm_file_read(input_file_name, &pgm) != LPGM_OK)
	{
		fprintf(stderr, "%s(): Exiting .. \n", __func__);
		return -1;
	}

	// change block pixel value
	block.point[0].x = 0;
	block.point[0].y = 0;
	block.point[1].x = 100;
	block.point[1].y = 100;
	new_value = 10;
	change_image_block_pixel_value(&pgm, block, new_value);

	if (lpgm_file_write(&pgm, out_file_name) != 0)
	{
		fprintf(stderr, "%s(): Exiting .. \n", __func__);
		return -1;
	}
	fprintf(stdout, "%s(): Saved pgm file: [%s] \n", __func__, out_file_name);

	lpgm_file_destroy(&pgm);

	return 0;
}
