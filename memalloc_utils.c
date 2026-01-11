#include "memalloc_utils.h"

int is_valid_block(block_t *header)
{
    block_t *it = get_heap_start();

    while (it != NULL)
    {
        if (it == header)
        {
            return 1;
        }
        it = it->next;
    }

    return 0;
}

block_t *find_previous_block(block_t *current)
{
    // On crée un itérateur pour parcourir la mé&moire
    block_t *it = get_heap_start();

    while (it != NULL && it->next != current)
    {
        it = it->next;
    }

    if (it == NULL)
    {
        return NULL;
    }

    return it;
}

size_t align_size(size_t size)
{
    size_t reste = size % 8;
    if (reste == 0)
    {
        return size;
    }
    return size + (8 - reste);
}

void coalesce_next_block(block_t *current_header)
{
    if (current_header->next != NULL && current_header->next->is_free)
    {
        current_header->size = current_header->size + HEADER_SIZE + current_header->next->size;
        current_header->next = current_header->next->next;
    }
}

void coalesce_previous_block(block_t *current_header)
{
    block_t *prev = find_previous_block(current_header);

    // On le place après pour faciliter le calcul
    if (prev != NULL && prev->is_free)
    {
        prev->size = prev->size + sizeof(block_t) + current_header->size;
        prev->next = current_header->next;
    }
}

void coalesce_both_directions(block_t *current_header)
{
    coalesce_next_block(current_header);
    coalesce_previous_block(current_header);
}

void split_next_block(block_t *current_header, size_t size)
{
    block_t *new_header = (block_t *)((char *)current_header + sizeof(block_t) + size);
    new_header->is_free = 1;
    new_header->next = current_header->next;
    new_header->size = current_header->size - size - sizeof(block_t);
    current_header->next = new_header;
    current_header->size = size;
}

int check_can_split(block_t *current_header, size_t size)
{
    return current_header->size > size + MIN_SIZE_TO_SPLIT;
}