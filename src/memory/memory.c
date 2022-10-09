#include "memory.h"
#include "../bootloader/bootservices.h"
#include "../print/printf/printf.h"

struct uefi_memory_region working_memory = {
    .type = LIMINE_MEMMAP_BAD_MEMORY,
    .length = 0,
    .base = 0,
};

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
    printf("Working memory [ base=%x, size=%x, type=%x]\n", working_memory.base, working_memory.length, working_memory.type);
}