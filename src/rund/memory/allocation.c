#include <rund/memory/allocation.h>
#include <rund/memory/gc.h>

#include <string.h>

void* clone(const void* ptr, size_t size)
{
    void* new_ptr = gc_alloc(size);
    memcpy(new_ptr, ptr, size);
    return new_ptr;
}
