#ifndef MENUFACE_H
#define MENUFACE_H

#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Pokemon *(*MenuFaceLoadCallback)(void *context);
void menuFaceLoad(Pokemon *pokemon, MenuFaceLoadCallback callback, void *context);
u32 menuFaceGetID(Pokemon *pokemon);
s8 menuFaceSearchEnable(u32 faceId);

#ifdef __cplusplus
}
#endif

#endif
