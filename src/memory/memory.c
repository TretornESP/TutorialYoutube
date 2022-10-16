#include "memory.h"
#include "../bootloader/bootservices.h"
#include "../print/dbgprinter.h"
#include "../util/string.h"
#include "../print/printf/printf.h"

#define ADDR_TO_PAGE(x) ((uint64_t)(x) >> 12)

struct uefi_memory_region working_memory = {
    .type = LIMINE_MEMMAP_BAD_MEMORY,
    .length = 0,
    .base = 0,
};
uint64_t last_requested;
uint64_t total_pages;
uint8_t * phys_region;

void lock_page(uint64_t page) {
    phys_region[page] = 1;
    last_requested = page;
}

void unlock_page(uint64_t page) {
    printf("Unlocking page %d\n", page);
    phys_region[page] = 0;
}

void free_page(void* addr) {
    uint64_t addr_uint = (uint64_t)addr;
    uint64_t addr_normaliced = (addr_uint - working_memory.base);

    if (addr_uint < working_memory.base) {
        panic("Freeing page outside of memory region");
    }

    if (phys_region[ADDR_TO_PAGE(addr_normaliced)] == 0) {
        panic("Freeing page that is not locked");
    }

    unlock_page(ADDR_TO_PAGE(addr_normaliced));

    if (phys_region[ADDR_TO_PAGE(addr_normaliced)] == 1) {
        panic("Free failed");
    }
}

void * request_page() {
    uint64_t first = last_requested-1;

    while(phys_region[last_requested]) {
        if (last_requested >= total_pages)
            last_requested = 0;
        if (last_requested == first) {
            printf("No free memory and no swap available\n"); //TODO: Implementar swap
            return (void*)-1;
        }
        last_requested++;
    }

    lock_page(last_requested);

    if (phys_region[last_requested] == 0) {
        panic("Page request failed");
    }

    return (void*)(working_memory.base + (last_requested * 0x1000));
}

void init_pfa() {
    total_pages = working_memory.length / 0x1000;
    phys_region = (uint8_t*)working_memory.base;

    memset((void*)phys_region, 0, total_pages);
    for (uint64_t i = 0; i < (total_pages / 0x1000); i++) {
        lock_page(i);
    }
}

void init_memory() {
    printf("Cargando memoria\n");

    uint64_t entries = get_memory_map_entries();

    printf("Detected %d memory regions\n", entries);

    uint64_t total_memory = 0;
    uint64_t total_avail_memory = 0;

    for (uint64_t i = 0; i < entries; i++) {
        uint64_t type = get_memory_map_type(i);
        uint64_t length = get_memory_map_length(i);
        uint64_t base = get_memory_map_base(i);

        printf("Memory region %d base=%x length=%x type=%d\n", i, base, length, type);

        total_memory += length;
        if (type == LIMINE_MEMMAP_USABLE) {
            total_avail_memory += length;
            if (length > working_memory.length) {
                working_memory.length = length;
                working_memory.type = type;
                working_memory.base = base;
            }
        }
    }
    
    printf("Total memory: %x, available_memory: %x\n", total_memory, total_avail_memory);
    printf("Working memory [ base=%llx, size=%llx, type=%llx]\n", working_memory.base, working_memory.length, working_memory.type);

    init_pfa();
}