#ifndef _STRING_H
#define _STRING_H
#define STRING_MAX_SIZE 65536
#include <stdint.h>

uint64_t strlen(const char* str);
void memset(void * ptr, char chr, uint64_t size);
#endif