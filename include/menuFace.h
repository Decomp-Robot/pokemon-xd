#ifndef MENUFACE_H
#define MENUFACE_H

#include <types.h>
#include <GSvec.h>

struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

extern "C" {
void menuFaceDispFromID3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 faceID,
                       GSivec2 *size, GSivec2 *position);
}

#endif
