#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* clone(const void* ptr, size_t size);

#define Val(x) ({ typeof(x) __x = (x); clone(&__x, sizeof(__x)); })
#define Ref(x) (&x)

#ifdef __cplusplus
extern "C" }
#endif
