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

enum MenuSpriteValueType {
    MENU_SPRITE_VALUE_TEXTURE = 1,
    MENU_SPRITE_VALUE_PRIMITIVE = 2,
    MENU_SPRITE_VALUE_FLASH = 3
};

struct MenuSpriteData {
    u16 mChainEndFlag : 1;
    u16 mUseWindowSize : 1;
    u16 mValueType : 2;
    u16 mSequenceID : 12;
    u8 mReservedAfterBits;
    s8 mOffsetX;
    s8 mOffsetY;
    u8 mAlpha;
    u8 mReservedAfterAlpha[2];
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
