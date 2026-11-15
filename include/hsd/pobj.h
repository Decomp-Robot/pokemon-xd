#ifndef HSD_POBJ_H
#define HSD_POBJ_H

#include <types.h>
#include <dolphin/gx/GXEnum.h>
#include <hsd/forward.h>

#define HSD_POBJ_TYPE_MASK 0x3000
#define HSD_POBJ_SHAPE_ANIM 0x1000

enum HSDVertexDescriptorConstants {
    HSD_VTX_ATTR_POSITION = 9,
    HSD_VTX_ATTR_NULL = 255,
    HSD_VTX_INPUT_DIRECT = 1
};

struct _HSD_VtxDescList {
    GXAttr mAttribute;
    GXAttrType mInputType;
    GXCompCnt mComponentCount;
    GXCompType mComponentFormat;
    u8 mFractionalBits;
    u8 _11;
    u16 mStride;
    void *mArrayBase;
};

/* A primitive object: one display list plus its vertex description. */
struct _HSD_PObj {
    u8 _00[4];
    _HSD_PObj *mNext;
    _HSD_VtxDescList *mVtxAttr;
    u16 mFlags;
    u16 mDisplayListBlocks;
    void *mDisplayList;
    u8 _14[4];
    _HSD_AObj *mAObj;
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_ClearVtxDesc(void);
#ifdef __cplusplus
}
#endif

#endif
