#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct gc_entry
{
    void* ptr;
    size_t size;
    uint8_t flags;
} gc_entry_t;

typedef struct gc
{
    size_t nitems; 
    size_t nslots;
    gc_entry_t* entries;
    void* stack;
} gc_t;

void gc_init(gc_t* gc, void* stack);
void gc_stop();
void* gc_alloc(size_t size);
void gc_free(void* ptr);
void gc_collect();
void gc_dump();

