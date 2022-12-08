#ifndef _DRIVER_IF_H
#define _DRIVER_IF_H
struct driver_package {
    void (*init)(void);
    void (*exit)(void);
};
#endif