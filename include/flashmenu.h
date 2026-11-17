#ifndef FLASH_MENU_H
#define FLASH_MENU_H

#include <types.h>
#include <flashcalc.h>
#include <GSgfxTypes.h>

struct GStexture;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;
struct _PLACEOBJECT2;

struct FlashDisplayObject;

struct FlashSpriteState {
    u16 frameCount;
    u16 unknown;
    FlashDisplayObject* displayList;
};

struct FlashMovieHeader {
    u8 unknownMovieHeader[6];
    u16 frameCount;
    u8 unknownMovieHeader2[16];
    u16 spriteCapacity;
    u16 depthCapacity;
    u8 unknownMovieData[20];
};

struct FlashDefinition {
    u8 kind;
    u8 unknownPadding[3];
    void* data;
};

struct FlashDisplayObject {
    u16 characterId;
    FLASHMATRIX matrix;
    f32 morphRatio;
    FLASHCXFORM colorTransform;
    void* unknownPlacement;
    _PLACEOBJECT2* placement;
    u16* spriteFrames;
    u8 placementFlags;
};

struct FlashFrameLabel {
    u16 frame;
    char* name;
    FlashFrameLabel* next;
};

struct FlashTextureBinding {
    s32 characterId;
    GStexture* texture;
};

class FlashMenu {
public:
    FlashMenu();
    ~FlashMenu();
    s32 Init(void* resource, u32 resourceID);
    void Release();
    void Update();
    void Draw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    void DrawSub(GSgfxLayerID layer, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    GStexture* GetGStexture(s32 textureID);
    u16 GetFrameFromLabel(char* label);
    u32 GetPlayFrame();
    void SetPlayFrame(u32 frame);

private:
    void ResetDisplays();
    void AdvanceFrame();

public:
    u32 tagOffset;
    u32 firstTagOffset;
    FlashMovieHeader* movie;
    u32 parseFrame;
    u16 currentDepth;
    s32 spriteNesting;
    u32 currentSpriteId;
    u16 spriteFrame;
    u8 fixPointers;
    u32 shapeCount;
    u8 loopFlag;
    u8 pauseFlag;
    u8 unknownFlags[2];
    u32 startFrame;
    u32 endFrame;
    u32 frameCount;
    u32 resourceID;
    FlashDefinition* definitions;
    FlashDisplayObject* rootDisplay;
    FlashSpriteState* sprites;
    u32 playFrame;
    f32 playbackRate;
    FlashTextureBinding textures[30];
    FlashFrameLabel* labels;
};

#endif
