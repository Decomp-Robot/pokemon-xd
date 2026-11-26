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
    u32 mFlags;
};

#define HSD_DOBJ_HIDDEN 0x1
#define HSD_DOBJ_TRSP_SHIFT 1

#ifdef __cplusplus
extern "C" {
#endif
void HSD_DObjSetCurrent(_HSD_DObj *dobj);
#ifdef __cplusplus
}
#endif

#endif
