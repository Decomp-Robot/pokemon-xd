#include <string.h>
#include <dolphin/types.h>
#include <GSmemman.h>
#include <hsd/assert.h>
#include <hsd/fobj.h>
#include <hsd/objalloc.h>
#include <hsd/spline.h>

typedef enum _FObjOp {
    FOBJ_OP_NONE = 0, FOBJ_OP_STEP = 1, FOBJ_OP_LINEAR = 2,
    FOBJ_OP_HERMITE = 3, FOBJ_OP_HERMITE2 = 4, FOBJ_OP_TANGENT = 5,
    FOBJ_OP_DISCRETE = 6
} FObjOp;
typedef enum _FObjFormat {
    FOBJ_FORMAT_FLOAT = 0, FOBJ_FORMAT_S16 = 0x20, FOBJ_FORMAT_U16 = 0x40,
    FOBJ_FORMAT_S8 = 0x60, FOBJ_FORMAT_U8 = 0x80
} FObjFormat;
typedef enum _FObjFlags {
    FOBJ_FLAG_SLOPE_DIRTY = 0x20,
    FOBJ_FLAG_DISCRETE_PENDING = 0x40,
    FOBJ_FLAG_DISCRETE_UPDATE = 0x80
} FObjFlags;

/* Allocator state shared by every HSD_FObj of the process. */
static HSD_ObjAllocData alloc_data;

static inline u32 FObjReadPackNb(HSD_FObj* fobj, u8 c)
{
    u32 nb_pack;
    s32 shift;

    nb_pack = ((c >> 4) & 0x7) + 1;
    shift = 3;
    if (!(c & 0x80)) {
        return nb_pack;
    }
    do {
        c = *fobj->ad++;
        nb_pack += (c & 0x7F) << shift;
        shift += 7;
    } while (c & 0x80);
    return nb_pack;
}

/* Reads a pack length stored as a base 128 number in the data stream. */
static inline u32 FObjReadPackLen(HSD_FObj* fobj)
{
    u32 nb_pack = 0;
    s32 shift = 0;
    u8 c;

    do {
        c = *fobj->ad++;
        nb_pack |= (c & 0x7F) << shift;
        shift += 7;
    } while (c & 0x80);
    return nb_pack;
}

/* Reads one scalar of the current pack from the animation data stream.
 * The high three bits of mode select the storage format and, for the fixed
 * point formats, the low five bits hold the number of fractional bits. */

static inline f32 FObjReadValue(HSD_FObj* fobj, u8 mode)
{
    union {
        u32 integer;
        f32 scalar;
    } conv;
    f32 fval;
    s32 scale;

    if (mode == FOBJ_FORMAT_FLOAT) {
        conv.integer = *fobj->ad++;
        conv.integer |= (u32)*fobj->ad++ << 8;
        conv.integer |= (u32)*fobj->ad++ << 16;
        conv.integer |= (u32)*fobj->ad++ << 24;
        return conv.scalar;
    }

    scale = 1 << (mode & 0x1F);
    switch (mode & 0xE0) {
    case FOBJ_FORMAT_S8:
        fval = (s8)*fobj->ad;
        fobj->ad++;
        break;
    case FOBJ_FORMAT_U8:
        fval = *fobj->ad;
        fobj->ad++;
        break;
    case FOBJ_FORMAT_S16:
        fval = ((s8)fobj->ad[1] << 8 | fobj->ad[0]);
        fobj->ad += 2;
        break;
    case FOBJ_FORMAT_U16:
        fval = (fobj->ad[1] << 8 | fobj->ad[0]);
        fobj->ad += 2;
        break;
    default:
        return 0.0f;
    }
    return fval / scale;
}

/* Evaluates the current pack at the position of fobj and hands the result to
 * the update callback of the animated object. */

static inline void FObjUpdateValue(HSD_FObj* fobj, void* object, HSD_AnimUpdate update,
                                   HSD_ObjData* value)
{
    switch (fobj->op_intrp) {
    case FOBJ_OP_DISCRETE:
        if ((fobj->flags & FOBJ_FLAG_DISCRETE_UPDATE) == 0) {
            return;
        }
        value->scalar = fobj->a;
        fobj->flags &= ~FOBJ_FLAG_DISCRETE_UPDATE;
        break;
    case FOBJ_OP_STEP:
        value->scalar = (fobj->frame >= fobj->pack) ? fobj->b : fobj->a;
        break;
    case FOBJ_OP_LINEAR:
        if (fobj->flags & FOBJ_FLAG_SLOPE_DIRTY) {
            fobj->flags &= ~FOBJ_FLAG_SLOPE_DIRTY;
            if (fobj->pack != 0) {
                fobj->s = (fobj->b - fobj->a) / fobj->pack;
            } else {
                fobj->s = 0.0f;
                fobj->a = fobj->b;
            }
        }
        value->scalar = fobj->s * fobj->frame + fobj->a;
        break;
    case FOBJ_OP_HERMITE:
    case FOBJ_OP_HERMITE2:
    case FOBJ_OP_TANGENT:
        if (fobj->pack != 0) {
            value->scalar = splGetHelmite(1.0 / fobj->pack, fobj->frame,
                                          fobj->a, fobj->b, fobj->s, fobj->t);
        } else {
            value->scalar = fobj->b;
        }
        break;
    default:
        return;
    }
    update(object, fobj->chan, value);
}

static inline FObjLoadState FObjGetStat(HSD_FObj* fobj)
{
    return (fobj == NULL) ? FOBJ_LOAD_NONE : (FObjLoadState)(fobj->flags & 0xF);
}

static inline void FObjSetStat(HSD_FObj* fobj, FObjLoadState stat)
{
    if (fobj != NULL) {
        fobj->flags = (fobj->flags & 0xF0) | (stat & 0xF);
    }
}

HSD_ObjAllocData* HSD_FObjGetAllocData(void)
{
    return &alloc_data;
}

void HSD_FObjInitAllocData(void)
{
    HSD_ObjAllocInit(&alloc_data, sizeof(HSD_FObj), 4);
}

void HSD_FObjRemove(HSD_FObj* fobj)
{
    if (fobj != NULL) {
        HSD_FObjFree(fobj);
    }
}

void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    if (fobj != NULL) {
        HSD_FObjRemoveAll(fobj->next);
        HSD_FObjRemove(fobj);
    }
}

void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 frame)
{
    if (fobj != NULL) {
        for (; fobj != NULL; fobj = fobj->next) {
            if (fobj == NULL) {
                continue;
            }
            fobj->ad = fobj->bd;
            fobj->frame = fobj->start + frame;
            fobj->op = FOBJ_OP_NONE;
            fobj->op_intrp = FOBJ_OP_NONE;
            fobj->flags &= ~FOBJ_FLAG_DISCRETE_PENDING;
            fobj->nb_pack = 0;
            fobj->pack = 0;
            fobj->a = 0.0f;
            fobj->b = 0.0f;
            fobj->s = 0.0f;
            fobj->t = 0.0f;
            FObjSetStat(fobj, FOBJ_LOAD_DATA0);
        }
    }
}

void HSD_FObjStopAnimAll(HSD_FObj* fobj, void* object, HSD_AnimUpdate update,
                         f32 rate)
{
    for (; fobj != NULL; fobj = fobj->next) {
        if (fobj == NULL) {
            continue;
        }
        if (fobj->op_intrp == FOBJ_OP_DISCRETE) {
            HSD_FObjInterpretAnim(fobj, object, update, rate);
        }
        FObjSetStat(fobj, FOBJ_LOAD_NONE);
    }
}

FObjLoadState FObjLoadData(HSD_FObj* fobj)
{
    u8 c;
    FObjLoadState st;

    if ((u32)(fobj->ad - fobj->bd) >= fobj->size) {
        return FOBJ_LOAD_END;
    }

    fobj->op_intrp = fobj->op;
    if (fobj->nb_pack == 0) {
        fobj->op = *fobj->ad & 0xF;
        c = *fobj->ad++;
        fobj->nb_pack = FObjReadPackNb(fobj, c);
    }
    fobj->nb_pack--;

    switch (fobj->op) {
    case FOBJ_OP_STEP:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(388, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        fobj->a = fobj->b;
        fobj->b = FObjReadValue(fobj, fobj->mode);
        if (fobj->op_intrp != FOBJ_OP_TANGENT) {
            fobj->s = fobj->t;
            fobj->t = 0.0f;
        }
        st = (st == FOBJ_LOAD_DATA0) ? (s32)FOBJ_LOAD_WAIT : (s32)FOBJ_LOAD_FRAME;
        FObjSetStat(fobj, st);
        return st;
    case FOBJ_OP_LINEAR:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(408, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        fobj->a = fobj->b;
        fobj->b = FObjReadValue(fobj, fobj->mode);
        if (fobj->op_intrp != FOBJ_OP_TANGENT) {
            fobj->s = fobj->t;
            fobj->t = 0.0f;
        }
        st = (st == FOBJ_LOAD_DATA0) ? (s32)FOBJ_LOAD_WAIT : (s32)FOBJ_LOAD_FRAME;
        FObjSetStat(fobj, st);
        return st;
    case FOBJ_OP_HERMITE:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(428, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        fobj->a = fobj->b;
        fobj->s = fobj->t;
        fobj->b = FObjReadValue(fobj, fobj->mode);
        fobj->t = 0.0f;
        st = (st == FOBJ_LOAD_DATA0) ? (s32)FOBJ_LOAD_WAIT : (s32)FOBJ_LOAD_FRAME;
        FObjSetStat(fobj, st);
        return st;
    case FOBJ_OP_HERMITE2:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(446, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        fobj->a = fobj->b;
        fobj->b = FObjReadValue(fobj, fobj->mode);
        fobj->s = fobj->t;
        fobj->t = FObjReadValue(fobj, fobj->slope_mode);
        st = (st == FOBJ_LOAD_DATA0) ? (s32)FOBJ_LOAD_WAIT : (s32)FOBJ_LOAD_FRAME;
        FObjSetStat(fobj, st);
        return st;
    case FOBJ_OP_TANGENT:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(465, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        fobj->s = fobj->t;
        fobj->t = FObjReadValue(fobj, fobj->slope_mode);
        return FObjGetStat(fobj);
    case FOBJ_OP_DISCRETE:
        st = FObjGetStat(fobj);
        HSD_ASSERTLINE(494, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);
        if (fobj->flags & FOBJ_FLAG_DISCRETE_PENDING) {
            fobj->op_intrp = fobj->op;
            fobj->flags &= ~FOBJ_FLAG_DISCRETE_PENDING;
            fobj->flags |= FOBJ_FLAG_DISCRETE_UPDATE;
            fobj->a = fobj->b;
        }
        fobj->b = FObjReadValue(fobj, fobj->mode);
        fobj->flags |= FOBJ_FLAG_DISCRETE_PENDING;
        st = (st == FOBJ_LOAD_DATA0) ? (s32)FOBJ_LOAD_WAIT : (s32)FOBJ_LOAD_FRAME;
        FObjSetStat(fobj, st);
        return st;
    default:
        return FOBJ_LOAD_NONE;
    }
}

void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* object, HSD_AnimUpdate update,
                            f32 rate)
{
    f32 duration;
    u32 st;

    duration = 0.0f;
    st = (fobj != NULL) ? (u32)FObjGetStat(fobj) : FOBJ_LOAD_NONE;
    if (st == FOBJ_LOAD_NONE) {
        return;
    }

    fobj->frame += rate;
    if (fobj->frame < 0.0) {
        return;
    }

    while (1) {
        switch (st) {
        case FOBJ_LOAD_END: {
            HSD_ObjData value;

            fobj->frame += duration;
            if (fobj->flags & FOBJ_FLAG_DISCRETE_PENDING) {
                fobj->op_intrp = fobj->op;
                fobj->flags &= ~FOBJ_FLAG_DISCRETE_PENDING;
                fobj->flags |= FOBJ_FLAG_DISCRETE_UPDATE;
                fobj->a = fobj->b;
            }
            if (update != NULL) {
                FObjUpdateValue(fobj, object, update, &value);
            }
            return;
        }
        case FOBJ_LOAD_DATA0:
        case FOBJ_LOAD_DATA:
            st = FObjLoadData(fobj);
            break;
        case FOBJ_LOAD_WAIT:
            if ((fobj->flags & FOBJ_FLAG_DISCRETE_UPDATE) && update != NULL) {
                HSD_ObjData value;

                FObjUpdateValue(fobj, object, update, &value);
            }
            st = FObjGetStat(fobj);
            HSD_ASSERTLINE(369, st == FOBJ_LOAD_WAIT);
            if ((u32)(fobj->ad - fobj->bd) >= fobj->size) {
                st = FOBJ_LOAD_END;
                break;
            }
            fobj->pack = FObjReadPackLen(fobj);
            fobj->flags |= FOBJ_FLAG_SLOPE_DIRTY;
            FObjSetStat(fobj, FOBJ_LOAD_DATA);
            st = FOBJ_LOAD_DATA;
            break;
        case FOBJ_LOAD_FRAME: {
            HSD_ObjData value;

            if (fobj->pack <= fobj->frame) {
                duration = fobj->pack;
                fobj->frame -= fobj->pack;
                FObjSetStat(fobj, st = FOBJ_LOAD_WAIT);
                break;
            }
            if (update != NULL) {
                FObjUpdateValue(fobj, object, update, &value);
            }
            FObjSetStat(fobj, FOBJ_LOAD_NEXT);
            return;
        }
        case FOBJ_LOAD_NEXT:
            FObjSetStat(fobj, st = FOBJ_LOAD_FRAME);
            break;
        case FOBJ_LOAD_NONE:
            return;
        }
    }
}

void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* object,
                               HSD_AnimUpdate update, f32 rate)
{
    HSD_FObj* next;

    for (next = fobj; next != NULL; next = next->next) {
        HSD_FObjInterpretAnim(next, object, update, rate);
    }
}

/* Debug helper of the object allocator, empty in the release library. */
void dumpObjAlloc(HSD_ObjAllocData* data, const char* message)
{
}

HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    HSD_FObj* fobj;

    if (desc) {
        fobj = HSD_FObjAlloc();
        fobj->next = HSD_FObjLoadDesc(desc->next);
        fobj->start = (s16)desc->start;
        fobj->chan = desc->chan;
        fobj->mode = desc->mode;
        fobj->slope_mode = desc->slope_mode;
        fobj->bd = desc->data;
        fobj->size = desc->size;
        fobj->flags = 0;
        return fobj;
    }
    return NULL;
}

HSD_FObj* HSD_FObjAlloc(void)
{
    HSD_FObj* new;
    GSmemPoolAllocator* pool;

    pool = GSmemGetSysdolAllocator(sizeof(HSD_FObj));
    if (pool && GSmemGetFreePool(pool)) {
        new = GSmemAllocPool(pool);
        HSD_ASSERTLINE(767, new);
        return new;
    }
    new = HSD_ObjAlloc(&alloc_data);
    dumpObjAlloc(&alloc_data, "fobj: alloc");
    HSD_ASSERTLINE(774, new);
    memset(new, 0, sizeof(HSD_FObj));
    return new;
}

void HSD_FObjFree(HSD_FObj* fobj)
{
    GSmemPoolAllocator* pool;

    pool = GSmemGetSysdolAllocator(sizeof(HSD_FObj));
    if (pool && GSmemIsRegistrationPool(pool, fobj)) {
        GSmemFreePool(pool, fobj);
    } else {
        HSD_ObjFree(&alloc_data, fobj);
        dumpObjAlloc(&alloc_data, "fobj: free");
    }
}
