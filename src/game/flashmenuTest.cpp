#include <menuSystem.h>
#include <types.h>
#include <GSgfx.h>
#include <winSprite.h>

extern "C" void FlashSystemSetLoopFlag(unsigned long flash, s32 loop);
extern "C" {
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
extern f32 flashTestTextureDimensions[2];
extern u16 flashTestTextureIDs[4];
extern bool flashTestSpriteVisibility[4];
extern bool flashTestSpriteSpecialVisibility;
extern u8 flashTestTexturesEnabled;
}

extern "C" s32 menuFlashTestSpriteControl(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    switch (sprite->mItemID) {
    case 0x155:
        winSpriteSetDisp(sprite, flashTestSpriteSpecialVisibility);
        break;
    case 0x68F:
        winSpriteSetDisp(sprite, flashTestSpriteVisibility[0]);
        break;
    case 0x690:
        winSpriteSetDisp(sprite, flashTestSpriteVisibility[1]);
        break;
    case 0x691:
        winSpriteSetDisp(sprite, flashTestSpriteVisibility[2]);
        break;
    case 0x692:
        winSpriteSetDisp(sprite, flashTestSpriteVisibility[3]);
        break;
    case 0x698:
        if (flashTestTexturesEnabled != 0) {
            windowDrawSprite(flashTestTextureDimensions[0], flashTestTextureDimensions[1], window, flashTestTextureIDs[0], 0, layer);
            windowDrawSprite(flashTestTextureDimensions[0], flashTestTextureDimensions[1], window, flashTestTextureIDs[1], 0, layer);
            windowDrawSprite(flashTestTextureDimensions[0], flashTestTextureDimensions[1], window, flashTestTextureIDs[2], 0, layer);
            windowDrawSprite(flashTestTextureDimensions[0], flashTestTextureDimensions[1], window, flashTestTextureIDs[3], 0, layer);
        }
        break;
    }
    return 0;
}



extern "C" void menuFlashTestClose(void)
{
    menuClose(0x4E);
}

extern "C" void menuFlashTestOpen(void)
{
    menuOpen(0x4E, 1);
}

#pragma force_active on
extern "C" void menuFlashTestInit(void)
{
    FlashSystemSetLoopFlag(0x17883000, 1);
}
#pragma force_active reset
