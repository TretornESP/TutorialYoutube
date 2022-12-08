#include "device.h"
#include "../print/dbgprinter.h"
#include "../print/printf/printf.h"
#include "../memory/memory.h"
#include "../util/string.h"
#include "../drivers/ahci/ahci.h"


struct device_driver char_device_drivers[256] = {0};
struct device_driver block_device_drivers[256] = {0};
struct device * devices;

const char* pci_device_classes[] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station", 
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non Essential Instrumentation"
};

const char* get_vendor_name(uint16_t vendor_id) {
    switch (vendor_id) {
        case 0x8086:
            return "Intel";
        case 0x10de:
            return "nVidia";
        case 0x1022:
            return "AMD";
        case 0x1234:
            return "Bochs";
    }
    return itoa(vendor_id, 16);
}

const char* get_device_class(uint8_t class) {
    return pci_device_classes[class];
}

const char* get_device_name(uint16_t vendor_id, uint16_t device_id) {
    switch (vendor_id) {
        case 0x8086: {
            switch (device_id) {
                case 0x29c0:
                    return "Express DRAM Controller";
                case 0x2918:
                    return "LPC Interface Controller";
                case 0x2922:
                    return "SATA Controller [AHCI]";
                case 0x2930:
                    return "SMBus Controller";
                case 0x10d3:
                    return "Gigabit Network Connection";
            }
            break;
        }
        case 0x1234: {
            switch (device_id) {
                case 0x1111:
                    return "Virtual VGA Controller";
            }
            break;
        }
    }
    
    return itoa(device_id, 16);
}

const char * mass_storage_controller_subclass_name(uint8_t subclass_code) {
    switch (subclass_code) {
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA";
        case 0x07:
            return "Serial Attached SCSI";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other";
    }

    return itoa(subclass_code, 16);
}

const char* serial_bus_controller_subclass_name(uint8_t subclass_code){
    switch (subclass_code){
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus";
        case 0x06:
            return "Infiniband";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus";
        case 0x80:
            return "SerialBusController - Other";
    }
    return itoa(subclass_code, 16);
}

const char* bridge_device_subclass_name(uint8_t subclass_code){
    switch (subclass_code){
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge";
        case 0x0a:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other";
    }

    return itoa(subclass_code, 16);
}

const char * get_subclass_name(uint8_t class_code, uint8_t subclass_code) {
    switch (class_code) {
        case 0x01:
            return mass_storage_controller_subclass_name(subclass_code);
        case 0x03: {
            switch (subclass_code) {
                case 0x00:
                    return "VGA Compatible Controller";
            }
            break;
        }
        case 0x06:
            return bridge_device_subclass_name(subclass_code);
        case 0x0c:
            return serial_bus_controller_subclass_name(subclass_code);
    }
    return itoa(subclass_code, 16);
}

const char* get_prog_interface(uint8_t class_code, uint8_t subclass_code, uint8_t prog_interface){
    switch (class_code){
        case 0x01: {
            switch (subclass_code){
                case 0x06: {
                    switch (prog_interface){
                        case 0x00:
                            return "Vendor Specific Interface";
                        case 0x01:
                            return "AHCI 1.0";
                        case 0x02:
                            return "Serial Storage Bus";
                    }
                    break;
                }
            }
            break;
        }
        case 0x03: {
            switch (subclass_code) {
                case 0x00: {
                    switch (prog_interface){
                        case 0x00:
                            return "VGA Controller";
                        case 0x01:
                            return "8514-Compatible Controller";
                    }
                    break;
                }
            }
            break;
        }
        case 0x0C: {
            switch (subclass_code){
                case 0x03: {
                    switch (prog_interface){
                        case 0x00:
                            return "UHCI Controller";
                        case 0x10:
                            return "OHCI Controller";
                        case 0x20:
                            return "EHCI (USB2) Controller";
                        case 0x30:
                            return "XHCI (USB3) Controller";
                        case 0x80:
                            return "Unspecified";
                        case 0xFE:
                            return "USB Device (Not a Host Controller)";
                    }
                    break;
                }
            }    
            break;
        }
    }
    return itoa(prog_interface, 16);
}

void insert_device(uint8_t major, struct pci_device_header* pci, const char* prefix, uint8_t id) {
    char name[32];
    memset(name, 0 , 32);

    struct device * device = devices;
    uint8_t minor = 0; 

    while (device->next != 0) {
        if (device->major == major) {
            minor++;
        }
        device = device->next;
    }

    snprintf(name, 32, "%s%x", prefix, minor+0xa);

    printf("Registering device: %s [MAJ: %d MIN: %d ID: %d PCI: %p]\n", name, major, minor, id, pci);

    device->next = (struct device*) request_page();
    memset(device->next, 0, sizeof(struct device));
    device->valid = 1;
    device->bc = ((major & 0x80) >> 7);
    device->major = major & 0x7f;
    device->minor = minor;
    strncpy(device->name, name, 32);
    device->pci = pci;
    device->internal_id = id;
}

void register_device(struct pci_device_header * pci) {
    printf("Device detected: %x, %x, %x\n", pci->class_code, pci->subclass, pci->prog_if);
    switch(pci->class_code) {
        case 0x01: {
            switch (pci->subclass) {
                case 0x06: {
                    switch (pci->prog_if) {
                        case 0x01: {
                            init_ahci(pci);
                            for (int i = 0; i < get_port_count(); i++) {
                                switch(get_port_type(i)) {
                                    case PORT_TYPE_SATA:
                                        insert_device(0x8, pci, "/dev/hd", i);
                                        break;
                                    case PORT_TYPE_SATAPI:
                                        insert_device(0x9, pci, "/dev/cd", i);
                                        break;
                                    case PORT_TYPE_SEMB:
                                        insert_device(0xa, pci, "/dev/semb", i);
                                        break;
                                    case PORT_TYPE_PM:
                                        insert_device(0xb, pci, "/dev/pm", i);
                                        break;
                                    case PORT_TYPE_NONE:
                                        insert_device(0xc, pci, "/dev/unknown", i);
                                        break;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        break;
    }
}

void register_char(uint8_t major, const char* name, struct file_operations* fops) {
    if (char_device_drivers[major].registered) {
        printf("Char device driver %d is already registered, aborting!!\n", major);
        return;
    }
    strncpy(char_device_drivers[major].name, name, strlen(name));
    char_device_drivers[major].registered = 1;
    char_device_drivers[major].fops = fops;

    printf("Registered char device driver: %d\n", major);
}

void register_block(uint8_t major, const char* name, struct file_operations* fops) {
    if (block_device_drivers[major].registered) {
        printf("Block device driver %d is already registered, aborting!!\n", major);
        return;
    }
    strncpy(block_device_drivers[major].name, name, strlen(name));
    block_device_drivers[major].registered = 1;
    block_device_drivers[major].fops = fops;

    printf("Registered block device driver: %d\n", major);

}

void unregister_block(uint8_t major) {
    block_device_drivers[major].registered = 0;
}

void unregister_char(uint8_t major) {
    char_device_drivers[major].registered = 0;
}

void init_devices() {
    printf("### DEVICES STARTUP ###\n");
    devices = (struct device*)request_page();
    memset(devices, 0, sizeof(struct device));

    init_acpi();
}

void device_list() {
    struct device * dev = devices;
    while (dev->valid) {
        printf("Device: %s [MAJ: %d MIN: %d]\n", dev->name, dev->major, dev->minor);
        dev = dev->next;
    }
}

struct device* get_device_head() {
    if (devices->valid) {
        return devices;
    }
    return (struct device*)0x0;
}

uint32_t get_device_count() {
    uint32_t count = 0;
    struct device * dev = devices;

    while (dev->valid) {
        count++;
        dev = dev->next;
    }

    return count;
}

uint32_t get_device_count_by_major(uint8_t major) {
    uint32_t count = 0;
    struct device * dev = devices;

    while (dev->valid) {
        if (dev->major == major)
            count++;
        dev = dev->next;
    }

    return count;
}

struct device* get_next_device(struct device * dev) {
    if (dev->next->valid)
        return dev->next;
    return (struct device*)0x0;
}

struct device * device_search(const char* device) {
    struct device * dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device))  == 0) {
            return dev;
        }
        dev = dev -> next;
    }
    return (struct device*)0x0;
}

uint64_t device_read(const char* device, uint64_t size, uint64_t offset, uint8_t *buffer) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            } else {
                return char_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    }

    return 0;
}

uint64_t device_write(const char* device, uint64_t size, uint64_t offset, uint8_t *buffer) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            } else {
                return char_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    }

    return 0;
}

uint64_t device_ioctl(const char* device, uint32_t op, void* buffer) {
    struct device* dev = devices;
    do {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            } else {
                return char_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            }
        }
        dev = dev->next;
    } while(dev->valid);

    return 0;
}