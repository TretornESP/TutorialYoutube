#ifndef _ACPI_H
#define _ACPI_H
#include <stdint.h>

struct rsdp_descriptor {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed));

struct rsdp2_descriptor {
    struct rsdp_descriptor first_part;

    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed));

struct acpi_sdt_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct rsdt {
    struct acpi_sdt_header header;
    uint32_t pointer_other_sdt[];
} __attribute__((packed));

struct xsdt {
    struct acpi_sdt_header header;
    uint64_t pointer_other_sdt[];
} __attribute__((packed));

struct mcfg_header {
    struct acpi_sdt_header header;
    uint64_t reserved;
} __attribute__((packed));

struct device_config {
    uint64_t base_address;
    uint16_t pci_sec_group;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t reserved;
};

void init_acpi();

#endif