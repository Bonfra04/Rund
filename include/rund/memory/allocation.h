#pragma once

#include <stddef.h>
#include <stdint.h>

int* alloc_int(int value);
uint64_t* alloc_uint64_t(uint64_t value);
size_t* alloc_size_t(size_t value);
void* clone(const void* ptr, size_t size);

