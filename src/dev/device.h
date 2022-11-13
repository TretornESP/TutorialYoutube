#ifndef _DEVICE_H
#define _DEVICE_H
#include <stdint.h>
#include "pci/pci.h"
#include "acpi/acpi.h"

#define BLOCK_SCSI 0x8
#define CHAR_SCSI 0x88
#define ABS_TO_PAGE(addr, page_size) (addr / page_size)

struct file_operations {
    uint64_t (*read)(uint8_t, uint64_t, uint64_t, uint8_t*);
    uint64_t (*write)(uint8_t, uint64_t, uint64_t, uint8_t*);
    uint64_t (*ioctl)(uint8_t, uint32_t, void*);
};

struct device_driver {
    struct file_operations* fops;
    uint8_t registered;
    char name[32];
};

struct device {
    uint8_t bc : 1;
    uint8_t valid : 1;
    uint8_t major;
    uint8_t minor;
    char name[32];
    uint8_t internal_id;
    struct pci_device_header *pci;
    struct device * next;
};

const char* get_vendor_name(uint16_t);
const char* get_device_name(uint16_t, uint16_t);
const char* get_device_class(uint8_t);
const char* get_subclass_name(uint8_t, uint8_t);
const char* get_prog_interface(uint8_t, uint8_t, uint8_t);

void register_device(struct pci_device_header*);
void device_list();
void register_char(uint8_t, const char*, struct file_operations*);
void register_block(uint8_t, const char*, struct file_operations*);
void unregister_char(uint8_t);
void unregister_block(uint8_t);

struct device * get_device_head();
struct device * get_next_device(struct device*);
struct device* device_search(const char*);
uint32_t get_device_count();
uint32_t get_device_count_by_major(uint8_t);

void init_devices();

uint64_t device_ioctl(const char*, uint32_t, void*);
uint64_t device_read(const char*, uint64_t, uint64_t, uint8_t*);
uint64_t device_write(const char*, uint64_t, uint64_t, uint8_t*);

#endif