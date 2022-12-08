#ifndef DISK_INTERFACE_H
#define DISK_INTERFACE_H
#include <stdint.h>
uint8_t disk_get_status(const char * disk);
uint8_t disk_initialize(const char * disk);
uint8_t disk_read(const char * disk, uint8_t * buffer, uint32_t lba, uint32_t count);
uint8_t disk_write(const char * disk, uint8_t * buffer, uint32_t lba, uint32_t count);
uint8_t disk_ioctl(const char * disk, uint32_t op, void * buffer);

#endif