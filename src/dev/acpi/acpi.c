#include "acpi.h"
#include "../../print/printf/printf.h"
#include "../../util/string.h"
#include "../../print/dbgprinter.h"
#include "../../bootloader/bootservices.h"
#include "../pci/pci.h"

uint8_t acpi_sdt_checksum(struct acpi_sdt_header * table_header) {
    uint8_t sum = 0;

    for (uint32_t i = 0; i < table_header->length; i++) {
        sum += ((uint8_t*) table_header)[i];
    }

    return (sum == 0);
}

struct acpi_sdt_header * find_rsdt(struct rsdt* rsdt, char* signature, uint64_t sig_len) {
    uint32_t entries = (rsdt->header.length - sizeof(struct acpi_sdt_header)) / sizeof(uint32_t);
    printf("Navigating %d entries\n", entries);

    for (uint32_t i = 0; i < entries; i++) {
        uint32_t pto = rsdt->pointer_other_sdt[i];
        struct acpi_sdt_header * header = (struct acpi_sdt_header*)(uint64_t)pto;
        printf("Entry %d => %x HEADER AT: %p SIG: %s\n", i, pto, header, header->signature);

        if (!memcmp(header->signature, signature, sig_len))
            return header;
    }

    printf("Could not find ACPI table with signature %s\n", signature);
    return 0;
}

struct acpi_sdt_header * find_xsdt(struct xsdt* xsdt, char* signature, uint64_t sig_len) {
    uint32_t entries = (xsdt->header.length - sizeof(struct acpi_sdt_header)) / sizeof(uint64_t);
    printf("Navigating %d entries\n", entries);

    for (uint32_t i = 0; i < entries; i++) {
        uint32_t pto = xsdt->pointer_other_sdt[i];
        struct acpi_sdt_header * header = (struct acpi_sdt_header*)(uint64_t)pto;
        printf("Entry %d => %x HEADER AT: %p SIG: %s\n", i, pto, header, header->signature);

        if (!memcmp(header->signature, signature, sig_len))
            return header;
    }

    printf("Could not find ACPI table with signature %s\n", signature);
    return 0;  
}

void * init_acpi_vt(void* rsdp_address) {
    char signature[9];
    char oemid[7];

    printf("RSDP Revision 2\n");

    struct rsdp2_descriptor * rsdp = (struct rsdp2_descriptor*) rsdp_address;

    if (memcmp(rsdp->first_part.signature, "RSD PTR ", 8))
        panic("RSDP Signature mismatch");

    strncpy(oemid, rsdp->first_part.oem_id, 6);
    strncpy(signature, rsdp->first_part.signature, 8);

    printf("RSDP at: 0x%llx Sig: %s Check: %d OEM: %s Rev: %d RsdtAddress 0x%llx Len: 0x%llx XsdtAddr: 0x%llx ExtCheck: %d Res: %s\n",
        rsdp, signature, rsdp->first_part.checksum, oemid, rsdp->first_part.revision, rsdp->first_part.rsdt_address,
        rsdp->length, rsdp->xsdt_address, rsdp->extended_checksum, rsdp->reserved
    );

    struct xsdt* xsdt = (struct xsdt*)(uint64_t)(rsdp->xsdt_address);
    struct acpi_sdt_header* mcfg = find_xsdt(xsdt, "MCFG", 4);

    if (!acpi_sdt_checksum(mcfg))
        panic("XSDT checksum mismatch");

    return (void*)mcfg;
}

void * init_acpi_vz(void* rsdp_address) {
    char signature[9];
    char oemid[7];

    printf("RSDP Revision 0\n");

    struct rsdp_descriptor * rsdp = (struct rsdp_descriptor*) rsdp_address;

    if (memcmp(rsdp->signature, "RSD PTR ", 8))
        panic("RSDP Signature mismatch");

    strncpy(oemid, rsdp->oem_id, 6);
    strncpy(signature, rsdp->signature, 8);

    printf("RSDP at: 0x%llx Sig: %s Check: %d OEM: %s Rev: %d RsdtAddress 0x%llx\n",
        rsdp, signature, rsdp->checksum, oemid, rsdp->revision, rsdp->rsdt_address
    );

    struct rsdt* rsdt = (struct rsdt*)(uint64_t)(rsdp->rsdt_address);
    struct acpi_sdt_header* mcfg = find_rsdt(rsdt, "MCFG", 4);

    if (!acpi_sdt_checksum(mcfg))
        panic("RSDT checksum mismatch");

    return (void*)mcfg;
}

void init_acpi() {
    char signature[9];
    char oemid[7];
    char oemtableid[9];

    void* rsdp_address = (void*)get_rsdp_address(); //Bootloader
    struct rsdp_descriptor * prev_rsdp = (struct rsdp_descriptor*)rsdp_address;
    struct mcfg_header * mcfg_header = 0x0;

    if (prev_rsdp->revision == 0) {
        mcfg_header = (struct mcfg_header*)init_acpi_vz(rsdp_address);
    } else if (prev_rsdp->revision == 2) {
        mcfg_header = (struct mcfg_header*)init_acpi_vt(rsdp_address);
    } else {
        panic("RSDP Revision not supported");
    }

    strncpy(signature, mcfg_header->header.signature, 4);
    strncpy(oemtableid, mcfg_header->header.oem_table_id, 8);
    strncpy(oemid, mcfg_header->header.oem_id, 6);

    printf("MCFG at 0x%llx, Sig: %s, Len: 0x%llx Rev: %d Check: %d OEM: %s OTI: %s OR: %d CID: %d CREV: %d\n",
        mcfg_header, signature, mcfg_header->header.length, mcfg_header->header.revision, mcfg_header->header.checksum, oemid, oemtableid,
        mcfg_header->header.oem_revision, mcfg_header->header.creator_id, mcfg_header->header.creator_revision
    );

    enumerate_pci(mcfg_header);

}