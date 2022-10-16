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

    void * page1 = request_page();
    printf("Page 1: %p\n", page1);
    void * page2 = request_page();
    printf("Page 2: %p\n", page2);
    free_page(page2);
    void * page3 = request_page();
    printf("Page 3: %p\n", page3);

    printf("Kernel looping\n");
    hlt();
}