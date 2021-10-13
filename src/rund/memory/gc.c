#include <rund/memory/gc.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static gc_t* gc;

static size_t gc_suitable_size()
{
    return gc->nitems + 1;
}

static bool gc_resize_more()
{
    size_t new_size = gc_suitable_size();
    size_t old_size = gc->nslots;
    if(new_size <= old_size)
        return true;

    gc_entry_t* old_entries = gc->entries;
    gc->entries = malloc(sizeof(gc_entry_t) * new_size);
    if(gc->entries == NULL)
        return false;

    gc->nslots = new_size;
    for(size_t i = 0; i < old_size; i++)
        memcpy(gc->entries, old_entries, sizeof(gc_entry_t) * old_size);

    free(old_entries);

    return true;
}

static void* gc_add(void* ptr, size_t size)
{
    if(!gc_resize_more())
        return NULL;

    gc->entries[gc->nitems++] = (gc_entry_t){ .ptr = ptr, .size = size, .flags = 0 };

    return ptr;
}

void gc_init(gc_t* _gc, void* stack)
{
    gc = _gc;

    gc->entries = NULL;
    gc->nitems = 0;
    gc->nslots = 0;
    gc->stack = stack;
}

void gc_stop()
{
    for(size_t i = 0; i < gc->nitems; i++)
        free(gc->entries[i].ptr);
    free(gc->entries);
    gc = NULL;
}

void* gc_alloc(size_t size)
{
    void* ptr = malloc(size);
    if(ptr != NULL)
        ptr = gc_add(ptr, size);

    gc_collect();
    return ptr;
}

void gc_free(void* ptr)
{
    if(ptr == NULL)
        return;
    for(size_t i = 0; i < gc->nitems; i++)
        if(gc->entries[i].ptr == ptr)
        {
            for(size_t j = i; j < gc->nitems - 1; j++)
                gc->entries[j] = gc->entries[j + 1];
            gc->nitems--;
            free(ptr);
            break;
        }
}

void gc_collect()
{
    void* stack_top;

    if(gc->nitems == 0)
        return;

    for(size_t i = 0; i < gc->nitems; i++)
        gc->entries[i].flags = 0;

    for(void* ptr = &stack_top; ptr <= gc->stack - sizeof(void*) + 1; ptr = ((uint8_t*)ptr) + 1)
    {
        void* value = *(void**)ptr;
        for(size_t i = 0; i < gc->nitems; i++)
            if(gc->entries[i].ptr == value)
                gc->entries[i].flags = 1;
    }

    bool flagged;
    do
    {
        flagged = false;
        for(size_t i = 0; i < gc->nitems; i++)
            if(gc->entries[i].flags != 0)
                for(void* ptr = gc->entries[i].ptr; ptr < (void*)((uint8_t*)gc->entries[i].ptr + gc->entries[i].size - sizeof(void*) + 1); ptr = ((uint8_t*)ptr) + 1)
                {
                    void* value = *(void**)ptr;
                    for(size_t j = 0; j < gc->nitems; j++)
                        if(gc->entries[j].flags == 0 && gc->entries[j].ptr == value)
                        {
                            gc->entries[j].flags = 1;
                            flagged = true;
                        }
                }
    } while(flagged);

    for(size_t i = 0; i < gc->nitems; i++)
        for(size_t j = 0; j < gc->nitems; j++)
            if(gc->entries[j].flags == 0)
            {
                gc_free(gc->entries[j].ptr);
                i = -1;
                break;
            }

}

#include <stdio.h>

void gc_dump()
{
    printf("GC capacity: %zu\n", gc->nslots);
    printf("Entries (%zu):\n", gc->nitems);
    for(size_t i = 0; i < gc->nitems; i++)
        printf(" - address: %p\n", gc->entries[i].ptr);
}