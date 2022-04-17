#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct res_s{
    void *ptr;
    void (*free_fn)(void *);
}res;

static res **ptrs;
static uint32_t current_size = 10;
static uint32_t current_index;



void init_mem() { ptrs = calloc(current_size, sizeof(res)); }

void end_mem() {
    for (uint32_t i = 0; i < current_index; i++) {
        ptrs[i]->free_fn(ptrs[i]->ptr);
        free(ptrs[i]);
    }
    free(ptrs);
}

static void check_realloc() {
    if (current_index >= current_size) {
        printf("[mem] reallocation, size from %d to %d\n", current_size,
               current_size * 2);
        res **backup = ptrs;
        ptrs = realloc(ptrs, current_size * 2 * sizeof(res));
        if (!ptrs) {
            ptrs = backup;
            end_mem();
            printf("Reallocation of memory failed\n");
            exit(1);
        }
        memset(ptrs + current_index, 0, current_index * sizeof(res));
        current_size = current_size * 2;
    }
}

void *alloc(uint32_t nmemb, uint32_t size) {
    void *ptr = calloc(nmemb, size);
    check_realloc();
    res *r = calloc(1,sizeof(res));
    r->ptr = ptr;
    r->free_fn = free;
    ptrs[current_index++] = r;
    return ptr;
}

void *alloc_res(void *ptr, void (*free_fn)(void *)){
    check_realloc();
    res *r = calloc(1,sizeof(res));
    r->ptr = ptr;
    r->free_fn = free_fn;
    ptrs[current_index++] = r;
    return ptr;
}

char *strdup_m(char *src) {
    char *ptr = strdup(src);
    check_realloc();
    res *r = calloc(1,sizeof(res));
    r->ptr = ptr;
    r->free_fn = free;
    ptrs[current_index++] = r;
    return ptr;
}