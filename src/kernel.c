#include "print/printf/printf.h"
#include "bootloader/bootservices.h"
#include "util/string.h"
#include "memory/memory.h"
#include "io/interrupts.h"
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
    init_interrupts();

    uint64_t * badptr = (uint64_t*)0xffffffffdeadb000;
    *badptr = 0xdeadbeef;

    printf("Kernel looping\n");
    hlt();
}