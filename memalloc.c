#include "memalloc.h"

typedef struct block {
    size_t size;
    int is_free;
    struct block *next;
} block_t;

// static car impossible d'y accéder autre part qu'ici
static char my_heap[HEAP_SIZE];

static block_t *heap_start = (block_t *)my_heap;

int init_heap() {
    // Le bloc est libre
    heap_start->is_free = 1;
    // Taille de la mémoire = taille max - taille du premier header
    heap_start->size = HEAP_SIZE - sizeof(block_t);
    // next est null car aucun bloc
    heap_start->next = NULL;

    return 0;
}

void *my_malloc(size_t size) {
    block_t *current = heap_start;

    while(current != NULL) {
        // Vérifier la disponibilité et la taille
        if (current->is_free && current->size >= size) {
            // On split ssi la taille est suffisante pour le bloc demandé + un autre bloc
            if (current->size >= size + sizeof(block_t) + 1) {
                block_t *next = (block_t*)((char*)current + sizeof(block_t) + size);
                next->is_free = 1;
                next->size = current->size - sizeof(block_t) - size;
                next->next = current->next;
                current->next = next;
                current->size = size;
            }
            current->is_free = 0;
            return (void*)(current + 1);
        }
        current = current->next;
    }
    return NULL;
}

int my_free(void *ptr) {
    // remonter au header
    block_t *header = (block_t*)((char*)ptr - sizeof(block_t));
    header->is_free = 0;
    if (header->next->is_free) {
        header->size = header->size + sizeof(block_t) + header->next->size;
        header->next = header->next->next;
    }

    return 0;
}