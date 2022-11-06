#include "paging.h"
#include "memory.h"
#include "../util/string.h"
#include "../print/dbgprinter.h"
#include "../print/printf/printf.h"
#include "../bootloader/bootservices.h"

struct page_directory * pml4;

void init_paging() {
    printf("### PAGING STARTUP ###\n");
    __asm__("movq %%cr3, %0" : "=r"(pml4));
    __asm__("movq %0, %%cr3" : : "r"(pml4)); //Invalidamos TLB

    uint64_t virtual_start = get_kernel_address_virtual();
    uint64_t linker_kstart = (uint64_t)&KERNEL_START;

    if (virtual_start != linker_kstart) {
        printf("Crashing: KERNEL_START: %llx VIRT_ADDR: %p\n", linker_kstart, virtual_start);
        panic("init_paging: kernel virtual address does not match KERNEL_START linker symbol\n");
    }   
}

void set_page_perms(void* address, uint8_t permissions) {
    struct page_map_index map;
    address_to_map((uint64_t)address, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];

    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry *pte = &pt->entries[map.P_i];

    pte -> writeable = (permissions & 1);
    pte -> user_access = ((permissions & 2) >> 1);
    pte -> execution_disabled = ((permissions & 4 ) >> 2);
    pte -> cache_disabled = ((permissions & 8) >> 3);
}

uint8_t get_page_perms(void * address) {
    struct page_map_index map;
    address_to_map((uint64_t)address, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];

    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry pte = pt->entries[map.P_i];

    uint8_t result = pte.writeable;
    result |= (pte.user_access << 1);
    result |= (pte.execution_disabled << 2);
    result |= (pte.cache_disabled << 3);

    return result;
}

void page_set(void* address, uint8_t field) {
    uint8_t perms = get_page_perms(address);
    perms |= field;
    set_page_perms(address, perms);
}

void page_clear(void* address, uint8_t field) {
    uint8_t perms = get_page_perms(address);
    perms &= ~field;
    set_page_perms(address, perms);
}

void address_to_map(uint64_t address, struct page_map_index* map) {
    address >>= 12;
    map->P_i = address & 0x1ff;
    address >>= 9;
    map->PT_i = address & 0x1ff;
    address >>= 9;
    map->PD_i = address & 0x1ff;
    address >>= 9;
    map->PDP_i = address & 0x1ff;
}

void map_memory(void* virtual_memory, void* physical_memory) {
    if ((uint64_t)virtual_memory & 0xfff) {
        printf("Crashing: virtual memory: %p\n", virtual_memory);
        panic("map_memory: virtual memory unaligned\n");
    }

    if ((uint64_t)physical_memory & 0xfff) {
        printf("Crashing: physical memory: %p\n", physical_memory);
        panic("map_memory: physical memory unaligned\n");
    }

    struct page_map_index map;
    address_to_map((uint64_t)virtual_memory, &map);

    struct page_directory_entry pde;

    pde = pml4->entries[map.PDP_i];
    struct page_directory* pdp;
    if (!pde.present) {
        pdp = (struct page_directory*)request_page();
        memset(pdp, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pdp >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pml4->entries[map.PDP_i] = pde;
    } else {
        pdp = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    }

    pde = pdp->entries[map.PD_i];
    struct page_directory* pd;
    if (!pde.present) {
        pd = (struct page_directory*)request_page();
        memset(pd, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pd >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pdp->entries[map.PD_i] = pde;
    } else {
        pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    }

    pde = pd->entries[map.PT_i];
    struct page_table* pt;
    if (!pde.present) {
        pt = (struct page_table*)request_page();
        memset(pt, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pt >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pd->entries[map.PT_i] = pde;
    } else {
        pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    }

    struct page_table_entry pte = pt->entries[map.P_i];
    pte.page_ppn = (uint64_t) physical_memory >> 12;
    pte.present = 1;
    pte.writeable = 1;
    pt->entries[map.P_i] = pte;
}

uint64_t virtual_to_physical(void* virtual_memory) {
    struct page_map_index map;
    address_to_map((uint64_t)virtual_memory, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];

    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry pte = pt->entries[map.P_i];

    uint64_t physical = ((uint64_t)pte.page_ppn << 12) | ((uint64_t)virtual_memory & 0xfff);
    return physical;
}

void * request_page_identity() {
    void * result = request_page();
    map_memory(result, result);
    return result;
}

void mprotect(void* address, uint64_t size, uint8_t permissions) {
    uint64_t start = (uint64_t)address;
    uint64_t end   = start + size;

    start = start & ~0xfff;
    end = (end + 0xfff) & ~0xfff;

    for (uint64_t i = start; i < end; i+= 0x1000) {
        set_page_perms((void*)i, permissions);
    }
}