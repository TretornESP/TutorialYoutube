#include "string.h"

uint64_t strlen(const char* str) {
    uint64_t len = 0;
    while (str[len] && len < STRING_MAX_SIZE)
        len++;
    return len;
}

void memset(void * ptr, char chr, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        ((char*)ptr)[i] = chr;
    }
}