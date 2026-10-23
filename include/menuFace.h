#ifndef MENUFACE_H
#define MENUFACE_H

#include <Pokemon.h>
#include <types.h>
#include <winSprite.h>

struct GSvec2;

#ifdef __cplusplus
extern "C" {
#endif
u8 menuFaceDispFromID2(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID, struct GSvec2* position);
u8 menuFaceDisp(struct tagWINDOW_WORK* window, struct tagSPRITE_WORK* sprite, struct Pokemon* pokemon);

typedef Pokemon *(*MenuFaceLoadCallback)(void *context);
void menuFaceLoad(Pokemon *pokemon, MenuFaceLoadCallback callback, void *context);
u8 menuFaceCheckLoading(void);
u32 menuFaceGetID(Pokemon *pokemon);
s8 menuFaceSearchEnable(u32 faceId);

#ifdef __cplusplus
}
#endif

#endif
