#include <rund/utils/allocation.h>

#include <stdlib.h>
#include <string.h>

int* alloc_int(int value)
{
    int* ptr = malloc(sizeof(int));
    *ptr = value;
    return ptr;
}

uint64_t* alloc_uint64(uint64_t value)
{
    uint64_t* ptr = malloc(sizeof(uint64_t));
    *ptr = value;
    return ptr;
}

size_t* alloc_size(size_t value)
{
    size_t* ptr = malloc(sizeof(size_t));
    *ptr = value;
    return ptr;
}

void* clone(const void* ptr, size_t size)
{
    void* new_ptr = malloc(size);
    memcpy(new_ptr, ptr, size);
    return new_ptr;
}
