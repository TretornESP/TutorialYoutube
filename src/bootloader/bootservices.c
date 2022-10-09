#include "bootservices.h"
#include "limine_requests.h"

#define CALL_SERVICE(x) (x.response)

#define DEFAULT_TERMINAL_WRITER 0x0
uint64_t terminal_writer = DEFAULT_TERMINAL_WRITER;

void service_error() {
    while (1) {
        __asm__("hlt");
    }
}

uint64_t get_terminal_count() {
    return CALL_SERVICE(TERMINAL)->terminal_count;
}

char * get_bootloader_name() {
    return CALL_SERVICE(BOOTLOADER)->name;
}

char * get_bootloader_version() {
    return CALL_SERVICE(BOOTLOADER)->version;
}

uint64_t get_current_terminal() {
    return terminal_writer;
}

void set_terminal_writer(uint64_t terminal) {
    if (terminal > get_terminal_count()) {
        service_error();
    }
    terminal_writer = terminal;
}

void generic_terminal_writer(const char* str, uint64_t length) {
    struct limine_terminal * terminal = CALL_SERVICE(TERMINAL)->terminals[terminal_writer];
    CALL_SERVICE(TERMINAL)->write(terminal, str, length);
}

void (*get_terminal_writer())(const char*, uint64_t) {
    return generic_terminal_writer;
}


uint64_t get_memory_map_entries() {
    return CALL_SERVICE(MEMMAP)->entry_count;
}

uint64_t get_memory_map_base(uint64_t entry) {
    return CALL_SERVICE(MEMMAP)->entries[entry]->base;
}

uint64_t get_memory_map_length(uint64_t entry) {
    return CALL_SERVICE(MEMMAP)->entries[entry]->length;
}

uint64_t get_memory_map_type(uint64_t entry) {
    return CALL_SERVICE(MEMMAP)->entries[entry]->type;
}