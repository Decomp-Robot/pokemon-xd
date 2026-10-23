#ifndef GSTEXTURE_H
#define GSTEXTURE_H
#include <types.h>
struct GStexture;
enum GStextureWrapMode { GS_TEXTURE_WRAP_CLAMP = 0 };
enum GStextureMipFilterMode { GS_TEXTURE_MIP_FILTER_NONE = 0 };

#ifdef __cplusplus
extern "C" {
#endif
struct GStexture *GStextureLoad(void *data);
void GStextureFree(struct GStexture *texture);
void GStextureSetWrap(struct GStexture *texture, s32 wrapU, s32 wrapV);
void GStextureSetFilter(struct GStexture *texture, s32 minFilter, s32 magFilter, s32 mipFilter);
u16 GStextureGetXsize(struct GStexture *texture);
u16 GStextureGetYsize(struct GStexture *texture);
#ifdef __cplusplus
}
#endif
#endif
