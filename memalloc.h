#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stddef.h>

#define HEAP_SIZE (1024 * 1024) // 1MB

typedef struct block {
    size_t size;
    int is_free;
    struct block *next;
} block_t;

block_t *get_heap_start();

int init_heap();

void* my_malloc(size_t size);

int my_free(void *ptr);
#endif