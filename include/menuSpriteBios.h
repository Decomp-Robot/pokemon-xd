#ifndef MENU_SPRITE_BIOS_H
#define MENU_SPRITE_BIOS_H

#include <types.h>

// A sprite's texture rectangle and resource for one language.
struct MenuSpriteImage {
    short mX, mY, mWidth, mHeight;
    unsigned long mResourceID;
};

// The BIOS chooses a language image from the sprite's table entry.
struct MenuSpriteData;

#ifdef __cplusplus
extern "C" {
#endif

struct MenuSpriteData* menuSpriteBiosGetPtr(unsigned long spriteID);
struct MenuSpriteImage* menuSpriteBiosGetValPtr(struct MenuSpriteData* spriteData);

#ifdef __cplusplus
}
#endif

#endif
