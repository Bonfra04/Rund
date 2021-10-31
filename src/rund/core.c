#include <rund/core.h>

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