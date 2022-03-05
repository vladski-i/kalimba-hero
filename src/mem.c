#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void **ptrs;
static uint32_t current_size = 10;
static uint32_t current_index;

void init_mem() { ptrs = calloc(current_size, sizeof(void *)); }

void end_mem() {
    for (uint32_t i = 0; i < current_index; i++) {
        free(ptrs[i]);
    }
    free(ptrs);
}

static void check_realloc() {
    if (current_index >= current_size) {
        printf("[mem] reallocation, index %d, size from %d\n", current_index,
               current_size);
        void **backup = ptrs;
        ptrs = realloc(ptrs, current_size * 2 * sizeof(void *));
        printf("[mem] backup %p, ptrs %p\n", backup, ptrs);
        if (!ptrs) {
            ptrs = backup;
            end_mem();
            printf("Reallocation of memory failed\n");
            exit(1);
        }
        memset(ptrs + current_index, 0, current_index * sizeof(void *));
        current_size = current_size * 2;
    }
}

void *alloc(uint32_t nmemb, uint32_t size) {
    void *ptr = calloc(nmemb, size);
    check_realloc();
    ptrs[current_index++] = ptr;
    return ptr;
}

char *strdup_m(char *src) {
    char *ptr = strdup(src);
    check_realloc();
    ptrs[current_index++] = (void *)ptr;
    return ptr;
}