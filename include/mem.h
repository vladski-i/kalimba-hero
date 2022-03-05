#ifndef __MEM_H
#define __MEM_H

#include <stdint.h>

void *alloc(uint32_t nmemb, uint32_t size);
char *strdup_m(char *src);
void init_mem();
void end_mem();
#endif
