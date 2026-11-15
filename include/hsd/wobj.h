#ifndef HSD_WOBJ_H
#define HSD_WOBJ_H

#include <types.h>
#include <hsd/forward.h>

struct _HSD_WObj {
    u8 _00[24];
    _HSD_AObj *mAObj;
    _HSD_RObj *mRObj;
};

struct _HSD_Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct _HSD_WObjDesc {
    const char *className;
    _HSD_Vec3 position;
    void *constraints;
};

#endif
