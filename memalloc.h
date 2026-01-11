#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stddef.h>

#define HEAP_SIZE (1024 * 1024) // 1MB

typedef struct block
{
    size_t size;
    int is_free;
    struct block *next;
} block_t;

extern block_t *heap_start;

block_t *get_heap_start();

int init_heap();

void *my_malloc(size_t size);

int my_free(void *ptr);

void *my_realloc(void *ptr, size_t new_size);

void *my_calloc(size_t nmemb, size_t size);
#endif