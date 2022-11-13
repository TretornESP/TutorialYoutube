#ifndef _DBGPRINTER_H	
#define _DBGPRINTER_H
#include <stdint.h>
void panic(const char* str);
char * itoa(int64_t, int);
#endif