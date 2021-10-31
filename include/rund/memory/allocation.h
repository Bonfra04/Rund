#pragma once

#include <stddef.h>

void* clone(const void* ptr, size_t size);

#define Val(x) ({ typeof(x) __x = (x); clone(&__x, sizeof(__x)); })

