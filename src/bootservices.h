#ifndef _BOOTSERVICES_H
#define _BOOTSERVICES_H
#include "limine.h"
#include <stdint.h>

char * get_bootloader_name();
char * get_bootloader_version();

uint64_t get_terminal_count();
uint64_t get_current_terminal();

void set_terminal_writer(uint64_t terminal);
void (*get_terminal_writer())(const char*, uint64_t);
#endif