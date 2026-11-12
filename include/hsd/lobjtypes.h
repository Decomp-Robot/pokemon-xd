#ifndef HSD_LOBJ_TYPES_H
#define HSD_LOBJ_TYPES_H

#include <types.h>
#include <dolphin/gx/GXStruct.h>
#include <hsd/cobjtypes.h>

struct _HSD_ClassInfo;
struct _HSD_LObj;
struct _HSD_LObjAnimAll;
struct _HSD_WObj;

enum HSD_LObjFlags {
    HSD_LOBJ_TYPE_MASK = 0x3,
    HSD_LOBJ_DIFFUSE = 0x4
};

struct _HSD_LObjDesc {
    const char *className;
    _HSD_LObjDesc *next;
    u16 flags;
    u16 attenuationFlags;
    GXColor color;
    _HSD_WObjDesc *position;
    _HSD_WObjDesc *interest;
    f32 *attenuation;
};

struct _HSD_LObj {
    _HSD_ClassInfo *classInfo;
    u16 referenceCount;
    u16 reserved;
    u16 flags;
    u8 _0A[0x02];
    _HSD_LObj *next;
    GXColor color;
    u8 _14[0x04];
    _HSD_WObj *position;
    _HSD_WObj *interest;
};

#endif
