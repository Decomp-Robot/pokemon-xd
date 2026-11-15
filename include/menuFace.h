#ifndef MENUFACE_H
#define MENUFACE_H

#include <Pokemon.h>
#include <types.h>
#include <winSprite.h>

struct GSvec2;
struct GSivec2;
struct FightPokemon;

#ifdef __cplusplus
extern "C" {
#endif

typedef Pokemon *(*MenuFaceLoadCallback)(void *context);
u8 menuFaceLoadFightPokemon(struct FightPokemon *pokemon, MenuFaceLoadCallback callback, void *context);
u8 menuFaceLoad_Tool(Pokemon *pokemon, MenuFaceLoadCallback callback, void *context);
u8 menuFaceLoad(Pokemon *pokemon, MenuFaceLoadCallback callback, void *context);
u8 menuFaceLoadFromID_Tool(u16 faceID, MenuFaceLoadCallback callback, void *context);
u8 menuFaceLoadFromID(u16 faceID, MenuFaceLoadCallback callback, void *context);
u8 menuFaceCheckLoading(void);
u8 menuFaceDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, Pokemon *pokemon);
u8 menuFaceDispFromID3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 faceID, struct GSivec2 *size, struct GSivec2 *position);
u8 menuFaceDispFromID2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 faceID, struct GSvec2 *position);
u8 menuFaceDispFromID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 faceID);
u32 menuFaceGetTexID(u16 faceID);
u16 menuFaceGetID3(Pokemon *pokemon, u8 deoxysSel, s8 rare);
u16 menuFaceGetID2(Pokemon *pokemon, u8 deoxysSel);
u16 menuFaceGetID(Pokemon *pokemon);
s8 menuFaceSearchEnable(u16 faceID);
void menuFaceAllocMemory(void);
void menuFaceInit(s32 num);

#ifdef __cplusplus
}
#endif
#endif
