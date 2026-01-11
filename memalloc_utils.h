#ifndef MEMALLOC_UTILS_H
#define MEMALLOC_UTILS_H

#include "memalloc.h"

#define HEADER_SIZE sizeof(block_t)
#define MIN_SIZE_TO_SPLIT HEADER_SIZE + 1

// Récupérer le header
#define GET_HEADER(ptr) ((block_t*)((char*)(ptr) - HEADER_SIZE))

// Calcul de la taille minimale en fonction de la taille actuelle
size_t align_size(size_t size);

block_t *find_previous_block(block_t *current);

int is_valid_block(block_t *header);

// Coalescence
void coalesce_next_block(block_t *current_header);

void coalesce_previous_block(block_t *current_header);

void coalesce_both_directions(block_t *current_header);

// Séparer le reste du bloc en mémoire libre
void split_next_block(block_t *current_header, size_t size);

// Vérifier s'il y a assez d'espace restant pour créer un nouveau bloc de mémoire libre
int check_can_split(block_t *current_header, size_t size);



#endif