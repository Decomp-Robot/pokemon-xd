#include <CAnt.h>

#include <ant.h>
#include <GSfsys.h>
#include <GSgfx.h>
#include <GSgfxC.h>
#include <GSgfxTypes.h>
#include <GSgfxVideo.h>
#include <GSprim.h>
#include <GSres.h>
#include <GStexture.h>
#include <GSthread.h>
#include <Pokemon.h>
#include <menu.h>
#include <menuPocketTool.h>
#include <pokemonDB.h>
#include <time.h>
#include <types.h>

#include <string.h>

GSgfxColAu8 colorWhite = { 255, 255, 255, 255 };
GSgfxColAu8 colorPurple = { 60, 12, 255, 255 };
GSgfxColAu8 colorNone = { 0, 0, 0, 0 };

static const s32 ANT_RESOURCE_GROUP = 2085;
static const int ANT_RESOURCE_MAX = 240;
static const u32 ANT_RESOURCE_DARK_RUGIA = 0x20083200;

static const u8 ANT_CMD_FRAME_SET = 0x01;
static const u8 ANT_CMD_WAIT = 0x02;
static const u8 ANT_CMD_OFFSET = 0x04;
static const u8 ANT_CMD_RELATIVE_JUMP = 0x08;

static const s32 ANT_HIDE_FLAG = 1;

// Header of a loaded animation resource. The offsets are relative to the
// start of the resource.
struct AntData {
    u32 unknown0;
    u32 frameSetOffset;
    u32 unknown8;
    u32 textureOffset;
    u16 commandOffset[1];
};

// Run of sprite pieces that make up one displayed picture; frameOffset is
// relative to the frame set table.
struct AntFrameSet {
    u16 frameCount;
    u16 frameOffset;
};

// One sprite piece: a rectangle of the texture and where to put it.
struct AntFrame {
    u16 srcX;
    u16 srcY;
    u16 width;
    u16 height;
    s16 offsetX;
    s16 offsetY;
    u8 unknownC[16];
};

struct AntCache {
    CAntCacheEntry entry[ANT_RESOURCE_MAX];
    s32 pendingReadIndex;
};

static AntCache _pokeDance;

static inline s32 antFindEntry(u32 resourceID)
{
    for (s32 i = 0; i < ANT_RESOURCE_MAX; i++) {
        if (_pokeDance.entry[i].resourceID == 0)
            break;
        u32 id = _pokeDance.entry[i].resourceID;
        if (id == resourceID)
            return i;
    }
    return -1;
}

// Unown forms, in the index order that pokemonGetAnnonKatati returns.
enum AntUnownForm {
    ANT_UNOWN_A,
    ANT_UNOWN_B,
    ANT_UNOWN_C,
    ANT_UNOWN_D,
    ANT_UNOWN_E,
    ANT_UNOWN_F,
    ANT_UNOWN_G,
    ANT_UNOWN_H,
    ANT_UNOWN_I,
    ANT_UNOWN_J,
    ANT_UNOWN_K,
    ANT_UNOWN_L,
    ANT_UNOWN_M,
    ANT_UNOWN_N,
    ANT_UNOWN_O,
    ANT_UNOWN_P,
    ANT_UNOWN_Q,
    ANT_UNOWN_R,
    ANT_UNOWN_S,
    ANT_UNOWN_T,
    ANT_UNOWN_U,
    ANT_UNOWN_V,
    ANT_UNOWN_W,
    ANT_UNOWN_X,
    ANT_UNOWN_Y,
    ANT_UNOWN_Z,
    ANT_UNOWN_EXCLAMATION,
    ANT_UNOWN_QUESTION,
    ANT_UNOWN_FORM_COUNT
};

inline u32 antUnownSprite(Pokemon* pokemon)
{
    // Resource IDs of the Unown sprites, one row per form. Column 0 is the
    // normal sprite and column 1 the rare one chosen by pokemonCheckRare2.
    const u32 unownSpriteIds[ANT_UNOWN_FORM_COUNT][2] = {
        { 0x1E4C3200, 0x1E4D3200 }, // ANT_UNOWN_A
        { 0x1E4E3200, 0x1E4F3200 }, // ANT_UNOWN_B
        { 0x1E503200, 0x1E513200 }, // ANT_UNOWN_C
        { 0x1E523200, 0x1E533200 }, // ANT_UNOWN_D
        { 0x1E543200, 0x1E573200 }, // ANT_UNOWN_E
        { 0x1E583200, 0x1E593200 }, // ANT_UNOWN_F
        { 0x1E5A3200, 0x1E5B3200 }, // ANT_UNOWN_G
        { 0x1E5C3200, 0x1E5D3200 }, // ANT_UNOWN_H
        { 0x1E5E3200, 0x1E5F3200 }, // ANT_UNOWN_I
        { 0x1E603200, 0x1E613200 }, // ANT_UNOWN_J
        { 0x1E623200, 0x1E633200 }, // ANT_UNOWN_K
        { 0x1E643200, 0x1E653200 }, // ANT_UNOWN_L
        { 0x1E663200, 0x1E673200 }, // ANT_UNOWN_M
        { 0x1E683200, 0x1E693200 }, // ANT_UNOWN_N
        { 0x1E6A3200, 0x1E6B3200 }, // ANT_UNOWN_O
        { 0x1E6C3200, 0x1E6D3200 }, // ANT_UNOWN_P
        { 0x1E6E3200, 0x1E713200 }, // ANT_UNOWN_Q
        { 0x1E723200, 0x1E733200 }, // ANT_UNOWN_R
        { 0x1E743200, 0x1E753200 }, // ANT_UNOWN_S
        { 0x1E763200, 0x1E773200 }, // ANT_UNOWN_T
        { 0x1E783200, 0x1E793200 }, // ANT_UNOWN_U
        { 0x1E7A3200, 0x1E7B3200 }, // ANT_UNOWN_V
        { 0x1E7C3200, 0x1E7D3200 }, // ANT_UNOWN_W
        { 0x1E7E3200, 0x1E7F3200 }, // ANT_UNOWN_X
        { 0x1E803200, 0x1E813200 }, // ANT_UNOWN_Y
        { 0x1E823200, 0x1E833200 }, // ANT_UNOWN_Z
        { 0x1E553200, 0x1E563200 }, // ANT_UNOWN_EXCLAMATION
        { 0x1E6F3200, 0x1E703200 }, // ANT_UNOWN_QUESTION
    };
    u8 form = pokemonGetAnnonKatati(
        pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_PID, 0));
    if (pokemonCheckRare2(pokemon))
        return unownSpriteIds[form][1];
    else
        return unownSpriteIds[form][0];
}

static f32 antInverseFade(f32 fade)
{
    return 1.0f - fade;
}

f32 CAnt::colorAnimationTimeDark;
f32 CAnt::colorAnimationTimeReliveReady;

namespace {
void dispTexture(GStexture* texture, f32 x, f32 y, int srcX, int srcY,
                 int width, int height, GSgfxColAu8 color, f32 scale,
                 unsigned char alpha);
}

static inline f32 antAbs(f32 value)
{
    return value < 0.0f ? -value : value;
}

inline AntData* CAnt::resolveCacheEntry()
{
    u32 resourceID = mResource;
    if (resourceID == 0)
        return 0;

    CAntCacheEntry* entries = _pokeDance.entry;
    s32 index = antFindEntry(resourceID);
    if (index == -1) {
        for (index = 0; index < ANT_RESOURCE_MAX; index++) {
            if (entries[index].resourceID == 0)
                break;
        }
        CAntCacheEntry* entry = &_pokeDance.entry[index];
        memset(entry, 0, sizeof(CAntCacheEntry));
        entry->resourceID = resourceID;
    }
    CAntCacheEntry* entry = &_pokeDance.entry[index];
    entry->useCount = ANT_USE_ACTIVE;
    return entry->animData;
}

inline AntData* CAnt::getAnimData()
{
    if (mResource == 0)
        return 0;
    if (mCachedAnimData != 0)
        return mCachedAnimData;
    mCachedAnimData = resolveCacheEntry();
    return mCachedAnimData;
}

inline void CAnt::readCommand(AntData* animData)
{
    if (animData == 0)
        return;
    u8* block = (u8*)animData + animData->commandOffset[mAnimIndex];
    u8 command = block[mCommandCursor++];
    if (command & ANT_CMD_WAIT) {
        mWaitSeconds = (f32)(block[mCommandCursor++] * 18 / 10) / 60.0f;
    }
    if (command & ANT_CMD_OFFSET) {
        mOffsetX = block[mCommandCursor++];
        mOffsetY = block[mCommandCursor++];
    }
    if (command & ANT_CMD_FRAME_SET) {
        mFrameSet = block[mCommandCursor++];
    }
    if (command & ANT_CMD_RELATIVE_JUMP) {
        mCommandCursor -= block[mCommandCursor];
    }
}

inline void CAnt::nextCommand()
{
    if (mResource == 0)
        return;
    mWaitSeconds = 0.0f;
    mTimerSeconds = 0.0f;
    readCommand(getAnimData());
}

inline void CAnt::update()
{
    if (mWaitSeconds) {
        mTimerSeconds += timeGetLastFrameSec();
        if (mTimerSeconds < mWaitSeconds - 0.5f * (1.0f / (f32)GSgfxVideoGetVsyncRate()))
            return;
    }
    nextCommand();
}

void CAnt::drawAndUpdateWithOffset(PocketVec<short> offset)
{
    if (mResource == 0)
        return;
    draw(offset);
    update();
}

void CAnt::drawAndUpdate(PocketVec<short> offset)
{
    if (mResource == 0)
        return;
    setPosition(offset);
    draw(PocketVec<short>::zero);
    update();
}

void CAnt::drawAndUpdate()
{
    if (mResource == 0)
        return;
    draw(PocketVec<short>::zero);
    update();
}

void CAnt::draw(PocketVec<short> offset)
{
    if (mResource == 0 || mHideFlag == ANT_HIDE_FLAG)
        return;

    PocketVec<short> position = mPosition;
    position.x += offset.x;
    position.y += offset.y;

    if (getAnimData() == 0) {
        windowDrawSprite((s16)(position.x - 24), (s16)(position.y - 24), 0,
                         menuRelivehallPcBoxDefaultPokemonSprite, 0,
                         GSGFX_LAYER_MENU);
        return;
    }

    GSgfxColAu8 color;
    switch (mColorMode) {
    case ANT_COLOR_DARK: {
        color = colorPurple;
        f32 time = colorAnimationTimeDark;
        f32 alpha = 1.0f - 2.0f * antAbs(time - 0.5f);
        alpha = 0.76f * alpha + 0.16f;
        color.a = (u8)(255.0f * alpha);
        break;
    }
    case ANT_COLOR_NONE:
        color = colorNone;
        break;
    case ANT_COLOR_RELIVE_READY: {
        f32 time = colorAnimationTimeReliveReady;
        f32 fade = 1.0f - 2.0f * antAbs(time - 0.5f);
        f32 inv = antInverseFade(fade);
        if (fade > 0.5)
            color.a = (u8)(255.0f * fade);
        else
            color.a = 128;
        color.r = (u8)(colorWhite.r * fade + colorPurple.r * inv);
        color.g = (u8)(colorWhite.g * fade + colorPurple.g * inv);
        color.b = (u8)(colorWhite.b * fade + colorPurple.b * inv);
        break;
    }
    }

    AntData* animData = getAnimData();
    GStexture* texture = animData != 0 ? (GStexture*)((u8*)animData + animData->textureOffset) : 0;
    GStextureGetXsize(texture);
    GStextureGetYsize(texture);

    animData = getAnimData();

    u8* frameSetTable = (u8*)animData + animData->frameSetOffset;
    AntFrameSet* frameSet = &((AntFrameSet*)frameSetTable)[mFrameSet];
    AntFrame* frames = (AntFrame*)(frameSetTable + frameSet->frameOffset);
    for (int i = 0; i < frameSet->frameCount; i++) {
        AntFrame* frame = &frames[i];
        dispTexture(texture,
                    (f32)(position.x + mOffsetX + frame->offsetX),
                    (f32)(position.y + mOffsetY + frame->offsetY + 32),
                    frame->srcX, frame->srcY, frame->width, frame->height,
                    color, mScale, mAlpha);
    }
}

void CAnt::setAnimation(s32 animation)
{
    mAnimIndex = animation;
    mCommandCursor = 0;
    mTimerSeconds = 0.0f;
    mOffsetX = 0;
    mOffsetY = 0;
    mFrameSet = 0;
    mWaitSeconds = 0.0f;
    nextCommand();
}

void CAnt::use()
{
    if (mResource == 0)
        return;
    resolveCacheEntry();
}

inline void CAnt::initByPokemonAndSetColorAnimation(Pokemon* pokemon)
{
    memset(this, 0, sizeof(CAnt));
    mScale = 1.0f;
    mAlpha = 255;

    if (pokemon->checkValid()) {
        u16 species = pokemon->getPokemonDataId();
        if (species == POKEMON_SPECIES_ANNON) {
            mResource = antUnownSprite(pokemon);
        } else if (species == POKEMON_SPECIES_RUGIA && pokemon->isDarkPokemon()) {
            mResource = ANT_RESOURCE_DARK_RUGIA;
        } else {
            bool rare = pokemon->isRare();
            mResource = pokemonDB_GetPokebodyId(species, rare);
        }

        if (mResource != 0 && mCachedAnimData == 0) {
            mCachedAnimData = resolveCacheEntry();
        }
    }

    if (pokemon->isDarkPokemon()) {
        if (pokemon->getDarkPoint() > 0.0f)
            mColorMode = ANT_COLOR_DARK;
        else
            mColorMode = ANT_COLOR_RELIVE_READY;
    }
}

void CAnt::initByPokemonAndSetColorAnimationInReliveHallVisitor(Pokemon* pokemon)
{
    initByPokemonAndSetColorAnimation(pokemon);
}

void CAnt::initByPokemonAndSetColorAnimationInAny(Pokemon* pokemon)
{
    initByPokemonAndSetColorAnimation(pokemon);
}

void CAnt::initByPokemon(Pokemon* pokemon)
{
    memset(this, 0, sizeof(CAnt));
    mScale = 1.0f;
    mAlpha = 255;

    if (pokemon->checkValid()) {
        u16 species = pokemon->getPokemonDataId();
        if (species == POKEMON_SPECIES_ANNON) {
            mResource = antUnownSprite(pokemon);
        } else if (species == POKEMON_SPECIES_RUGIA && pokemon->isDarkPokemon()) {
            mResource = ANT_RESOURCE_DARK_RUGIA;
        } else {
            bool rare = pokemon->isRare();
            mResource = pokemonDB_GetPokebodyId(species, rare);
        }

        if (mResource != 0 && mCachedAnimData == 0) {
            mCachedAnimData = resolveCacheEntry();
        }
    }
}

void CAnt::init()
{
    memset(this, 0, sizeof(CAnt));
    mScale = 1.0f;
    mAlpha = 255;
}

static inline void antReleaseResources(bool cancelRequests)
{
    CAntCacheEntry* entries = _pokeDance.entry;

    if (cancelRequests) {
        CAntCacheEntry* entry = entries;
        for (int i = 0; i < ANT_RESOURCE_MAX; entry++, i++) {
            if (i == _pokeDance.pendingReadIndex)
                continue;
            if (entry->resourceID == 0)
                continue;
            if (entry->animData != 0)
                continue;
            entry->useCount = ANT_USE_FREE;
        }
    }

    CAntCacheEntry* pending = &_pokeDance.entry[_pokeDance.pendingReadIndex];
    pending->useCount = ANT_USE_PENDING_READ;

    u32 count = 0;
    u32 i;
    for (i = 0; i < ANT_RESOURCE_MAX; i++) {
        if (entries[i].resourceID == 0)
            break;
        if (entries[i].useCount != ANT_USE_FREE) {
            _pokeDance.entry[count++] = entries[i];
        } else if (entries[i].animData != 0) {
            GSresFreeResource(ANT_RESOURCE_GROUP, entries[i].resourceID);
        }
    }
    for (; count < ANT_RESOURCE_MAX; count++)
        memset(&_pokeDance.entry[count], 0, sizeof(CAntCacheEntry));

    CAntCacheEntry* entry = entries;
    for (u32 j = 0; j < ANT_RESOURCE_MAX; entry++, j++) {
        if (entry->resourceID == 0)
            return;
        if (entry->useCount == ANT_USE_PENDING_READ) {
            entry->useCount = ANT_USE_ACTIVE;
            _pokeDance.pendingReadIndex = j;
            return;
        }
    }
}

void CAnt::systemUnUseResourceRelease()
{
    antReleaseResources(false);
}

void CAnt::systemUsecountReset()
{
    for (int i = 0; i < ANT_RESOURCE_MAX; i++)
        _pokeDance.entry[i].useCount = ANT_USE_FREE;
}

void CAnt::systemUpdate(bool update)
{
    if (_pokeDance.pendingReadIndex != -1) {
        CAntCacheEntry* entry = &_pokeDance.entry[_pokeDance.pendingReadIndex];
        if (GSfsysGetStatus(ANT_RESOURCE_GROUP) == GSFSYS_STATUS_DONE) {
            entry->animData = (AntData*)GSresGetResource(ANT_RESOURCE_GROUP, entry->resourceID);
            if (entry->animData != 0)
                _pokeDance.pendingReadIndex = -1;
        }
    }

    if (_pokeDance.pendingReadIndex == -1 && update) {
        CAntCacheEntry* entry = _pokeDance.entry;
        for (int i = 0; i < ANT_RESOURCE_MAX; entry++, i++) {
            if (entry->resourceID == 0)
                break;
            if (entry->resourceID != 0 && entry->animData == 0) {
                GSfsysReadID(ANT_RESOURCE_GROUP, entry->resourceID);
                _pokeDance.pendingReadIndex = i;
                break;
            }
        }
    }

    colorAnimationTimeDark += timeGetLastFrameSec();
    colorAnimationTimeReliveReady += timeGetLastFrameSec();
    if (colorAnimationTimeDark > 1.0f)
        colorAnimationTimeDark -= 1.0f;
    if (colorAnimationTimeReliveReady > 1.0f)
        colorAnimationTimeReliveReady -= 1.0f;
}

void CAnt::waitEndOfReading()
{
    while (true) {
        if (_pokeDance.pendingReadIndex != -1) {
            CAntCacheEntry* entry = &_pokeDance.entry[_pokeDance.pendingReadIndex];
            if (GSfsysGetStatus(ANT_RESOURCE_GROUP) == GSFSYS_STATUS_DONE) {
                entry->animData = (AntData*)GSresGetResource(ANT_RESOURCE_GROUP, entry->resourceID);
                if (entry->animData != 0)
                    _pokeDance.pendingReadIndex = -1;
            }
        }
        if (_pokeDance.pendingReadIndex == -1)
            break;
        GSthreadSwitch();
    }
}

void CAnt::allRequestCancel()
{
    antReleaseResources(true);
}

void CAnt::systemFree()
{
    waitEndOfReading();
    GSfsysFree(ANT_RESOURCE_GROUP);
    GSresFreeGroup(ANT_RESOURCE_GROUP);
    memset(&_pokeDance, 0, sizeof(_pokeDance));
}

void CAnt::systemInit()
{
    memset(&_pokeDance, 0, sizeof(_pokeDance));
    _pokeDance.pendingReadIndex = -1;
}

GStexture* antGetGStexture(void* resource)
{
    AntData* animData = (AntData*)resource;
    return (GStexture*)((u8*)animData + animData->textureOffset);
}

namespace {

void dispTexture(GStexture* texture, f32 x, f32 y, int srcX, int srcY,
                 int width, int height, GSgfxColAu8 color, f32 scale,
                 unsigned char alpha)
{
    u16 textureWidth = GStextureGetXsize(texture);
    u16 textureHeight = GStextureGetYsize(texture);
    f32 u0 = (f32)srcX / (f32)textureWidth;
    f32 v0 = (f32)srcY / (f32)textureHeight;
    f32 u1 = u0 + (f32)width / (f32)textureWidth;
    f32 v1 = v0 + (f32)height / (f32)textureHeight;

    if (scale != 1.0f) {
        x += width / 2;
        y += height / 2;
        width = (int)((f32)width * scale);
        height = (int)((f32)height * scale);
        x -= width / 2;
        y -= height / 2;
    }

    GSgfxVF* vf = GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, 0, 1, GX_RGBA8, 1, GX_TEX_ST, GX_F32);
    GSgfxSetupRenderAndSelect(vf);

    if (alpha != 255) {
        color.a = alpha;
        GSgfxSetTUnitUserColorArg(0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GSgfxSetTUnitUserColorOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GSgfxSetTUnitUserAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
        GSgfxSetTUnitUserAlphaOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    } else {
        GSgfxSetTUnitUserColorArg(0, GX_CC_TEXC, GX_CC_RASC, GX_CC_RASA, GX_CC_ZERO);
        GSgfxSetTUnitUserColorOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GSgfxSetTUnitUserAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_TEXA);
        GSgfxSetTUnitUserAlphaOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    }
    GSprimSetBlendMode(GS_GFX_BLEND_ALPHA);

    u8 colR = color.r;
    u8 colG = color.g;
    u8 colB = color.b;
    u8 colA = color.a;
    s16 xi = (s16)x;
    s16 yi = (s16)y;
    GSgfxSelectTexture(0, texture);
    GSgfxSetPrimitiveType(GS_GFX_PRIM_TRIANGLE_STRIP);
    GSgfxBeginSurface(4);

    GSgfxVertex2s16(xi, yi);
    GSgfxColor4u8(colR, colG, colB, colA);
    GSgfxTexCoord2f(u0, v0);

    GSgfxVertex2s16(xi + width, yi);
    GSgfxColor4u8(colR, colG, colB, colA);
    GSgfxTexCoord2f(u1, v0);

    GSgfxVertex2s16(xi, yi + height);
    GSgfxColor4u8(colR, colG, colB, colA);
    GSgfxTexCoord2f(u0, v1);

    GSgfxVertex2s16(xi + width, yi + height);
    GSgfxColor4u8(colR, colG, colB, colA);
    GSgfxTexCoord2f(u1, v1);

    GSgfxEndSurface();
}

} // namespace

void antReadPostFunc(void* resource, u32 size)
{
}

