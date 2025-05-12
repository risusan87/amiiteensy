
#include <stdint.h>
#include "utils.h"

__attribute__((section(".startup"), used))
void *memcpy(void *dst, const void *src, size_t len)
{
    uint32_t *d = (uint32_t*)dst;
    const uint32_t* s = (const uint32_t*)src;
    while (len--) {
        *d++ = *s++;
    }
    return dst;
};

__attribute__((section(".startup"), used))
void *memset(void *src, int content, size_t len)
{
    uint32_t *s = (uint32_t*)src;
    while (len--) {
        *s++ = content;
    }
    return src;
};