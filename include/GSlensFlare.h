#ifndef GS_LENS_FLARE_H
#define GS_LENS_FLARE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 GSlensFlareLoad(void *data);
void GSlensFlareFree(u32 handle);

#ifdef __cplusplus
}
#endif

#endif
