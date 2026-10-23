#ifndef GS_GFX_TYPES_H
#define GS_GFX_TYPES_H
#include <types.h>
typedef enum GSgfxLayerID { GSGFX_LAYER_INVALID = -1, GSGFX_LAYER_DEFAULT = 2, GSGFX_LAYER_MENU = 7 } GSgfxLayerID;
typedef struct GSgfxColAu8 { u8 r, g, b, a; } GSgfxColAu8;
#ifdef __cplusplus
extern const GSgfxColAu8 GSgfxColAu8_Black;
#endif
#endif
