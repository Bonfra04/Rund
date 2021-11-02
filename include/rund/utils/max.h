#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t max_uint64_t(uint64_t a, uint64_t b);
uint64_t min_uint64_t(uint64_t a, uint64_t b);

#define max(a, b) ((_Generic((a), \
    uint64_t: max_uint64_t  \
))(a,b))

#define min(a, b) ((_Generic((a), \
    uint64_t: min_uint64_t  \
))(a,b))

#ifdef __cplusplus
extern "C" }
#endif
