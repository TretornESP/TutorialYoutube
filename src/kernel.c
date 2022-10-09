#include "print/printf/printf.h"
#include "bootloader/bootservices.h"
#include "util/string.h"
#include "memory/memory.h"
#include <stdint.h>

void hlt() {
    __asm__("hlt");
}

void _start() {
    printf("Loading kernel, bootloader=%s, version=%s\n",
        get_bootloader_name(),
        get_bootloader_version()
    );
    
    init_memory();

    printf("Kernel looping\n");
    hlt();
}