#include <menuPocketTool.h>
#include <menuInfoWindow.h>
#include <GSmodel.h>
#include <GSbound.h>
#include <gamedata.h>
#include <memo.h>
#include <wazaSequenceSys.h>
#include <GSmsgEx.h>
#include <msgctrl.h>
#include <winMsg.h>
#include <darkPokemon.h>
#include <GSprim.h>
#include <GSthread.h>
#include <GStexture.h>
#include <GSsnd.h>
#include <GSgfx.h>
#include <GSres.h>
#include <sound.h>
#include <sprite.h>
#include <floorLayerConfig.h>
#include <fade.h>
#include <menu.h>
#include <menuDataBios.h>
#include <menuSub.h>
#include <window.h>
#include <time.h>
#include <libc/memory.h>
#include <math.h>

const GSgfxColAu8 GSgfxColAu8_Black = {0, 0, 0, 255};
CMetaMenu* CMetaMenu::pCurrentMenu;
static CMetaMenu* _pThis;
static int _selectItemQuantity;
PocketVec<short> CMenuPokemonCursor::defaultPositionTbl;

enum {
    POCKET_SOUND_FADE_TIME = 50,
    POCKET_SOUND_SILENCE_BGM = 8,
    POCKET_SOUND_VOLUME_MAX = 255,
    DEBUG_POKEMON_LEVEL = 25,
    DEBUG_POKEMON_SPECIES_COUNT = 386,
    DEBUG_DARK_POKEMON_INITIAL_INDEX = 12,
    DEBUG_DARK_POKEMON_RETRY_COUNT = 10
};

template <>
PocketVec<short> PocketVec<short>::zero = {0, 0};
u16 menuRelivehallPcBoxDefaultPokemonSprite = 0x02df;
long CMetaMenu::defautlLocalWindowOpenTable[1] = {0};
static unsigned long _darkPokemonIndex = DEBUG_DARK_POKEMON_INITIAL_INDEX;

enum {
	CELEBI_SPECIES_NUMBER = 0xfb,
	RESERVED_SPECIES_SLOTS = 0x19,
};

static const float CURSOR_PI = 3.14159265358979323846f;
static const float CURSOR_FLASH_CYCLE_SECONDS = 0.5f;
static const float CURSOR_ANIMATION_HALF = 0.5f;
static const float CURSOR_FLASH_PHASE_MIDPOINT = 0.5f;
static const unsigned long CURSOR_HIGHLIGHT_COLOR = 0xffc800ff;
#define CURSOR_COLOR_RGB_MASK 0xffffff00

enum {
    CURSOR_MARK_SPRITE = 0x214,
    CURSOR_HELD_SPRITE = 0x215,
    CURSOR_CORNER_SPRITE = 0x216
};

static const f32 zero = 0.0f;
static const f32 twoPi = 6.28318530717958647692f;
static const f32 invTwoPi = 0.15915494309189533577f;
static const f32 pi = 3.14159265358979323846f;
static const f32 half = 0.5f;
static const f32 degHalfTurn = 180.0f;
static const f32 negativePi = -3.14159265358979323846f;

void CMetaMenu::tutorialExitKick() {}

int CMetaMenu::isAnimationStateNone() { return 1; }

bool getBitTable(unsigned long *table, long index)
{
    return table[index / 32] & (1 << (index % 32));
}

void setBitTable(unsigned long *table, long index, bool flag)
{
    long pos = index / 32;
    long bit = index % 32;

    // A false flag leaves the table untouched.
    if (flag == 0) {
        return;
    }
    table[pos] |= (1 << bit);
}

void menuPocketToolFloatToZero(float& value, float step) {
    if (value > 0.0f) {
        if ((value -= step) < 0.0f) {
            value = 0.0f;
        }
    } else if (value < 0.0f) {
        if ((value += step) > 0.0f) {
            value = 0.0f;
        }
    }
}

float dirRefreshPI(float dir)
{
    float result = dir - (float)(long)(invTwoPi * dir) * twoPi;

    if (result > pi) {
        result -= twoPi;
    } else if (result < negativePi) {
        result += twoPi;
    }

    return result;
}

float dirRefresh2PI(float dir)
{
    float result = dir - (float)(long)(invTwoPi * dir) * twoPi;

    if (result < zero) {
        result += twoPi;
    }

    return result;
}

long degreeRefresh360(long degree)
{
    long result = degree % 360;

    if (result < 0) {
        result += 360;
    }

    return result;
}

long degreeRefresh180(long degree)
{
    long result = degree % 360;

    if (result >= 180) {
        result -= 360;
    } else if (result <= -180) {
        result += 360;
    }

    return result;
}

long degreeSubHalf(long degree1, long degree2)
{
    return degreeRefresh180(degree1 - degree2);
}

float dirSubHalf(float dir1, float dir2)
{
    return dirRefreshPI(dir1 - dir2);
}

float dirSubOne(float dir1, float dir2)
{
    return dirRefresh2PI(dir1 - dir2);
}

long xyxy2dirDegree(const PocketVec<short> &point, const PocketVec<short> &center)
{
    float dx = point.x - center.x;
    float dy = -(point.y - center.y);
    float dir = atan2(dx, dy);

    return degreeRefresh180((long)(degHalfTurn * dir / pi + half));
}

float xyxy2dir(const PocketVec<short> &point, const PocketVec<short> &center)
{
    float dx = point.x - center.x;
    float dy = -(point.y - center.y);

    return atan2(dx, dy);
}

float xy2dir(float x, float y)
{
    return atan2(x, -y);
}

bool isDirRegionIn(float dir, float from, float to)
{
    float range = (to == from) ? twoPi : dirSubOne(to, from);

    return dirSubOne(dir, from) <= range;
}

float xyxy2distance(const PocketVec<short>& point,
                    const PocketVec<short>& center) {
    long dx = point.x - center.x;
    long dy = point.y - center.y;
    return sqrtf(dx * dx + dy * dy);
}

unsigned long GSgfxColAu8To32(GSgfxColAu8 color) {
    unsigned long packedColor;
    memcpy(&packedColor, &color, sizeof(packedColor));
    return packedColor;
}

tagMENU_ITEM_DATA* menuPtItemGetByMenuIdAndItemId(long menuID,
                                               unsigned long itemID,
                                               long offset) {
    tagMENU_ITEM_DATA* item = menuItemBiosGetPtr(menuID, itemID);
    for (long index = 0; index < offset; ++index) {
        item = menuItemBiosGetPtr(menuID, item->mNextID);
    }
    return item;
}

void menuPtSpriteWorkSetMsgId(long menuID, unsigned long itemID, long offset,
                             unsigned long msgID) {
    menuPtSpriteWorkGetByItemID(menuID, itemID, offset)->mMsgID = msgID;
}

tagSPRITE_WORK* menuPtSpriteWorkGetByItemID(long menuID, unsigned long itemID,
                                         long offset) {
    return menuPtSpriteWorkGetByItemID(windowSearchID(menuID), itemID, offset);
}

tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(tagWINDOW_WORK *window, unsigned long itemID, long offset)
{
    tagSPRITE_WORK *work = window->mSpriteWork;

    while (work != 0) {
        if (work->mItemID == itemID) {
            break;
        }
        work = work->mNext;
    }

    while (offset > 0) {
        work = work->mNext;
        offset--;
    }

    return work;
}

unsigned long menuPtSpriteWorkGetIndexFromId(tagWINDOW_WORK *window, tagSPRITE_WORK *work, unsigned long itemID)
{
    tagSPRITE_WORK *sprite = menuPtSpriteWorkGetByItemID(window, itemID, 0);
    unsigned long index = 0;

    while (1) {
        if (sprite == work) {
            break;
        }
        sprite = sprite->mNext;
        index++;
    }

    return index;
}

tagMENU_ITEM_DATA *menuPtItemGetBySpriteWork(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    return menuItemBiosGetPtr(window->mMenuID, work->mItemID);
}

tagSPRITE_WORK *menuPtGetSpriteWorkByHookFunction(tagWINDOW_WORK *window, void *hook, unsigned long index, long offset)
{
    tagSPRITE_WORK *work = window->mSpriteWork;

    while (work != 0) {
        if (work->mHookFunction == hook && index-- == 0) {
            break;
        }
        work = work->mNext;
    }

    while (offset != 0 && work != 0) {
        offset--;
        work = work->mNext;
    }

    return work;
}

void menuPtSpriteWorkSetMenuItem(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, unsigned long itemID) {
    MENU_ITEM_dd_* item = menuItemBiosGetPtr(window->mMenuID, itemID);
    MenuSpriteImage* entry = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(item->mSpriteID));
    sprite->mTextureResourceID = entry->mResourceID;
    sprite->mTextureX = entry->mX;
    sprite->mTextureY = entry->mY;
    sprite->mTextureWidth = entry->mWidth;
    sprite->mTextureHeight = entry->mHeight;
    sprite->mX = item->mX;
    sprite->mY = item->mY;
    sprite->mWidth = item->mWidth;
    sprite->mHeight = item->mHeight;
}

void menuPtSpriteWorkSetSpriteData(tagSPRITE_WORK* sprite, MenuSpriteData* spriteData) {
    MenuSpriteImage* entry = menuSpriteBiosGetValPtr(spriteData);
    sprite->mTextureResourceID = entry->mResourceID;
    sprite->mTextureX = entry->mX;
    sprite->mTextureY = entry->mY;
    sprite->mTextureWidth = entry->mWidth;
    sprite->mTextureHeight = entry->mHeight;
    sprite->mWidth = entry->mWidth;
    sprite->mHeight = entry->mHeight;
}

void menuPtSpriteWorkSetSpriteId(tagSPRITE_WORK* sprite, unsigned long spriteID) {
    MenuSpriteData* data = menuSpriteBiosGetPtr(spriteID);
    menuPtSpriteWorkSetSpriteData(sprite, data);
}

void menuPtSpriteWorkSetSw(tagWINDOW_WORK* window, unsigned long itemID, long offset, bool display) {
    winSpriteSetDisp(menuPtSpriteWorkGetByItemID(window, itemID, offset), display);
}

void menuPtSpriteWorkSetSw(tagWINDOW_WORK*, tagSPRITE_WORK* sprite, bool display) {
    winSpriteSetDisp(sprite, display);
}

void menuPtSpriteWorkSetWidthRateColor(tagWINDOW_WORK* window, unsigned long itemID,
                                      long offset, float widthRate, GSgfxColAu8 color) {
    tagSPRITE_WORK* sprite = menuPtSpriteWorkGetByItemID(window, itemID, offset);
    MENU_ITEM_dd_* item = menuPtItemGetBySpriteWork(window, sprite);
    sprite->mColor = GSgfxColAu8To32(color);
    sprite->mWidth = item->mWidth * widthRate;
    winSpriteSetDisp(sprite, widthRate != 0.0f);
}

void menuPtItemDrawAlpha(MENU_ITEM_dd_* item, u8 alpha) {
    windowDrawSprite2(item->mX, item->mY, item->mWidth, item->mHeight, POCKET_MENU_SPRITE_COLOR_RGB_MASK | alpha, 0, (u16)item->mSpriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtItemDrawOffset(MENU_ITEM_dd_* item, long x, long y) {
    windowDrawSprite2(item->mX + x, item->mY + y, item->mWidth, item->mHeight, POCKET_MENU_SPRITE_COLOR_WHITE, 0, (u16)item->mSpriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtItemDrawOffsetXWColor(MENU_ITEM_dd_* item, long x, long width, unsigned long color) {
    windowDrawSprite2(item->mX + x, item->mY, width, item->mHeight, color, 0, (u16)item->mSpriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtSpriteDraw(u16 spriteID, long x, long y) {
    windowDrawSprite(x, y, 0, spriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtSpriteDrawColor(u16 spriteID, long x, long y, unsigned long color) {
    MenuSpriteImage* entry = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    windowDrawSprite2(x, y, entry->mWidth, entry->mHeight, color, 0, spriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtSpriteDrawColor(u16 spriteID, long x, long y, long width, long height, unsigned long color) {
    windowDrawSprite2(x, y, width, height, color, 0, spriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtSpriteDrawCenterColor(u16 spriteID, long x, long y, unsigned long color) {
    MenuSpriteImage* entry = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    x -= entry->mWidth / 2;
    y -= entry->mHeight / 2;
    windowDrawSprite2(x, y, entry->mWidth, entry->mHeight, color, 0, spriteID, 0, (GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
}

void menuPtSpriteDrawAndRollScale(u16 spriteID, unsigned long color,
                                  float x, float y, float angle, float scale) {
    static float xy[4][2];
    static float st[4][2];
    MenuSpriteImage* entry = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    GStexture* texture = (GStexture*)GSresGetResourceAllGroup(entry->mResourceID);
    GSgfxColAu8 rgba = {0, 0, 0, 0};
    rgba.r = color >> 24;
    rgba.g = color >> 16;
    rgba.b = color >> 8;
    rgba.a = color;
    float textureX = entry->mX;
    float textureY = entry->mY;
    float width = entry->mWidth;
    float height = entry->mHeight;

    st[0][0] = st[1][0] = textureX / GStextureGetXsize(texture);
    st[2][0] = st[3][0] = (textureX + width) / GStextureGetXsize(texture);
    st[1][1] = st[2][1] = textureY / GStextureGetYsize(texture);
    st[0][1] = st[3][1] = (textureY + height) / GStextureGetYsize(texture);

    xy[0][0] = xy[1][0] = 0.0f;
    xy[2][0] = xy[3][0] = width;
    xy[1][1] = xy[2][1] = 0.0f;
    xy[0][1] = xy[3][1] = height;
    float halfHeight = height / 2.0f;
    float halfWidth = width / 2.0f;
    for (long index = 0; index < 4; ++index) {
        xy[index][1] -= halfHeight;
        xy[index][0] -= halfWidth;
    }
    for (long index = 0; index < 4; ++index) {
        float vertexX = xy[index][0];
        float vertexY = xy[index][1];
        float rotation = -angle;
        float cosine = (float)cos(rotation);
        float sine = (float)sin(rotation);
        xy[index][0] = vertexY * sine + vertexX * cosine;
        sine = (float)sin(rotation);
        cosine = (float)cos(rotation);
        xy[index][1] = vertexY * cosine - vertexX * sine;
    }
    for (long index = 0; index < 4; ++index) {
        xy[index][0] *= scale;
        xy[index][1] *= scale;
    }
    for (long index = 0; index < 4; ++index) {
        xy[index][0] += x;
        xy[index][1] += y;
    }
    GSprimSetQuadVertexOrder(GS_PRIM_QUAD_VERTEX_ORDER_QUAD);
    GSprimDraw2DTexQuad4V(xy, st, texture, rgba);
}

void menuPtMenuLoad(long menuID) {
    menuOpenCustom(menuID, windowGetActiveID(), 0, POCKET_MENU_WINDOW_OPEN_LOAD, 0, 0);
}

void menuPtWindowWorkDrawPrime(tagWINDOW_WORK* window) {
    for (tagSPRITE_WORK* sprite = window->mSpriteWork; sprite; sprite = sprite->mNext) {
        winSpriteDraw(window, sprite);
    }
    GSgfxBlendMode(GS_GFX_BLEND_ALPHA, POCKET_MENU_BLEND_SOURCE_ALPHA,
                   POCKET_MENU_BLEND_INVERSE_SOURCE_ALPHA);
}

void menuPtWindowWorkDraw(tagWINDOW_WORK* window) {
    if (window) {
        long menuLayer = floorGetMenuLayerID();
        GSgfxLayerID layer = (GSgfxLayerID)window->mLayerID;
        spriteSetOffset((GSgfxLayerID)layer, window->mScrollX, window->mScrollY);
        spriteClearScissor((GSgfxLayerID)layer);
        spriteSetEnv((GSgfxLayerID)layer);
        spriteSetOffset((GSgfxLayerID)menuLayer, window->mScrollX, window->mScrollY);
        spriteClearScissor((GSgfxLayerID)menuLayer);
        spriteSetEnv((GSgfxLayerID)menuLayer);
        menuPtWindowWorkDrawPrime(window);
        spriteSetOffset((GSgfxLayerID)window->mLayerID, 0, 0);
        spriteClearScissor((GSgfxLayerID)window->mLayerID);
        spriteSetEnv((GSgfxLayerID)window->mLayerID);
    }
}

void menuPtWindowWorkDraw(long windowID) {
    menuPtWindowWorkDraw(windowSearchID(windowID));
}

void menuPtPlaySe(unsigned long soundID) {
    GSsndPlayNow(soundID);
}

void menuPtPlayBgm(unsigned long soundID) {
    soundOpenPlay(soundID, POCKET_MENU_BGM_FADE_FRAMES, POCKET_MENU_VOLUME_MAX);
}

void CState::init() {
    memset(this, 0, sizeof(*this));
    mPreviousState = STATE_NONE;
    mFrameTime = timeGetLastFrameSec();
}

void CState::addTime() {
    ++mFrameCount;
    mFrameTime = timeGetLastFrameSec();
    mElapsedTime += mFrameTime;
}

void CState::update() {
    if (mNextState != STATE_NONE) {
        mPreviousState = mState;
        mState = mNextState;
        mNextState = STATE_NONE;
        mElapsedTime = 0.0f;
        mFrameCount = 0;
        mStateFlags[0] = 0;
        mStateFlags[1] = 0;
        mStateFlags[2] = 0;
        memcpy(mPreviousParameters, mParameters, sizeof(mParameters));
    } else {
        addTime();
    }
}

long menuPocketToolCMetaCtrl(tagWINDOW_WORK*) { return 0; }

long menuPocketToolCMetaDraw(tagWINDOW_WORK*) {
    if (CMetaMenu::pCurrentMenu) {
        CMetaMenu::pCurrentMenu->drawHook();
    }
    return 0;
}

long menuPocketToolCMetaButton(tagWINDOW_WORK*) { return 0; }

void CMetaMenu::localWindowSetTable(long* windowTable) {
    mLocalWindowTable = windowTable;
}

void CMetaMenu::localWindowCreateAll() {
    menuOpenCustom(POCKET_MENU_WINDOW_INPUT, windowGetActiveID(), 0, 0, 0, 0);
    for (long index = 0; ; ++index) {
        long windowID = mLocalWindowTable[index];
        if (!windowID) break;
        menuPtMenuLoad(windowID);
        menuSetDisp(windowID, false);
    }
    CMetaMenu::pCurrentMenu = this;
}

void CMetaMenu::localWindowDeleteAll() {
    windowCloseMain(windowSearchID(POCKET_MENU_WINDOW_INPUT));
    CMetaMenu::pCurrentMenu = 0;
    for (long index = 0; ; ++index) {
        long windowID = mLocalWindowTable[index];
        if (!windowID) break;
        windowCloseMain(windowSearchID(windowID));
    }
}

long CMetaMenu::localWindowSearch(long value) {
    for (long i = 0; i < 64; i++) {
        if (mLocalWindowIds[i] == value) return i;
    }
    return -1;
}

void CMetaMenu::localWindowEntry(long value, long index, void (*callback)(CMetaMenu*, long)) {
    mLocalWindowIds[index] = value;
    mLocalWindowCallbacks[index] = callback;
}

void CMetaMenu::localWindowDrawAll() {
    localWindowDrawAll(0, LOCAL_WINDOW_COUNT - 1);
}

void CMetaMenu::localWindowDrawAll(long first, long last) {
    for (int index = first; index <= last; ++index) {
        if (mLocalWindowIds[index]) {
            if (mLocalWindowCallbacks[index]) {
                mLocalWindowCallbacks[index](this, mLocalWindowIds[index]);
            } else {
                menuPtWindowWorkDraw(mLocalWindowIds[index]);
            }
        }
    }
}

void CMetaMenu::localWindowRemove(long windowID) {
    long index = localWindowSearch(windowID);
    if (index != -1) {
        mLocalWindowIds[index] = 0;
    }
}

void _tutorialWindowButtonFunc(tagWINDOW_WORK* window) {
    if (window) {
        u16 key = _pThis->tutorialManage();
        if (key & POCKET_MENU_KEY_A) {
            window->mAccept = true;
        }
        if (key & POCKET_MENU_KEY_B) {
            window->mAccept = true;
            window->mCancel = true;
        }
    }
}

void _tutorialWindowCursorFunc(tagWINDOW_WORK* window) {
    if (window) {
        menuDataBiosGetPtr(window->mMenuID);
        u16 key = _pThis->tutorialManage();
        if (key & POCKET_MENU_KEY_UP) {
            --window->mCursorIndex;
            if (window->mCursorIndex < 0) {
                window->mCursorIndex = (short)(_selectItemQuantity - 1);
            }
        } else if (key & POCKET_MENU_KEY_DOWN) {
            ++window->mCursorIndex;
            if (window->mCursorIndex >= _selectItemQuantity) {
                window->mCursorIndex = 0;
            }
        }
        window->mCursorState.mSelection.mCursor = (short)window->mCursorIndex;
    }
}

long CMetaMenu::actionMenu(unsigned long* choices, int quantity, long x, long y, long initialChoice) {
    MenuWindowData* windowData = menuDataBiosGetPtr(POCKET_MENU_WINDOW_ACTION);
    MenuWindowData savedData;
    if (mTutorialActive) {
        _selectItemQuantity = quantity;
        _pThis = this;
        savedData = *windowData;
        windowData->mCursor.mCursorMode = MENU_WINDOW_CALLBACK_CUSTOM;
        windowData->mCursorFunction = _tutorialWindowCursorFunc;
        windowData->mButton.mButtonMode = MENU_WINDOW_CALLBACK_CUSTOM;
        windowData->mButtonFunction = _tutorialWindowButtonFunc;
    }
    long selected = menuSubOpenSelect(0, choices, quantity, x, y, initialChoice);
    if (selected == -1) {
        selected = quantity - 1;
    }
    if (mTutorialActive) {
        *windowData = savedData;
    }
    return selected;
}

void CMetaMenu::aButtonWait() {
    bool accepted = false;
    while (!accepted) {
        switch (getKeyInfo()->mKeyTrg) {
        case POCKET_MENU_KEY_A:
            accepted = true;
            menuPtPlaySe(mAcceptSound);
            break;
        }
        updateAll();
    }
}

tagWINDOW_KEY* CMetaMenu::getKeyInfo() {
    if (mTutorialActive) {
        memset(&mKeyInfo, 0, sizeof(mKeyInfo));
        mKeyInfo.mKeyRepeat = mKeyInfo.mKeyTrg = tutorialManage();
        return &mKeyInfo;
    }
    memset(&mKeyInfo, 0, sizeof(mKeyInfo));
    if (windowGetActiveID() != POCKET_MENU_WINDOW_INPUT) {
        return &mKeyInfo;
    }
    u16 repeat = 0;
    u16 trigger = windowGetKeyTrg();
    u16 held = windowGetKeyOn();
    bool delayRepeat = false;
    if (trigger) {
        repeat = trigger;
    } else if (held && held == mPreviousKeyOn) {
        if (mKeyRepeatFrames >= POCKET_MENU_KEY_REPEAT_DELAY_FRAMES) {
            repeat = held;
        } else {
            delayRepeat = true;
        }
    }
    if (delayRepeat) {
        ++mKeyRepeatFrames;
    } else {
        mKeyRepeatFrames = 0;
    }
    mPreviousKeyOn = held;
    mKeyInfo.mKeyRepeat = repeat;
    mKeyInfo.mKeyTrg = trigger;
    return &mKeyInfo;
}

void CMetaMenu::alphaFadeInKick() {
    mAlphaFadeFrames = (unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    mAlphaFadeState = ALPHA_FADE_IN;
}

void CMetaMenu::alphaFadeOutKick() {
    mAlphaFadeFrames = (unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    mAlphaFadeState = ALPHA_FADE_OUT;
}

void CMetaMenu::alphaFadeUpdate() {
    if (mAlphaFadeFrames) {
        mAlphaFadeFrames -= timeGetLastFrameTime();
        if (mAlphaFadeFrames <= 0) {
            if (mAlphaFadeState == ALPHA_FADE_IN) {
                mAlphaFadeState = ALPHA_FADE_VISIBLE;
            } else if (mAlphaFadeState == ALPHA_FADE_OUT) {
                mAlphaFadeState = ALPHA_FADE_BLACK;
            }
        }
    }
}

void CMetaMenu::alphaFadeOutAndWait() {
    alphaFadeOutKick();
    do {
        updateAll();
    } while (!isAlphaFadeEnd());
    updateAll();
}

u8 CMetaMenu::getAlphaFadeAlpha() const {
    switch (mAlphaFadeState) {
    case ALPHA_FADE_VISIBLE:
        return 255;
    case ALPHA_FADE_IN: {
        return ((long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate()) -
                mAlphaFadeFrames) * 255 /
            (long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    }
    case ALPHA_FADE_OUT:
        return mAlphaFadeFrames * 255 /
            (long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    case ALPHA_FADE_BLACK:
        return 0;
    default:
        return 0;
    }
}

void CMetaMenu::alphaFadeDraw() {
    static float xy[4][2] = {
        {0.0f, 0.0f}, {640.0f, 0.0f},
        {640.0f, 480.0f}, {0.0f, 480.0f}
    };
    GSgfxColAu8 color = {0, 0, 0, 255};
    GSprimSetQuadVertexOrder(GS_PRIM_QUAD_VERTEX_ORDER_QUAD);
    color.a = 255 - getAlphaFadeAlpha();
    if (color.a) {
        GSprimDraw2DQuad4V(xy, color);
    }
}

void CMetaMenu::systemFadeOutKeepAfterFloorPush() {
    GSthreadSwitch();
    fadeSet(FADE_MODE_OUT, 0.0f);
}

void CMetaMenu::systemFadeInKick() {
    fadeSetToBlackInIfBlack(0.5f);
}

void CMetaMenu::systemFadeOutKick(float seconds) {
    if (seconds == -1.0f) {
        seconds = 0.5f;
    }
    fadeSetToBlackOutIfNotBlack(seconds);
}

void CMetaMenu::systemFadeOutSetAndWait(float seconds) {
    systemFadeOutKick(seconds);
    systemFadeWaitToEnd();
}

void CMetaMenu::systemFadeWaitToEnd() {
    while (fadeCheck(false)) {
        updateAll();
    }
}

void CMetaMenu::vsyncUpdate() { GSthreadSwitch(); }

void CMetaMenu::updateAll() { vsyncUpdate(); }

void CMetaMenu::drawHook()
{
    GSgfxLayerSetActive((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
    GSgfxLayerID layer = floorGetMenuLayerID();
    spriteSetOffset((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER, 0, 0);
    spriteClearScissor((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
    spriteSetEnv((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
    spriteSetOffset(layer, 0, 0);
    spriteClearScissor(layer);
    spriteSetEnv(layer);
    drawAll();
    mAnimationFrameTime += timeGetLastFrameTime();
    alphaFadeUpdate();
    tutorialManageUpdate();
    spriteSetOffset((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER, 0, 0);
    spriteClearScissor((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
    spriteClearEnv((GSgfxLayerID)POCKET_MENU_SPRITE_LAYER);
    spriteSetOffset(layer, 0, 0);
    spriteClearScissor(layer);
    spriteClearEnv(layer);
    GSgfxLayerSetActive((GSgfxLayerID)POCKET_MENU_WORLD_LAYER);
}

float CMetaMenu::calcAnimationTimeRate0to1(float seconds)
{
    unsigned long frames = seconds * GSgfxVideoGetVsyncRate();
    return (unsigned long)(mAnimationFrameTime % frames) / (float)frames;
}

float CMetaMenu::calcAnimationTimeRate0to1to0(float seconds)
{
    float rate = calcAnimationTimeRate0to1(seconds);
    if (rate > 0.5f)
        rate = 1.0f - rate;
    rate *= 2.0f;
    return rate;
}

long CMetaMenu::calcAnimationTimeRate0toX(float seconds, long maximum)
{
    float rate = calcAnimationTimeRate0to1(seconds);
    rate = maximum < (maximum + 1) * rate ? maximum : (maximum + 1) * rate;
    return rate;
}

void CMetaMenu::waitAtTime(float seconds)
{
    float elapsed = 0.0f;
    for (;;) {
        updateAll();
        if (elapsed > seconds)
            break;
        elapsed += timeGetLastFrameSec();
    }
}

void CInfoWindow::init(long windowID, unsigned long itemID)
{
    mItemID = 0;
    mKeyTrigger = 0;
    mDisplay = false;
    mAnimateMessage = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
    mWindowID = windowID;
    mItemID = itemID;
    close();
}

void CInfoWindow::initTutorialTalk()
{
    mItemID = 0;
    mKeyTrigger = 0;
    mDisplay = false;
    mAnimateMessage = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
    mWindowID = POCKET_MENU_WINDOW_TUTORIAL_TALK;
    tagWINDOW_WORK* window = windowSearchID(mWindowID);
    tagSPRITE_WORK* sprite = menuPtGetSpriteWorkByHookFunction(window, (void*)winMsgDraw, 0, 0);
    sprite->mHookFunction = 0;
    sprite->mDrawFlags = 0;
    mItemID = sprite->mItemID;
    close();
}

void CInfoWindow::close()
{
    if (mAnimateMessage)
        GSmsgKill(mMessageID);
    mDisplay = false;
    mAnimateMessage = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
}

void CInfoWindow::open(unsigned long messageID, bool animate)
{
    mAnimateMessage = animate;
    if (messageID) {
        mMessageID = messageID;
        if (mAnimateMessage)
            GSmsgExec(mMessageID, 0, 0);
    }
    mDisplay = true;
}

bool CInfoWindow::isMsgEnd()
{
    return !GSmsgIsCheck(mMessageID);
}

void CInfoWindow::setMsgCtrlValue(unsigned long id, unsigned long value)
{
    unsigned long index = 0;

    while (mMsgCtrls[index].mId != 0) {
        index++;
    }
    mMsgCtrls[index].mId = id;
    mMsgCtrls[index].mValue = value;
}

void CInfoWindow::draw()
{
    if (!mDisplay)
        return;
    menuPtWindowWorkDraw(mWindowID);
    for (unsigned long index = 0; mMsgCtrls[index].mId; ++index)
        msgctrlSetValue(mMsgCtrls[index].mId, mMsgCtrls[index].mValue);
    tagWINDOW_WORK* window = windowSearchID(mWindowID);
    tagSPRITE_WORK* sprite = menuPtSpriteWorkGetByItemID(window, mItemID, 0);
    long layer = winSpriteGetLayerID(window, sprite);
    if (mAnimateMessage) {
        spriteSetOffset((GSgfxLayerID)layer, window->mScrollX + sprite->mX, window->mScrollY + sprite->mY);
        spriteClearScissor((GSgfxLayerID)layer);
        spriteSetEnv((GSgfxLayerID)layer);
        GSmsgMain((GSgfxLayerID)layer, mMessageID, mKeyTrigger, !((signed char)window->mFlags & WINDOW_FLAG_MESSAGE_PAUSED));
        spriteSetOffset((GSgfxLayerID)layer, 0, 0);
        spriteClearScissor((GSgfxLayerID)layer);
        spriteSetEnv((GSgfxLayerID)layer);
    } else {
        GSmsgPrint2((GSgfxLayerID)layer, window->mScrollX + sprite->mX, window->mScrollY + sprite->mY, POCKET_MENU_SPRITE_COLOR_WHITE, mMessageID);
    }
}

void CInfoWindow::exec(CMetaMenu& menu, unsigned long messageID, bool closeOnEnd)
{
    open(messageID, true);
    do {
        mKeyTrigger = windowGetKeyTrg();
        menu.updateAll();
    } while (!isMsgEnd());
    if (closeOnEnd)
        close();
}

void CMenuPokemonCursor::init()
{
    memset(this, 0, sizeof(*this));
    mDisplay = true;
    pokemonInit(&mPokemon);
    mAnt.init();
    mBeforAnt.init();
    mHighlight = false;
    defaultPositionTbl.x = 0;
    defaultPositionTbl.y = 0;
    mPositionX = 0;
    mPositionY = 0;
    mPositionTable = &defaultPositionTbl;
}

void CMenuPokemonCursor::setAnimationOff()
{
    mAnimationState = ANIMATION_NONE;
    mAnimationRate = 0.0f;
    mBeforAnt.init();
}

void CMenuPokemonCursor::setPositionTable(PocketVec<short> *table, long index)
{
    setPositionIndex(index);
    mPrevPositionIndex = 0;
    mPositionTable = table;
}

void CMenuPokemonCursor::setPositionIndex(long index)
{
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    mPositionX = pos.x;
    mPositionY = pos.y;
    mPrevPositionIndex = mPositionIndex;
    mPositionIndex = index;
}

void CMenuPokemonCursor::setPositionTableDummy()
{
    PocketVec<short> *dummy = &defaultPositionTbl;
    long zero = 0;
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    *dummy = pos;
    mPositionTable = dummy;
    mPositionIndex = zero;
    mPrevPositionIndex = zero;
}

void CMenuPokemonCursor::setPokemon(Pokemon *pokemon)
{
    mBeforAnt = mAnt;

    if (pokemon == 0) {
        pokemonInit(&mPokemon);
        mAnt.init();
    } else {
        memcpy(&mPokemon, pokemon, sizeof(Pokemon));
        battlePokemonGetPokeBody(&mAnt, &mPokemon);
        mAnt.setAnimation(0);
    }
}

void CMenuPokemonCursor::setBeforAnt(CAnt &ant)
{
    mBeforAnt = ant;
}

void CMenuPokemonCursor::deletePokemon()
{
    pokemonInit(&mPokemon);
    mBeforAnt = mAnt;
    mAnt.init();
}

bool CMenuPokemonCursor::isCatchPokemon()
{
    return mPokemon.checkValid();
}

void CMenuPokemonCursor::draw()
{
    if (!mDisplay)
        return;

    PocketVec<short> position;
    if (mAnimationState == ANIMATION_MOVE) {
        PocketVec<short>& next = mPositionTable[mPositionIndex];
        position.x = mPositionX + (next.x - mPositionX) * mAnimationRate;
        position.y = mPositionY + (next.y - mPositionY) * mAnimationRate;
    } else {
        position = mPositionTable[mPositionIndex];
    }

    if (isCatchPokemon()) {
        position.x += 8;
        position.y -= 8;
        if (mAnimationState == ANIMATION_CHANGE) {
            float angle = CURSOR_PI;
            angle *= mAnimationRate;
            short spread = 20.0f * (float)sin(angle);
            short rise = -8.0f * mAnimationRate;
            PocketVec<short> before = position;
            PocketVec<short> current = position;
            before.x += spread;
            current.x -= spread;
            before.y -= rise;
            current.y -= -8 - rise;
            if (mAnimationRate < CURSOR_ANIMATION_HALF) {
                mAnt.drawAndUpdate(current);
                mBeforAnt.drawAndUpdate(before);
            } else {
                mBeforAnt.drawAndUpdate(before);
                mAnt.drawAndUpdate(current);
            }
        } else {
            mAnt.drawAndUpdate(position);
        }
    } else if (mAnimationState == ANIMATION_DELETE) {
        mBeforAnt.mScale = 1.0f - mAnimationRate;
        mBeforAnt.drawAndUpdate();
    }

    float phase = mFlashTime / CURSOR_FLASH_CYCLE_SECONDS;
    if (phase > CURSOR_FLASH_PHASE_MIDPOINT)
        phase = 1.0f - phase;
    float flash = 2.0f * phase;
    mFlashTime += timeGetLastFrameSec();
    if (mFlashTime >= CURSOR_FLASH_CYCLE_SECONDS)
        mFlashTime = fmod(mFlashTime, 0.5);

    position.x -= 45;
    position.y -= 45;
    unsigned long color = mHighlight ? CURSOR_HIGHLIGHT_COLOR : POCKET_MENU_SPRITE_COLOR_WHITE;
    if (!isCatchPokemon()) {
        MenuSpriteImage* image = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(CURSOR_CORNER_SPRITE));
        long width = image->mWidth;
        long height = image->mHeight;
        long spread = (short)(5.0f * flash);
        if (spread > 4)
            spread = 4;
        menuPtSpriteDrawColor(CURSOR_CORNER_SPRITE, (short)(position.x + 4 + spread), (short)(position.y + 4 + spread), width, height, color);
        menuPtSpriteDrawColor(CURSOR_CORNER_SPRITE, (short)(position.x + 50 - spread), (short)(position.y + 4 + spread), -width, height, color);
        menuPtSpriteDrawColor(CURSOR_CORNER_SPRITE, (short)(position.x + 4 + spread), (short)(position.y + 50 - spread), width, -height, color);
        menuPtSpriteDrawColor(CURSOR_CORNER_SPRITE, (short)(position.x + 50 - spread), (short)(position.y + 50 - spread), -width, -height, color);
    } else {
        menuPtSpriteDrawColor(CURSOR_HELD_SPRITE, (short)(position.x + 14), (short)(position.y + 14), (color & CURSOR_COLOR_RGB_MASK) | (short)(100.0f + 155.0f * flash));
    }
    menuPtSpriteDrawColor(CURSOR_MARK_SPRITE, (short)(position.x + 7), (short)(position.y + 35), color);
}

void CMenuGlobalState::init() {
    memset(this, 0, sizeof(*this));
    mPokemonCursor.init();
    mState.init();
}

u16 pokemonindex(s32 index)
{
	index++;
	if (index > CELEBI_SPECIES_NUMBER) {
		index += RESERVED_SPECIES_SLOTS;
	}
	return index;
}

void menuPocketSetDarkPokemon(Pokemon& pokemon, float darkRate, u16 shadowID) {
    pokemonSetDarkPokemonStatus(&pokemon, shadowID);
    u16 initialDarkPoint = pokemonBiosGetInitDp(&pokemon);
    float darkPoint = initialDarkPoint * darkRate;
    pokemon.setDarkPoint(darkPoint);
    DarkPokemon* darkPokemon = darkPokemonGetDarkPokemon(shadowID);
    darkPokemon->setStatus(DARK_POKEMON_STATUS_SNATCH_SUCCESS);
    darkPokemon->setSnatchIndex(shadowID);
}

void menuPocketDebugMakePokemonBySN(Pokemon& pokemon, u16 serial, bool dark, float darkRate) {
    menuPocketDebugMakePokemonByID(pokemon,
                                  pokemonindex(serial % DEBUG_POKEMON_SPECIES_COUNT),
                                  dark, darkRate);
}

void makePocketDeubugMakeDarkPokemonReset() {
    _darkPokemonIndex = DEBUG_DARK_POKEMON_INITIAL_INDEX;
}

void menuPocketDebugMakePokemonByID(Pokemon& pokemon, u16 species, bool dark, float darkRate) {
    pokemonCreate(&pokemon, species, DEBUG_POKEMON_LEVEL,
                  (void*)gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    if (dark) {
        for (int retry = 0; retry < DEBUG_DARK_POKEMON_RETRY_COUNT; ++retry) {
            if (++_darkPokemonIndex >= darkPokemonList_number) {
                _darkPokemonIndex = 1;
            }
            menuPocketSetDarkPokemon(pokemon, darkRate, _darkPokemonIndex);
            if (pokemonBiosGetInitDp(&pokemon)) {
                break;
            }
        }
    }
}

u16 menuPocketDebugMakeDarkPokemonByID(Pokemon& pokemon, u16 shadowID, float darkRate) {
    u16 species = darkPokemonGetDarkPokemon(shadowID)->getPokemonID();
    pokemonCreate(&pokemon, species, DEBUG_POKEMON_LEVEL,
                  (void*)gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    menuPocketSetDarkPokemon(pokemon, darkRate, shadowID);
    return pokemonBiosGetInitDp(&pokemon);
}

float menuGetPokemonScale(unsigned long sequence, int species) {
    _GSmodel* model = Sequence_GetGSmodel(sequence);
    float modelSize;
    switch (Sequence_GetModelSize(sequence)) {
    case -2: modelSize = 0.6f; break;
    case -1: modelSize = 0.75f; break;
    case 0: modelSize = 1.0f; break;
    case 1: modelSize = 1.15f; break;
    case 2: modelSize = 1.25f; break;
    case 3: modelSize = 1.4f; break;
    default: modelSize = 1.0f; break;
    }
    float scale, angle;
    memoGetScaleAngle(species, &scale, &angle);
    GSvec size;
    GSboundGetSize(GSmodelGetBound(model), &size, 0, 0);
    float result = 11.0f / (scale * ((size.x > size.y ? size.x : size.y) / modelSize));
    static float multi = 2.0f;
    return result * multi;
}

void menuPocketButtonWait() {
    do {
        GSthreadSwitch();
    } while (!(windowGetKeyTrg() & (POCKET_MENU_KEY_A | POCKET_MENU_KEY_B)));
}

void menuPtStopSound(MenuPtWorkSound* sound) {
    unsigned long bgmID = soundGetBGMID();
    u8 bgmVolume;
    if (bgmID) {
        bgmVolume = soundGetVolume(bgmID);
        soundOpenPlay(POCKET_SOUND_SILENCE_BGM, POCKET_SOUND_FADE_TIME,
                      POCKET_SOUND_VOLUME_MAX);
    } else {
        bgmVolume = 0;
    }
    unsigned long envID = soundGetEnvID();
    u8 envVolume;
    if (envID) {
        envVolume = soundGetVolume(envID);
    } else {
        envVolume = 0;
    }
    sound->mBgmID = bgmID;
    sound->mEnvID = envID;
    sound->mBgmVolume = bgmVolume;
    sound->mEnvVolume = envVolume;
}

void menuPtSaveSound(MenuPtWorkSound* sound) {
    unsigned long bgmID = soundGetBGMID();
    u8 bgmVolume;
    if (bgmID) {
        bgmVolume = soundGetVolume(bgmID);
    } else {
        bgmVolume = 0;
    }
    unsigned long envID = soundGetEnvID();
    u8 envVolume;
    if (envID) {
        envVolume = soundGetVolume(envID);
    } else {
        envVolume = 0;
    }
    sound->mBgmID = bgmID;
    sound->mEnvID = envID;
    sound->mBgmVolume = bgmVolume;
    sound->mEnvVolume = envVolume;
}

void menuPtRestoreSound(MenuPtWorkSound* sound) {
    if (sound->mBgmID && sound->mBgmID != (unsigned long)soundGetBGMID()) {
        soundOpenPlay(sound->mBgmID, POCKET_SOUND_FADE_TIME, sound->mBgmVolume);
    }
    if (sound->mEnvID && sound->mEnvID != (unsigned long)soundGetEnvID()) {
        soundOpenPlayEnv(sound->mEnvID, POCKET_SOUND_FADE_TIME, sound->mEnvVolume);
    }
}
