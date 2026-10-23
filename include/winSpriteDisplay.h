#ifndef WIN_SPRITE_DISPLAY_H
#define WIN_SPRITE_DISPLAY_H

#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
void winSpriteSetDisp(tagSPRITE_WORK *sprite, bool visible);
#else
void winSpriteSetDisp(tagSPRITE_WORK *sprite, u8 visible);
#endif

#ifdef __cplusplus
}
#endif

#endif
