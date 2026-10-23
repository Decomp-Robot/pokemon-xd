#ifndef CAMERA_H
#define CAMERA_H

#include <types.h>

enum {
    CAMERA_TYPE_DEFAULT = 0,
    CAMERA_TYPE_DEBUG_PRIMARY = 5,
    CAMERA_TYPE_DEBUG_SECONDARY = 6
};

#ifdef __cplusplus
extern "C" {
#endif

u8 cameraSetType(u8 type);
void cameraPlayAnime(u32 groupID, u32 resourceID, f32 frame, u8 loop);

#ifdef __cplusplus
}
#endif

#endif
