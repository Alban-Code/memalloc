# CAHIER DES CHARGES - ALLOCATEUR MÉMOIRE CUSTOM

## Objectif du projet
Implémenter un allocateur de mémoire dynamique en C pur, avec gestion manuelle d'un heap statique.

## Contraintes techniques
- **Langage** : C pur (standard C11)
- **Heap** : Tableau statique de 1 MB (`static char heap[1024 * 1024]`)
- **Pas de malloc/free système** : Uniquement tes propres fonctions
- **Compilation** : `gcc -Wall -Wextra -Werror`

## Fonctions obligatoires à implémenter

### Version minimale (MVP)
```c
void* my_malloc(size_t size);
void  my_free(void* ptr);
```

### Version complète
```c
void* my_malloc(size_t size);
void  my_free(void* ptr);
void* my_realloc(void* ptr, size_t new_size);
void* my_calloc(size_t nmemb, size_t size);
```

## Spécifications fonctionnelles

### `void* my_malloc(size_t size)`
**Comportement** :
- Alloue `size` octets de mémoire
- Retourne un pointeur vers la zone allouée
- Retourne `NULL` si allocation impossible

**Contraintes** :
- Si `size == 0` : comportement au choix (retourner NULL ou un pointeur unique)
- Alignement mémoire sur 8 octets minimum
- Algorithme de recherche : **First Fit** (premier bloc libre suffisant)

### `void my_free(void* ptr)`
**Comportement** :
- Libère la mémoire pointée par `ptr`
- Ne fait rien si `ptr == NULL`

**Contraintes** :
- Doit fusionner les blocs libres adjacents (coalescence)
- Doit détecter les doubles free (undefined behavior acceptable pour MVP)

### `void* my_realloc(void* ptr, size_t new_size)`
**Comportement** :
- Redimensionne le bloc pointé par `ptr` à `new_size` octets
- Retourne un pointeur vers le nouveau bloc (peut être différent de `ptr`)

**Cas particuliers** :
- Si `ptr == NULL` : équivalent à `my_malloc(new_size)`
- Si `new_size == 0` : équivalent à `my_free(ptr)`, retourne NULL
- Si redimensionnement impossible sur place : allouer ailleurs et copier

### `void* my_calloc(size_t nmemb, size_t size)`
**Comportement** :
- Alloue `nmemb * size` octets
- Initialise la mémoire à zéro
- Retourne un pointeur vers la zone allouée

**Contraintes** :
- Vérifier le dépassement lors de la multiplication (`nmemb * size`)

## Structure de données

### Métadonnées de bloc (minimum requis)
```c
typedef struct block {
    size_t size;           // Taille utilisable (sans métadonnées)
    int is_free;           // 1 = libre, 0 = occupé
    struct block* next;    // Prochain bloc dans la liste
} block_t;
```

### Organisation mémoire
```
Heap: [block_t][données][block_t][données][block_t][données]...
      ^         ^        ^         ^        ^
      metadata  user     metadata  user     metadata
```

## Algorithmes requis

### 1. Recherche de bloc (First Fit)
- Parcourir la liste chaînée des blocs
- Sélectionner le premier bloc libre de taille suffisante

### 2. Split (division de bloc)
Obligatoire si le bloc trouvé est beaucoup plus grand que nécessaire :
- Minimum 32 octets de différence pour splitter
- Créer un nouveau bloc avec l'espace restant

### 3. Coalescence (fusion de blocs)
Lors d'un `free`, fusionner avec :
- Le bloc suivant s'il est libre
- Le bloc précédent s'il est libre (nécessite parcours de la liste)

## Fonctions utilitaires recommandées
```c
void  init_heap(void);                    // Initialiser le heap au démarrage
void  print_heap_state(void);             // Debug : afficher l'état du heap
block_t* get_block_metadata(void* ptr);   // Récupérer les métadonnées depuis ptr
size_t get_total_free_memory(void);       // Stats : mémoire libre totale
size_t get_largest_free_block(void);      // Stats : plus grand bloc libre
```

## Tests obligatoires

### Tests de base
1. **Allocation simple** : `malloc` puis `free`
2. **Allocations multiples** : 10+ allocations de tailles variées
3. **Réutilisation** : `malloc` → `free` → `malloc` (même taille)
4. **Fragmentation** : Alterner allocations et libérations

### Tests edge cases
5. **malloc(0)** : Ne doit pas crasher
6. **free(NULL)** : Ne doit rien faire
7. **Double free** : Comportement défini (ou détection)
8. **Heap plein** : `malloc` doit retourner NULL
9. **realloc edge cases** : `ptr=NULL`, `size=0`

### Tests de stress
10. **Boucle intensive** : 1000+ allocations/libérations aléatoires
11. **Allocation maximale** : Tenter d'allouer tout le heap
12. **Vérification données** : Écrire/lire pour détecter corruptions

## Programme de test minimal
```c
int main(void) {
    // Test 1 : Allocation basique
    void* p1 = my_malloc(100);
    assert(p1 != NULL);
    my_free(p1);
    
    // Test 2 : Multiple allocations
    void* p2 = my_malloc(50);
    void* p3 = my_malloc(200);
    void* p4 = my_malloc(75);
    
    // Test 3 : Réutilisation après free
    my_free(p2);
    void* p5 = my_malloc(40);  // Doit réutiliser l'espace de p2
    
    // Test 4 : Coalescence
    my_free(p3);
    my_free(p4);
    void* p6 = my_malloc(250);  // Doit fusionner p3+p4
    
    print_heap_state();
    
    return 0;
}
```

## Livrables

### Fichiers source
- `my_malloc.c` : Implémentation
- `my_malloc.h` : Header avec prototypes
- `test_malloc.c` : Programme de tests
- `Makefile` : Compilation

### Documentation (optionnelle mais recommandée)
- `README.md` : Explication du fonctionnement
- Schémas de la structure mémoire
- Complexité algorithmique

## Critères de réussite

### Version MVP (suffisante)
✅ `my_malloc` et `my_free` fonctionnels  
✅ First Fit implémenté  
✅ Coalescence des blocs adjacents  
✅ Tests de base passent  

### Version complète (excellente)
✅ MVP + `my_realloc` et `my_calloc`  
✅ Split de blocs optimisé  
✅ Alignement mémoire correct  
✅ Gestion des edge cases  
✅ Tous les tests passent  
✅ Pas de fuites mémoire détectables  

## Extensions possibles (bonus)

- Liste doublement chaînée pour coalescence O(1)
- Stratégies multiples (First/Best/Worst Fit)
- Pools de tailles fixes pour petites allocations
- Détection de corruption mémoire
- Statistiques détaillées (fragmentation, utilisation)
- Thread-safety avec mutex

## Estimation de temps
- **MVP** : 1-2 jours
- **Version complète** : 3-5 jours
- **Avec bonus** : 1 semaine