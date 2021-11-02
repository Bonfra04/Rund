#include <rund/rendering.h>

#include <stdlib.h>

buffer_t buffer_create(size_t width, size_t height)
{
    buffer_t buffer;
    buffer.width = width;
    buffer.height = height;
    buffer.data = width == 0 || height == 0 ? NULL : calloc(width * height * sizeof(color_t), 1);

    return buffer;
}

void buffer_destroy(buffer_t* buffer)
{
    if(buffer->data == NULL)
        return;
    free(buffer->data);
    buffer->width = 0;
    buffer->height = 0;
}

color_t blend(color_t bg, color_t fg)
{
    if(fg & 0xFF000000)
        return fg;
    else
        return bg;
}
