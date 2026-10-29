#include <string.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <hsd/fog.h>

typedef struct _HSD_CObj _HSD_CObj;

_HSD_CObj *HSD_CObjGetCurrent(void);
f32 HSD_CObjGetNear(_HSD_CObj *camera);
f32 HSD_CObjGetFar(_HSD_CObj *camera);
void HSD_Panic(const char *file, int line, const char *message);

/* The GX range adjustment is expressed in a 640 pixel wide field whose origin
 * is the left edge, while a fog adjustment offset is signed and centred. */
enum {
    HSD_FOG_ADJ_OFFSET_BIAS = 320
};

#define HSD_FOG_ADJ_FIELD_WIDTH 640.0f

/* Values applied when the fog is disabled and when a fog range adjustment
 * falls back to an orthographic projection matrix. */
static const f32 fogRangeAdjNone[1] = { 0.0f };
static const f32 fogRangeAdjUnit[1] = { 1.0f };

void HSD_FogSet(_HSD_Fog *fog)
{
    _HSD_CObj *camera;
    _HSD_FogAdj *adjust;
    GXFogAdjTable table;
    Mtx44 matrix;
    Mtx44Ptr projmtx;
    f32 viewport[6];
    f32 projection[7];
    u32 flags;
    s32 center;
    s32 width;
    f32 half = 0.5f;

    if (fog == NULL) {
        GXSetFog(GX_FOG_NONE, fogRangeAdjNone[0], fogRangeAdjNone[0],
                 fogRangeAdjNone[0], fogRangeAdjNone[0], (GXColor){0, 0, 0, 0});
        return;
    }

    camera = HSD_CObjGetCurrent();

    if (camera == NULL) {
        HSD_Panic("fog.c", 87, "You must specify CObj first.\n");
    }

    GXSetFog(fog->type, fog->start, fog->end, HSD_CObjGetNear(camera),
             HSD_CObjGetFar(camera), fog->color);

    adjust = (fog != NULL) ? fog->adjust : NULL;
    flags = (adjust != NULL) ? adjust->flags : 0;

    if (adjust != NULL && (flags & HSD_FOGADJ_FLAG_ANY) != 0) {
        GXGetViewportv(viewport);

        if (flags & HSD_FOGADJ_FLAG_CENTER) {
            center = (s32)(viewport[0] + viewport[2] *
                                              (f32)(((adjust != NULL) ? adjust->center : -1) +
                                                    HSD_FOG_ADJ_OFFSET_BIAS) /
                                              HSD_FOG_ADJ_FIELD_WIDTH);
        } else {
            center = (s32)(viewport[0] + viewport[2] * half);
        }

        if (flags & HSD_FOGADJ_FLAG_WIDTH) {
            width = (adjust != NULL) ? adjust->width : -1;
        } else {
            width = (s32)viewport[2];
        }

        if (flags & HSD_FOGADJ_FLAG_MTX) {
            projmtx = (adjust != NULL) ? adjust->mtx : NULL;
        } else {
            projmtx = matrix;
            memset(matrix, 0, sizeof(matrix));
            GXGetProjectionv(projection);

            switch ((s32)projection[0]) {
            case GX_PERSPECTIVE:
                matrix[0][0] = projection[1];
                matrix[0][2] = projection[2];
                matrix[1][1] = projection[3];
                matrix[1][2] = projection[4];
                matrix[2][2] = projection[5];
                matrix[2][3] = projection[6];
                matrix[3][2] = -1.0f;
                break;
            default:
                matrix[0][0] = projection[1];
                matrix[0][3] = projection[2];
                matrix[1][1] = projection[3];
                matrix[1][3] = projection[4];
                matrix[2][2] = projection[5];
                matrix[2][3] = projection[6];
                matrix[3][3] = fogRangeAdjUnit[0];
                break;
            }
        }

        GXInitFogAdjTable(&table, (u16)width, projmtx);
        GXSetFogRangeAdj(GX_TRUE, (u16)center, &table);
    } else {
        GXSetFogRangeAdj(GX_FALSE, 0, NULL);
    }
}
