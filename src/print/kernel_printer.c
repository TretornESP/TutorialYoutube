#include "kernel_printer.h"
#include "../bootloader/bootservices.h"
void _putchar(char character)
{
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(&character, 1);
}