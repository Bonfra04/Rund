#pragma once

#include <stdint.h>

uint64_t max_uint64_t(uint64_t a, uint64_t b);
uint64_t min_uint64_t(uint64_t a, uint64_t b);

#define max(a, b) ((_Generic((a), \
    uint64_t: max_uint64_t  \
))(a,b))

#define min(a, b) ((_Generic((a), \
    uint64_t: min_uint64_t  \
))(a,b))

