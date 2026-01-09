#include "memalloc.h"

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

int is_valid_block(block_t *header) {
    block_t *it = heap_start;

    while(it != NULL) {
        if (it == header) {
            return 1;
        }
        it = it->next;
    }

    return 0;
}

block_t *find_previous_block(block_t *current) {
    // On crée un itérateur pour parcourir la mé&moire
    block_t *it = heap_start;

    while(it != NULL && it->next != current) {
        it = it->next;
    }

    if (it == NULL) {
        return NULL;
    }

    return it;
}

size_t align_size(size_t size) {
    size_t reste = size%8;
    if (reste == 0) {
        return size;
    }
    return size + (8 - reste);
}

block_t *get_heap_start() {
    return heap_start;
}

void *my_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size_t correct_size = align_size(size);

    block_t *current = heap_start;

    while(current != NULL) {
        // Vérifier la disponibilité et la taille
        if (current->is_free && current->size >= correct_size) {
            // On split ssi la taille est suffisante pour le bloc demandé + un autre bloc
            if (current->size >= correct_size + sizeof(block_t) + 1) {
                block_t *next = (block_t*)((char*)current + sizeof(block_t) + correct_size);
                next->is_free = 1;
                next->size = current->size - sizeof(block_t) - correct_size;
                next->next = current->next;
                current->next = next;
                current->size = correct_size;
            }
            current->is_free = 0;
            return (void*)(current + 1);
        }
        current = current->next;
    }
    return NULL;
}

int my_free(void *ptr) {
    if (ptr == NULL) {
        return 0;
    }
    // remonter au header
    block_t *header = (block_t*)((char*)ptr - sizeof(block_t));

    if ((char*)header < my_heap || (char*)header >= my_heap + HEAP_SIZE) {
        return -1;
    }

    if(!is_valid_block(header)) {
        return -1;
    }

    if (header->is_free) {
        return 0;
    }
    header->is_free = 1;
    if (header->next != NULL && header->next->is_free) {
        header->size = header->size + sizeof(block_t) + header->next->size;
        header->next = header->next->next;
    }

    block_t *prev = find_previous_block(header);

    // On le place après pour faciliter le calcul
    if(prev != NULL && prev->is_free) {
        prev->size = prev->size + sizeof(block_t) + header->size;
        prev->next = header->next;
    }

    return 0;
}