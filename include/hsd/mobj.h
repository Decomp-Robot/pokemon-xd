#ifndef HSD_MOBJ_H
#define HSD_MOBJ_H

#include <types.h>
#include <hsd/forward.h>

struct _HSD_MatAnim {
    _HSD_MatAnim *mNext;
    _HSD_AObjDesc *mAnimation;
    _HSD_TexAnim *mTexAnim;
};

struct _HSD_MObj {
    u8 _00[8];
    _HSD_TObj *mTObj;
    u8 _0C[8];
    _HSD_AObj *mAObj;
};

#endif
