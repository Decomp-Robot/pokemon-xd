#ifndef HSD_FOG_H
#define HSD_FOG_H

#include <types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct _HSD_FogAdj _HSD_FogAdj;
typedef struct _HSD_Fog _HSD_Fog;

/* Flags of a fog range adjustment. Each flag selects where the matching GX
 * range adjustment argument comes from; no flag disables the adjustment. */
#define HSD_FOGADJ_FLAG_CENTER 0x1
#define HSD_FOGADJ_FLAG_WIDTH 0x2
#define HSD_FOGADJ_FLAG_MTX 0x4
#define HSD_FOGADJ_FLAG_ANY \
    (HSD_FOGADJ_FLAG_CENTER | HSD_FOGADJ_FLAG_WIDTH | HSD_FOGADJ_FLAG_MTX)

struct _HSD_FogAdj {
    u8 _00[0x08];
    u32 flags;  // 0x08
    s16 center; // 0x0C: signed offset from the middle of the 640 pixel field.
    u16 width;  // 0x0E
    Mtx44 mtx;  // 0x10
};

struct _HSD_Fog {
    u8 _00[0x08];
    GXFogType type;      // 0x08
    _HSD_FogAdj *adjust; // 0x0C
    f32 start;           // 0x10
    f32 end;             // 0x14
    GXColor color;       // 0x18
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_FogSet(_HSD_Fog *fog);
#ifdef __cplusplus
}
#endif

#endif
