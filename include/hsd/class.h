#ifndef HSD_CLASS_H
#define HSD_CLASS_H

#include <types.h>

struct _HSD_ClassInfo {
    u8 _00[0x30];
    void (*release)(void *object);
    void (*destroy)(void *object);
};

#endif
