
#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t len);
void *memset(void *src, int content, size_t len);

void print(const char *s);


#endif