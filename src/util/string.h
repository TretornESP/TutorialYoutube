#ifndef _STRING_H
#define _STRING_H
#define STRING_MAX_SIZE 65536
#include <stdint.h>

uint64_t strlen(const char* str);
void memset(void * ptr, char chr, uint64_t size);
void strncpy(char * dest, const char* src, uint64_t size);
void memcpy(void * dest, const void* src, uint64_t size);
uint64_t memcmp(const void * dest, const void * src, uint64_t size);
#endif