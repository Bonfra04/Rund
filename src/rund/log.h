#pragma once

#include <stdio.h>

#define TRACE(fmt, ...) printf("[TRACE:%d] " fmt "\n", __VA_ARGS__)
#define ERROR(fmt, ...) printf("[ERROR:%d] " fmt "\n", __VA_ARGS__)
