#include <stdio.h>
#include "memalloc.h"

void print_heap_state() {
    printf("\n=== État du Heap ===\n");
    block_t *start = get_heap_start();
    block_t *it = start;

    int i = 0;
    while(it != NULL) {
        printf("Bloc %d: addr=%p, size=%zu,", i, it, it->size);
        if(it->is_free) {
            printf(" FREE\n");
        } else {
            printf(" USED\n");
        }
        it = it->next;
        i++;
    }
    printf("===============\n\n");
}

int main() {
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

      return 0;
}

