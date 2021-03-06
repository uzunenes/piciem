#include "../include/libpgm.h"

#include <stdlib.h>

/*
* destroy image data
*/
void image_destroy(image_t* im)
{
    free(im->data);
    im->data = NULL;
}
