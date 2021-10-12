#pragma once

#include <stddef.h>
#include <stdint.h>

int* alloc_int(int value);
uint64_t* alloc_uint64(uint64_t value);
size_t* alloc_size(size_t value);
void* clone(const void* ptr, size_t size);

