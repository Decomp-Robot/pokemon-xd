#ifndef HSD_AOBJ_H
#define HSD_AOBJ_H

#include <types.h>
#include <hsd/assert.h>

typedef struct _HSD_AObj _HSD_AObj;
typedef struct _HSD_AObjDesc _HSD_AObjDesc;

struct _HSD_AObj {
    u32 mFlags;
    f32 mFrame;
    f32 mRewindFrame;
    f32 mEndFrame;
};

struct _HSD_AObjDesc {
    u32 mFlags;
    f32 mEndFrame;
};


#define HSD_AOBJ_FLAG_NO_ANIM 0x40000000u
#define HSD_AOBJ_FLAG_LOOP 0x20000000u

#ifdef __cplusplus
extern "C" {
#endif
void HSD_AObjSetRate(_HSD_AObj *aobj, f32 rate);
void HSD_AObjSetFlags(_HSD_AObj *animation, u32 flags);
void HSD_AObjClearFlags(_HSD_AObj *animation, u32 flags);
u32 HSD_AObjGetFlags(_HSD_AObj *animation);

#line 170 "aobj.h"
inline f32 HSD_AObjGetEndFrame(_HSD_AObj *aobj)
{
    HSD_ASSERT(aobj);
    return aobj->mEndFrame;
}

#ifdef __cplusplus
}
#endif

#endif
