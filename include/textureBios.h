#ifndef TEXTURE_BIOS_H
#define TEXTURE_BIOS_H

#include <GSgfx.h>
#include <winSprite.h>

struct TextureBiosLanguageEntry {
    s16 x, y;
    s16 width, height;
    u32 textureHandle;
};

extern "C" {

s16 *textureBiosGetDataTable(s32 textureID);
s16 *textureBiosGetLanguageEntry(s16 *table);
void menuCB_SetSprite(tagSPRITE_WORK *sprite, s16 *table);

void textureBiosLoadSpriteId(s32 x, s32 y, s32 width, s32 height, u32 color,
                             tagWINDOW_WORK *window, u16 textureID, s32 flipFlags,
                             GSgfxLayerID layer);

}

#endif
