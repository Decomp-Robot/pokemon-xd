#ifndef HSD_OBJECT_H
#define HSD_OBJECT_H

#include <types.h>
#include <hsd/forward.h>
#include <hsd/assert.h>

struct _HSD_Obj {
    _HSD_ClassInfo *class_info;
    u16 ref_count;
    u16 ref_count_individual;
};

#define HSD_OBJ(o) ((_HSD_Obj *)(o))
#define HSD_OBJ_NOREF 0xFFFF

static inline void HSD_ObjRef(void *o)
{
    ++HSD_OBJ(o)->ref_count;
    if (HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF) {
        __assert(__FILE__, 93, "HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF");
    }
}

#endif
