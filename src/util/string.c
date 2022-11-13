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

void strncpy(char * dest, const char* src, uint64_t size) {
    uint64_t i = 0;
    while (i < size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void memcpy(void * dest, const void* src, uint64_t size) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;

    for (uint64_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

uint64_t memcmp(const void * dest, const void * src, uint64_t size) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;

    for (uint64_t i = 0; i < size; i++) {
        if (d[i] != s[i])
            return 1;
    }

    return 0;
}