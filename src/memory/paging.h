#ifndef _PAGING_H
#define _PAGING_H
#include <stdint.h>

#define PAGESIZE 0x1000

#define PAGE_WRITE_BIT      0x1
#define PAGE_USER_BIT       0x2
#define PAGE_NX_BIT         0x3
#define PAGE_CACHE_DISABLE  0x8

#define PAGE_ALLOW_WRITE(x)     ((page_set      ((x), PAGE_WRITE_BIT)))
#define PAGE_RESTRICT_WRITE(x)  ((page_clear    ((x), PAGE_WRITE_BIT)))
#define PAGE_ALLOW_USER(x)      ((page_set      ((x), PAGE_USER_BIT)))
#define PAGE_RESTRICT_USER(x)   ((page_clear    ((x), PAGE_USER_BIT)))
#define PAGE_ALLOW_NX(x)        ((page_set      ((x), PAGE_NX_BIT)))
#define PAGE_RESTRICT_NX(x)     ((page_clear    ((x), PAGE_NX_BIT)))
#define PAGE_DISABLE_CACHE(x)   ((page_set      ((x), PAGE_CACHE_DISABLE)))
#define PAGE_ENABLE_CACHE(x)    ((page_clear    ((x), PAGE_CACHE_DISABLE)))

struct page_directory_entry {
    uint64_t present            : 1;
    uint64_t writeable          : 1;
    uint64_t user_access        : 1;
    uint64_t write_through      : 1;
    uint64_t cache_disabled     : 1;
    uint64_t accessed           : 1;
    uint64_t ignored_3          : 1;
    uint64_t size               : 1;
    uint64_t ignored_2          : 4;
    uint64_t page_ppn           : 28;
    uint64_t reserved_1         : 12;
    uint64_t ignored_1          : 11;
    uint64_t execution_disabled : 1;
} __attribute__((packed));

struct page_table_entry {
    uint64_t present            : 1;
    uint64_t writeable          : 1;
    uint64_t user_access        : 1;
    uint64_t write_through      : 1;
    uint64_t cache_disabled     : 1;
    uint64_t accessed           : 1;
    uint64_t dirty              : 1;
    uint64_t size               : 1;
    uint64_t global             : 1;
    uint64_t ignored_2          : 3;
    uint64_t page_ppn           : 28;
    uint64_t reserved_1         : 12;
    uint64_t ignored_1          : 11;
    uint64_t execution_disabled : 1;
} __attribute__((packed));

struct page_directory {
    struct page_directory_entry entries[512];
} __attribute__((aligned(PAGESIZE)));

struct page_table {
    struct page_table_entry entries[512];
} __attribute__((aligned(PAGESIZE)));

struct page_map_index {
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
};

void page_set(void*, uint8_t);
void page_clear(void*, uint8_t);

void init_paging();
void address_to_map(uint64_t, struct page_map_index*);
void map_memory(void*, void*);
uint64_t virtual_to_physical(void*);
void * request_page_identity();
void mprotect(void*, uint64_t, uint8_t);

#endif