#include <stdio.h>
#include <string.h>
#include "memalloc.h"

void print_heap_state()
{
    printf("\n=== État du Heap ===\n");
    block_t *start = get_heap_start();
    block_t *it = start;

    int i = 0;
    while (it != NULL)
    {
        printf("Bloc %d: addr=%p, size=%zu,", i, it, it->size);
        if (it->is_free)
        {
            printf(" FREE\n");
        }
        else
        {
            printf(" USED\n");
        }
        it = it->next;
        i++;
    }
    printf("===============\n\n");
}

int main()
{
    init_heap();

    printf("\n=== TEST 1: Allocation simple ===\n");
    void *p1 = my_malloc(100);
    printf("p1 = %p\n", p1);
    print_heap_state();

    printf("\n=== TEST 2: Free ===\n");
    my_free(p1);
    print_heap_state();

    printf("\n=== TEST 3: Réutilisation ===\n");
    void *p2 = my_malloc(50);
    printf("p2 = %p (devrait être proche de p1)\n", p2);
    print_heap_state();

    printf("\n=== TEST 4: Coalescence ===\n");
    void *p3 = my_malloc(100);
    void *p4 = my_malloc(100);
    printf("p3 = %p, p4 = %p\n", p3, p4);
    print_heap_state();

    my_free(p3);
    printf("Après free(p3):\n");
    print_heap_state();

    my_free(p4);
    printf("Après free(p4) - les blocs devraient fusionner:\n");
    print_heap_state();

    printf("\n=== TEST 5: Edge cases ===\n");
    void *p_null = my_malloc(0);
    printf("my_malloc(0) = %p (devrait être NULL)\n", p_null);

    my_free(NULL);
    printf("my_free(NULL) - ne devrait pas crasher\n");

    // Réinitialiser le heap pour les tests realloc
    init_heap();

    printf("\n=== TEST 6: realloc - Réduction de taille ===\n");
    void *p5 = my_malloc(200);
    strcpy((char *)p5, "Hello World");
    printf("Avant realloc(200->50): '%s'\n", (char *)p5);
    print_heap_state();

    p5 = my_realloc(p5, 50);
    printf("Après realloc(50): '%s' (données préservées)\n", (char *)p5);
    print_heap_state();

    printf("\n=== TEST 7: realloc - Agrandissement sur place ===\n");
    void *p6 = my_malloc(100);
    strcpy((char *)p6, "Test");
    printf("Avant realloc(100->300): '%s' à %p\n", (char *)p6, p6);
    print_heap_state();

    void *p6_new = my_realloc(p6, 300);
    printf("Après realloc(300): '%s' à %p\n", (char *)p6_new, p6_new);
    if (p6 == p6_new) {
        printf("✓ Agrandissement sur place (même adresse)\n");
    } else {
        printf("✗ Allocation ailleurs (adresse différente)\n");
    }
    print_heap_state();

    printf("\n=== TEST 8: realloc - Agrandissement ailleurs ===\n");
    void *p7 = my_malloc(50);
    void *p8 = my_malloc(50);
    strcpy((char *)p7, "Data");
    printf("p7='%s' à %p, p8=%p\n", (char *)p7, p7, p8);
    print_heap_state();

    void *p7_new = my_realloc(p7, 500);
    printf("Après realloc(p7, 500): '%s' à %p\n", (char *)p7_new, p7_new);
    if (p7 != p7_new) {
        printf("✓ Allocation ailleurs car pas d'espace adjacent\n");
    }
    print_heap_state();

    printf("\n=== TEST 9: realloc - Edge cases ===\n");
    void *p9 = my_realloc(NULL, 100);
    printf("realloc(NULL, 100) = %p (équivalent à malloc)\n", p9);

    strcpy((char *)p9, "Test");
    void *p9_freed = my_realloc(p9, 0);
    printf("realloc(ptr, 0) = %p (équivalent à free, devrait être NULL)\n", p9_freed);
    print_heap_state();

    return 0;
}
