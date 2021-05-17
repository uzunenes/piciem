#include "../include/pigiem.h"

#include <stdlib.h>

float
lpgm_get_2Darray_value(const float* data, int cols, int x, int y)
{
	if (data == NULL)
	{
		return 0;
	}

	return data[x * cols + y];
}

lpgm_status_t
lpgm_normalize_array(float* data, int len, float new_max)
{
	int i;
	float temp;
	float min = 9999999;
	float max = -999999;

	if (data == NULL)
	{
		return LPGM_FAIL;
	}

	// find max and min value
	for (i = 0; i < len; ++i)
	{
		temp = data[i];
		if (temp < min)
		{
			min = temp;
		}
		if (temp > max)
		{
			max = temp;
		}
	}

	if ((max - min) < .000000001)
	{
		min = 0.0;
		max = 1.0;
	}

	for (i = 0; i < len; ++i)
	{
		data[i] = (data[i] - min) / (max - min) * new_max;
	}

	return LPGM_OK;
}
