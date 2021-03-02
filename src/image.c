#include "../include/libpgm.h"

#include <stdlib.h>

/*
* destroy image data
*/
void image_destroy(struct image_s* im)
{
    free(im->data);
    im->data = NULL;
}
