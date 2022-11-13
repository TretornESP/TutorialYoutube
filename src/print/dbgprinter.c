#include "dbgprinter.h"
#include "./printf/printf.h"
#define BUFFERSIZE 128
char boot_conversor_buffer[BUFFERSIZE] = {0};

void panic(const char* str) {
    printf("PANIC: %s\n", str);
    while (1) {
        __asm__("hlt");
    }
}

char* itoa(int64_t value, int base) {
    // check that the base if valid
    for (int i = 0; i < BUFFERSIZE; i++) {
        boot_conversor_buffer[i] = 0;
    }
    if (base < 2 || base > 36) { *boot_conversor_buffer = '\0'; return boot_conversor_buffer; }

    char* ptr = boot_conversor_buffer, *ptr1 = boot_conversor_buffer, tmp_char;
    int64_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return boot_conversor_buffer;
}