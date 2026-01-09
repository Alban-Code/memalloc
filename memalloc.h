#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stddef.h>

#define HEAP_SIZE (1024 * 1024) // 1MB

int init_heap();

void* my_malloc(size_t size);

int my_free(void *ptr);
#endif