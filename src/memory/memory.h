#ifndef _MEMORY_H
#define _MEMORY_H
#include <stdint.h>

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

extern uint64_t KERNEL_START;
extern uint64_t KERNEL_END;

struct uefi_memory_region {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

void init_memory();
void free_page(void* addr);
void * request_page();
#endif