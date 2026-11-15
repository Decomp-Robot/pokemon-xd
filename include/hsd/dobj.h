#ifndef HSD_DOBJ_H
#define HSD_DOBJ_H

#include <types.h>
#include <hsd/forward.h>

struct _HSD_DObj {
    u8 _00[4];
    _HSD_DObj *mNext;
    _HSD_MObj *mMaterial;
    _HSD_PObj *mPrimitive;
    _HSD_AObj *mAObj;
};

#endif
