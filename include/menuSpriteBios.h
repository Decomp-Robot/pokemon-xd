#ifndef MENU_SPRITE_BIOS_H
#define MENU_SPRITE_BIOS_H

#include <types.h>

struct MenuSpriteValue {
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    u32 mDrawValue;
};

struct MenuSpriteData {
    u8 _00[8];
    struct MenuSpriteValue mValue[5];
    s16 mNextID;
    u8 _46[2];
};

#ifdef __cplusplus
extern "C" {
#endif

struct MenuSpriteData *menuSpriteBiosGetPtr(u32 spriteID);
struct MenuSpriteValue *menuSpriteBiosGetValPtr(struct MenuSpriteData *data);

#ifdef __cplusplus
}
#endif

#endif
