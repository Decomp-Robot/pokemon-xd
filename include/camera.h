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
// Sets the camera type and returns the previous type.
u8 cameraSetType(u8 cameraType);
void cameraPlayAnime(s32 resourceGroup, unsigned long resourceId,
                 f32 startFrame, s32 loop);
#ifdef __cplusplus
}
#endif

#endif
