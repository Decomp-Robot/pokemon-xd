#ifndef HSD_ROBJ_H
#define HSD_ROBJ_H

#include <types.h>
#include <hsd/forward.h>

struct _HSD_RObj {
    _HSD_RObj *mNext;
    u8 _04[20];
    _HSD_AObj *mAObj;
};

#endif
