#include "bootservices.h"
#define STRING_MAX_SIZE 65536

uint64_t strlen(const char* str) {
    uint64_t len = 0;
    while (str[len] && len < STRING_MAX_SIZE)
        len++;
    return len;
}

void print(const char* str) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(str, strlen(str));
}

void hlt() {
    while (1) {
        __asm__("hlt");
    }
}

void _start() {
    print("Hello, world!");
    hlt();
}