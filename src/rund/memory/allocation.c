#include <rund/memory/allocation.h>
#include <rund/memory/gc.h>

#include <stdlib.h>
#include <string.h>

uint64_t* alloc_uint64_t(uint64_t value)
{
    uint64_t* ptr = gc_alloc(sizeof(uint64_t));
    *ptr = value;
    return ptr;
}

size_t* alloc_size_t(size_t value)
{
    size_t* ptr = gc_alloc(sizeof(size_t));
    *ptr = value;
    return ptr;
}

void* clone(const void* ptr, size_t size)
{
    void* new_ptr = gc_alloc(size);
    memcpy(new_ptr, ptr, size);
    return new_ptr;
}
