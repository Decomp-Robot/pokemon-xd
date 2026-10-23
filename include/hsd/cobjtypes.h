#ifndef HSD_COBJ_TYPES_H
#define HSD_COBJ_TYPES_H

#include <types.h>

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

struct _HSD_CObjDesc {
    const char *className;
    u16 flags;
    u16 projectionType;
    s16 viewportLeft;
    s16 viewportRight;
    s16 viewportTop;
    s16 viewportBottom;
    u16 scissorLeft;
    u16 scissorRight;
    u16 scissorTop;
    u16 scissorBottom;
    _HSD_WObjDesc *eye;
    _HSD_WObjDesc *interest;
    f32 roll;
    _HSD_Vec3 *up;
    f32 nearPlane;
    f32 farPlane;
    f32 fov;
    f32 aspect;
};

#endif
