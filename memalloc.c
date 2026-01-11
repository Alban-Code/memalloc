#include <string.h>

#include "memalloc.h"
#include "memalloc_utils.h"

// static car impossible d'y accéder autre part qu'ici
static char my_heap[HEAP_SIZE];

block_t *heap_start = (block_t *)my_heap;

int init_heap()
{
    // Le bloc est libre
    heap_start->is_free = 1;
    // Taille de la mémoire = taille max - taille du premier header
    heap_start->size = HEAP_SIZE - sizeof(block_t);
    // next est null car aucun bloc
    heap_start->next = NULL;

    return 0;
}

block_t *get_heap_start()
{
    return heap_start;
}

void *my_malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    size_t correct_size = align_size(size);

    block_t *current = heap_start;

    while (current != NULL)
    {
        // Vérifier la disponibilité et la taille
        if (current->is_free && current->size >= correct_size)
        {
            // On split ssi la taille est suffisante pour le bloc demandé + un autre bloc
            if (check_can_split(current, correct_size))
            {
                split_next_block(current, correct_size);
            }
            current->is_free = 0;
            return (void *)(current + 1);
        }
        current = current->next;
    }
    return NULL;
}

int my_free(void *ptr)
{
    if (ptr == NULL)
    {
        return 0;
    }
    // remonter au header
    block_t *header = GET_HEADER(ptr);

    if ((char *)header < my_heap || (char *)header >= my_heap + HEAP_SIZE)
    {
        return -1;
    }

    if (!is_valid_block(header))
    {
        return -1;
    }

    if (header->is_free)
    {
        return 0;
    }
    header->is_free = 1;

    coalesce_both_directions(header);

    return 0;
}

void *my_realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
    {
        return my_malloc(new_size);
    }

    if (new_size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    block_t *header = GET_HEADER(ptr);

    if (new_size == header->size)
    {
        return ptr;
    }

    size_t new_size_aligned = align_size(new_size);
    if (new_size_aligned < header->size)
    {
        // S'il y a assez d'espace pour splitter
        if (check_can_split(header, new_size_aligned))
        {
            split_next_block(header, new_size_aligned);

            coalesce_next_block(header->next);
        }
        header->size = new_size_aligned;
        return ptr;
    }

    if (new_size_aligned > header->size)
    {
        if (header->next != NULL && header->next->is_free && (header->size + header->next->size + sizeof(block_t)) >= new_size_aligned)
        {
            // Calculer la taille totale disponible après fusion
            size_t total_size = header->size + sizeof(block_t) + header->next->size;
            block_t *next_next = header->next->next;

            // Fusionner avec le bloc suivant
            header->next = next_next;
            header->size = total_size;

            // Si on peut splitter (assez d'espace restant pour un nouveau bloc)
            if (check_can_split(header, new_size_aligned))
            {
                split_next_block(header, new_size_aligned);
            }

            return ptr;
        }

        void *new_ptr = my_malloc(new_size);
        if (new_ptr == NULL)
        {
            return NULL;
        }

        memcpy(new_ptr, ptr, header->size);
        my_free(ptr);
        return new_ptr;
    }

    return ptr;
}