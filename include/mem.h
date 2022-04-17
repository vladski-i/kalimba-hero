#ifndef __MEM_H
#define __MEM_H

#include <stdint.h>

void *alloc(uint32_t nmemb, uint32_t size);
void *alloc_res(void *ptr, void (*free_fn)(void *));
char *strdup_m(char *src);
void init_mem();
void end_mem();
#endif
