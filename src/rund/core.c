#include <rund/core.h>

#include <stdlib.h>

buffer_t buffer_create(size_t width, size_t height)
{
    buffer_t buffer;
    buffer.width = width;
    buffer.height = height;
    buffer.data = malloc(width * height * sizeof(color_t));

    return buffer;
}

void buffer_destroy(buffer_t* buffer)
{
    buffer->width = 0;
    buffer->height = 0;
    free(buffer->data);
}