#include <rund/utils/max.h>

uint64_t max_uint64_t(uint64_t a, uint64_t b)
{
    return a > b ? a : b;
}

uint64_t min_uint64_t(uint64_t a, uint64_t b)
{
    return a < b ? a : b;
}