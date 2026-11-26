#ifndef FLASH_MENU_H
#define FLASH_MENU_H

#include <types.h>
#include <flashcalc.h>
#include <GSgfxTypes.h>

struct GStexture;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;
struct _PLACEOBJECT2;
struct FlashTag;

struct FlashDisplayObject;

enum FlashTagID {
    stagEnd = 0,
    stagShowFrame = 1,
    stagDefineShape = 2,
    stagFreeCharacter = 3,
    stagPlaceObject = 4,
    stagRemoveObject = 5,
    stagDefineBits = 6,
    stagDefineButton = 7,
    stagJPEGTables = 8,
    stagSetBackgroundColor = 9,
    stagDefineFont = 10,
    stagDefineText = 11,
    stagDoAction = 12,
    stagDefineFontInfo = 13,
    stagDefineSound = 14,
    stagDefineButtonSound = 17,
    stagSoundStreamHead = 18,
    stagSoundStreamBlock = 19,
    stagDefineBitsLossless = 20,
    stagDefineBitsJPEG2 = 21,
    stagDefineShape2 = 22,
    stagDefineButtonCxform = 23,
    stagProtect = 24,
    stagPlaceObject2 = 26,
    stagRemoveObject2 = 28,
    stagDefineShape3 = 32,
    stagDefineText2 = 33,
    stagDefineButton2 = 34,
    stagDefineBitsJPEG3 = 35,
    stagDefineBitsLossless2 = 36,
    stagDefineEditText = 37,
    stagDefineSprite = 39,
    stagNameCharacter = 40,
    stagFrameLabel = 43,
    stagSoundStreamHead2 = 45,
    stagDefineMorphShape = 46,
    stagDefineFont2 = 48,
    stagExportAssets = 56,
    stagImportAssets = 57,
};

enum FlashDefinitionKind {
    FLASH_DEFINITION_NONE = 0,
    FLASH_DEFINITION_MORPH_SHAPE = 1,
    FLASH_DEFINITION_SHAPE = 2,
    FLASH_DEFINITION_SPRITE = 4,
    FLASH_DEFINITION_FONT = 8,
    FLASH_DEFINITION_EDIT_TEXT = 16,
    FLASH_DEFINITION_TEXT = 32,
};

enum { FLASH_TEXTURE_BINDING_COUNT = 30 };
enum { FLASH_DEFINITION_MAX = 200 };
enum { FLASH_DEFAULT_CAPACITY = 40 };

struct _FILLSTYLE;
struct _LINESTYLE;

struct _FILLSTYLEARRAY {
    u32 count;
    _FILLSTYLE* styles;
};

struct _LINESTYLEARRAY {
    u32 count;
    _LINESTYLE* styles;
};

struct _SHAPERECORD {
    u8 code;
    void* data;
    _SHAPERECORD* next;
};

struct FlashSpriteState {
    u16 frameCount;
    u16 unknown;
    FlashDisplayObject* displayList;
};

struct FlashMovieHeader {
    u8 reserved0[6];
    u16 frameCount;
    u8 reserved1[12];
    FlashTag* tagTable;
    u16 spriteCapacity;
    u16 depthCapacity;
    u8 reserved2[20];
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
    _PLACEOBJECT2* previousPlacement;
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
    u8 ParseTag(u32 tagID, u32 tagLength, bool ignorePlayFrame, bool unused, bool registerDefinitions);
    void InitDataPointer();
    GStexture* GetGStexture(s32 textureID);
    u16 GetFrameFromLabel(char* label);
    u32 GetPlayFrame();
    void SetPlayFrame(u32 frame);

private:
    void ResetDisplays();
    void ResetPlacements();
    void ResetPlayback();
    void ResetSpriteDisplay(u32 sprite, u32 depth);
    void ParseMovie(FlashTag* tag);
    void AdvanceFrame();
    FlashMovieHeader* GetMovieHeader() { return movie; }
    void SetSpriteFrameCount(u32 sprite, u16 count) { sprites[sprite].frameCount = count; }

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
    FlashTextureBinding textures[FLASH_TEXTURE_BINDING_COUNT];
    FlashFrameLabel* labels;
};

#endif
