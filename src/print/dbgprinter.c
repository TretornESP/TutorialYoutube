#include "dbgprinter.h"
#include "./printf/printf.h"

void panic(const char* str) {
    printf("PANIC: %s\n", str);
    while (1) {
        __asm__("hlt");
    }
}