#include <flashmenu.h>
#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSmem.h>
#include <GSprim.h>
#include <winSprite.h>
#include <time.h>

struct FlashTag {
    FlashTagID tagID;
    u32 tagLength;
};

struct FlashTagName {
    FlashTagID tagID;
    const char* name;
};

FlashTagName _fltagnametable[] = {
    {stagEnd, "stagEnd"},
    {stagShowFrame, "stagShowFrame"},
    {stagDefineShape, "stagDefineShape"},
    {stagFreeCharacter, "stagFreeCharacter"},
    {stagPlaceObject, "stagPlaceObject"},
    {stagRemoveObject, "stagRemoveObject"},
    {stagDefineBits, "stagDefineBits"},
    {stagDefineButton, "stagDefineButton"},
    {stagJPEGTables, "stagJPEGTables"},
    {stagSetBackgroundColor, "stagSetBackgroundColor"},
    {stagDefineFont, "stagDefineFont"},
    {stagDefineText, "stagDefineText"},
    {stagDoAction, "stagDoAction"},
    {stagDefineFontInfo, "stagDefineFontInfo"},
    {stagDefineSound, "stagDefineSound"},
    {stagDefineSound, "stagDefineSound"},
    {stagDefineButtonSound, "stagDefineButtonSound"},
    {stagSoundStreamHead, "stagSoundStreamHead"},
    {stagSoundStreamBlock, "stagSoundStreamBlock"},
    {stagDefineBitsLossless, "stagDefineBitsLossless"},
    {stagDefineBitsJPEG2, "stagDefineBitsJPEG2"},
    {stagDefineShape2, "stagDefineShape2"},
    {stagDefineButtonCxform, "stagDefineButtonCxform"},
    {stagProtect, "stagProtect"},
    {stagPlaceObject2, "stagPlaceObject2"},
    {stagRemoveObject2, "stagRemoveObject2"},
    {stagDefineShape3, "stagDefineShape3"},
    {stagDefineText2, "stagDefineText2"},
    {stagDefineButton2, "stagDefineButton2"},
    {stagDefineBitsJPEG3, "stagDefineBitsJPEG3"},
    {stagDefineBitsLossless2, "stagDefineBitsLossless2"},
    {stagDefineEditText, "stagDefineEditText"},
    {stagDefineSprite, "stagDefineSprite"},
    {stagNameCharacter, "stagNameCharacter"},
    {stagFrameLabel, "stagFrameLabel"},
    {stagSoundStreamHead2, "stagSoundStreamHead2"},
    {stagDefineMorphShape, "stagDefineMorphShape"},
    {stagDefineFont2, "stagDefineFont2"},
    {stagExportAssets, "stagExportAssets"},
    {stagImportAssets, "stagImportAssets"},
    {(FlashTagID)-1, NULL},
};

static inline void FreeBuffer(void* buffer)
{
    if (buffer)
        GSmemFree(buffer);
}

inline void FlashMenu::ResetSpriteDisplay(u32 sprite, u32 depth)
{
    FlashSpriteState* spriteState = &sprites[sprite];
    spriteState->displayList[depth].characterId = 0;
    spriteState->displayList[depth].morphRatio = 0.0f;
    spriteState->displayList[depth].previousPlacement = NULL;
    spriteState->displayList[depth].placement = NULL;
    spriteState->displayList[depth].placementFlags = 0;
    flcalcUnitMatrix2(&spriteState->displayList[depth].matrix);
    flcalcUnitCxform(&spriteState->displayList[depth].colorTransform);
}

inline void FlashMenu::ResetDisplays()
{
    s32 i;
    FlashDisplayObject* display = rootDisplay;
    FlashMovieHeader* header = movie;
    s32 j;
    for (i = 0; i < header->depthCapacity; i++) {
        display->previousPlacement = NULL;
        display->placement = NULL;
        display->characterId = 0;
        display->morphRatio = 0.0f;
        flcalcUnitCxform(&display->colorTransform);
        flcalcUnitMatrix2(&display->matrix);
        ++display;
    }

    for (i = 0; i < header->spriteCapacity; i++) {
        for (j = 0; j < header->depthCapacity; j++)
            ResetSpriteDisplay(i, j);
    }
}

inline void FlashMenu::ResetPlacements()
{
    s32 i;
    FlashDisplayObject* display = rootDisplay;
    FlashMovieHeader* header = movie;
    for (i = 0; i < header->depthCapacity; i++) {
        display->morphRatio = 0.0f;
        display->previousPlacement = NULL;
        display->placement = NULL;
        flcalcUnitCxform(&display->colorTransform);
        ++display;
    }
}

inline void FlashMenu::AdvanceFrame()
{
    u32 frame = playFrame;
    if (!pauseFlag) {
        u32 rate = GSgfxVideoGetVsyncRate();
        u32 elapsed = timeGetLastFrameTime();
        if (elapsed >= 10)
            elapsed = 1;
        if (elapsed == 0)
            playbackRate = rate;
        else
            playbackRate = rate / (f32)elapsed;
        frame += elapsed;
        if (frame > endFrame) {
            if (loopFlag) {
                frame = startFrame;
                ResetPlacements();
            } else {
                frame = endFrame;
            }
        }
        playFrame = frame;
    }
}

static inline const char* GetTagName(FlashTagID tagID)
{
    FlashTagName* entry = _fltagnametable;

    while (true) {
        if (entry->tagID == tagID)
            break;
        if (entry->tagID < 0)
            break;
        entry++;
    }
    return entry->name;
}

inline void FlashMenu::ResetPlayback()
{
    FlashMovieHeader* header = movie;
    u16 sprite;
    u16 depth;

    playFrame = 0;
    playbackRate = 0.0f;

    for (depth = 0; depth < header->depthCapacity; depth++)
        for (sprite = 0; sprite < header->spriteCapacity; sprite++)
            rootDisplay[depth].spriteFrames[sprite] = 0;
}

inline void FlashMenu::ParseMovie(FlashTag* tag)
{
    s32 i;
    spriteNesting = 0;
    parseFrame = 0;
    fixPointers = 1;

    for (i = 0; i < FLASH_TEXTURE_BINDING_COUNT; i++) {
        textures[i].characterId = 0;
        textures[i].texture = NULL;
    }

    InitDataPointer();
    ResetPlayback();

    while (true) {
        FlashTagID tagID = tag->tagID;
        u32 tagLength = tag->tagLength;

        GetTagName(tagID);
        if (ParseTag(tagID, tagLength, true, false, true))
            break;
        tag = (FlashTag*)((u8*)movie + tagOffset);
    }
}

void FlashMenu::InitDataPointer()
{
    FlashDefinition* definition = definitions;
    FlashDisplayObject* display = rootDisplay;
    FlashMovieHeader* header = movie;
    s32 i;

    for (i = 0; i < FLASH_DEFINITION_MAX; i++) {
        definition->kind = FLASH_DEFINITION_NONE;
        definition->data = NULL;
        ++definition;
    }

    for (i = 0; i < header->depthCapacity; i++) {
        display->previousPlacement = NULL;
        display->placement = NULL;
        display->characterId = 0;
        display->morphRatio = 0.0f;
        display->placementFlags = 0;
        flcalcUnitCxform(&display->colorTransform);
        flcalcUnitMatrix2(&display->matrix);
        ++display;
    }

    for (s32 sprite = 0; sprite < header->spriteCapacity; sprite++) {
        for (s32 depth = 0; depth < header->depthCapacity; depth++)
            ResetSpriteDisplay(sprite, depth);
    }
}

void FlashMenu::Release()
{
    FlashMovieHeader* header = GetMovieHeader();
    u16 i;
    u16 j;
    FlashFrameLabel* label;
    FlashFrameLabel* node;

    if (definitions != NULL) {
        FreeBuffer(definitions);
        definitions = NULL;
    }

    for (i = 0; i < header->spriteCapacity; i++) {
        if (sprites[i].displayList != NULL) {
            for (j = 0; j < header->depthCapacity; j++) {
                if (sprites[i].displayList[j].spriteFrames != NULL) {
                    delete[] sprites[i].displayList[j].spriteFrames;
                    sprites[i].displayList[j].spriteFrames = NULL;
                }
            }
            delete[] sprites[i].displayList;
            sprites[i].displayList = NULL;
        }
    }
    if (sprites != NULL) {
        FreeBuffer(sprites);
        sprites = NULL;
    }

    for (i = 0; i < header->depthCapacity; i++) {
        if (rootDisplay[i].spriteFrames != NULL) {
            delete[] rootDisplay[i].spriteFrames;
            rootDisplay[i].spriteFrames = NULL;
        }
    }
    if (rootDisplay != NULL) {
        FreeBuffer(rootDisplay);
        rootDisplay = NULL;
    }

    label = labels;
    do {
        node = label;
        if (node != NULL) {
            label = label->next;
            delete node;
        }
    } while (label != NULL);
}

s32 FlashMenu::Init(void* resource, u32 resourceID)
{
    s32 spriteCount;
    FlashMovieHeader* header = (FlashMovieHeader*)resource;
    FlashMovieHeader* data;
    s32 depthCount;
    GSgfxLayer* layer;
    u16 spriteCapacity;
    s32 i;
    s32 j;
    u16 depthCapacity;
    s32 frame;

    layer = GSgfxLayerFindByID(GSGFX_LAYER_MENU);
    this->resourceID = resourceID;
    GSprimSetup2DRender(layer);
    GSprimSetBlendMode(GS_GFX_BLEND_ALPHA);

    definitions = (FlashDefinition*)GSmemAlloc(sizeof(FlashDefinition) * FLASH_DEFINITION_MAX);

    if (header->spriteCapacity == 0) {
        header->spriteCapacity = FLASH_DEFAULT_CAPACITY;
        spriteCapacity = FLASH_DEFAULT_CAPACITY;
    } else {
        spriteCapacity = header->spriteCapacity;
    }

    if (header->depthCapacity == 0) {
        header->depthCapacity = FLASH_DEFAULT_CAPACITY;
        depthCapacity = FLASH_DEFAULT_CAPACITY;
    } else {
        depthCapacity = header->depthCapacity;
    }

    spriteCount = spriteCapacity;
    sprites = (FlashSpriteState*)GSmemAlloc((spriteCount * sizeof(FlashSpriteState) + 31) & ~31);
    for (i = 0; i < spriteCapacity; i++) {
        sprites[i].displayList = NULL;
        sprites[i].displayList = new FlashDisplayObject[depthCapacity];
        for (j = 0; j < depthCapacity; j++) {
            sprites[i].displayList[j].spriteFrames = NULL;
            sprites[i].displayList[j].spriteFrames = new u16[spriteCapacity];
        }
    }

    depthCount = depthCapacity;
    rootDisplay = (FlashDisplayObject*)GSmemAlloc((depthCount * sizeof(FlashDisplayObject) + 31) & ~31);
    for (j = 0; j < depthCapacity; j++) {
        rootDisplay[j].spriteFrames = NULL;
        rootDisplay[j].spriteFrames = new u16[spriteCapacity];
    }

    labels = NULL;
    tagOffset = sizeof(FlashMovieHeader);
    parseFrame = 0;
    fixPointers = 0;

    for (i = 0; i < spriteCount; i++)
        SetSpriteFrameCount(i, 0);

    firstTagOffset = tagOffset;
    movie = header;
    data = movie;
    ParseMovie((FlashTag*)((u8*)data + tagOffset));

    if (fixPointers && data->tagTable == NULL)
        data->tagTable = (FlashTag*)movie;

    frame = parseFrame;
    startFrame = 0;
    endFrame = frame - 1;
    loopFlag = 0;
    pauseFlag = 0;
    frameCount = frame;
    return frame;
}

void FlashMenu::Update()
{
    FlashTag* tag;

    if (!definitions)
        return;

    ResetDisplays();

    GSprimSetup2DRender(NULL);
    GSprimSetBlendMode(GS_GFX_BLEND_ALPHA);

    u32 frame = playFrame;
    if (movie->frameCount > frame) {
        tagOffset = firstTagOffset;
        parseFrame = frame;
        tag = (FlashTag*)((u8*)movie + tagOffset);
        spriteNesting = 0;
        while (true) {
            if (ParseTag(tag->tagID, tag->tagLength, false, false, false))
                break;
            tag = (FlashTag*)((u8*)movie + tagOffset);
        }
    }
    shapeCount = 0;
}
void FlashMenu::Draw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    if (definitions) {
        GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
        GSgfxLayerSetActive(layer);
        GSgfxLayerFindByID(layer);
        DrawSub(layer, window, sprite);
        AdvanceFrame();
        GSgfxLayerSetActive(GSGFX_LAYER_MENU);
    }
}

FlashMenu::FlashMenu()
{
    currentDepth = 0;
    fixPointers = 0;
    shapeCount = 0;
}

FlashMenu::~FlashMenu()
{
}

GStexture* FlashMenu::GetGStexture(s32 textureID)
{
    for (s32 i = 0; i < FLASH_TEXTURE_BINDING_COUNT; i++) {
        if (textures[i].characterId == textureID)
            return textures[i].texture;
    }
    return NULL;
}

