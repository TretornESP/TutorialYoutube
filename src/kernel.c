#include "print/printf/printf.h"
#include "bootloader/bootservices.h"
#include "util/string.h"
#include "memory/memory.h"
#include "io/interrupts.h"
#include "memory/paging.h"
#include "dev/device.h"
#include "drivers/disk/disk.h"
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
    init_paging();
    init_devices();
    init_drive();

    device_list();
    
    uint8_t * buffer = request_page();
    device_read("/dev/hda", 2, 0, buffer);
    for (int i = 0; i < 0x100; i++) {
        printf("%c", buffer[i]);
        if ((i % 20) == 0) printf("\n");
    }

    printf("Kernel looping\n");
    hlt();
}