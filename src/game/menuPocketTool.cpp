#include <types.h>
#include <menuPokemonCursor.h>
#include <string.h>
#include <menuInfoWindow.h>
#include <CMetaMenu.h>
#include <menuPtSpriteWork.h>
#include <menuPocketTool.h>
#include <CMenuGlobalState.h>
#include <menu.h>
#include <time.h>
#include <sound.h>
#include <GSsnd.h>
#include <fade.h>
#include <GSmsg.h>
#include <GSthread.h>
#include <GSgfxVideo.h>
#include <winMsg.h>
#include <menuSpriteBios.h>
#include <sprite.h>
#include <msgctrl.h>
#include <floor.h>
#include <GSgfx.h>
#include <GSprim.h>
#include <GStexture.h>
#include <GSres.h>
#include <gamedata.h>
#include <GSmodel.h>
#include <GSbound.h>
#include <sequence.h>
#include <memo.h>
#include <darkPokemon.h>
#include <pokemonBios.h>
#include <menuDataBios.h>
#include <menuSub.h>
#include <math.h>

enum {
    MENU_META_BASE = 218,
    MENU_TUTORIAL_TALK = 227,
    MENU_ACTION_SELECT = 89,
    SPRITE_CURSOR_POINTER = 532,
    SPRITE_CURSOR_HELD = 533,
    SPRITE_CURSOR_CORNER = 534,
    SPRITE_RELIVE_HALL_POKEMON = 735,
    SOUND_SILENT_BGM = 8
};

enum MenuKey { MENU_KEY_UP = 1, MENU_KEY_DOWN = 2, MENU_KEY_A = 16, MENU_KEY_B = 32 };

inline float sin(float angle) { return (float)sin((double)angle); }
inline float cos(float angle) { return (float)cos((double)angle); }

const GSgfxColAu8 GSgfxColAu8_Black = {0, 0, 0, 255};

CMetaMenu *CMetaMenu::pCurrentMenu;
static CMetaMenu *_pThis;
static long _selectItemQuantity;
PocketVec<short> CMenuPokemonCursor::defaultPositionTbl;

template<> PocketVec<short> PocketVec<short>::zero = {0, 0};
unsigned short menuRelivehallPcBoxDefaultPokemonSprite = SPRITE_RELIVE_HALL_POKEMON;
long CMetaMenu::defautlLocalWindowOpenTable[] = {0};
unsigned long _darkPokemonIndex = 12;

void CMetaMenu::tutorialExitKick()
{
}

bool CMetaMenu::isAnimationStateNone()
{
    return true;
}

bool getBitTable(unsigned long *table, long index)
{
    return table[index / 32] & (1 << (index % 32));
}

void setBitTable(unsigned long *table, long index, bool flag)
{
    long pos = index / 32;
    long bit = index % 32;

    if (flag == 0) {
        return;
    }
    table[pos] |= (1 << bit);
}

void menuPocketToolFloatToZero(float &value, float speed)
{
    if (value > 0.0f) {
        if ((value -= speed) < 0.0f) value = 0.0f;
    } else if (value < 0.0f) {
        if ((value += speed) > 0.0f) value = 0.0f;
    }
}

float dirRefreshPI(float dir)
{
    float result = dir - (float)(long)(0.15915494f * dir) * 6.2831855f;

    if (result > 3.1415927f) {
        result -= 6.2831855f;
    } else if (result < (-3.1415927f)) {
        result += 6.2831855f;
    }

    return result;
}

float dirRefresh2PI(float dir)
{
    float result = dir - (float)(long)(0.15915494f * dir) * 6.2831855f;

    if (result < 0.0f) {
        result += 6.2831855f;
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

    return degreeRefresh180((long)(180.0f * dir / 3.1415927f + 0.5f));
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
    float range = (to == from) ? 6.2831855f : dirSubOne(to, from);

    return dirSubOne(dir, from) <= range;
}

float xyxy2distance(const PocketVec<short> &point, const PocketVec<short> &center)
{
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    return sqrtf(dx * dx + dy * dy);
}

unsigned long GSgfxColAu8To32(GSgfxColAu8 color)
{
    unsigned long result;
    memcpy(&result, &color, sizeof(color));
    return result;
}

tagMENU_ITEM_DATA *menuPtItemGetByMenuIdAndItemId(long menuID, unsigned long itemID, long offset)
{
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(menuID, itemID);
    for (long i = 0; i < offset; ++i) {
        item = menuItemBiosGetPtr(menuID, item->mNextID);
    }
    return item;
}

void menuPtSpriteWorkSetMsgId(long menuID, unsigned long itemID, long offset, unsigned long message)
{
    menuPtSpriteWorkGetByItemID(menuID, itemID, offset)->mMsgID = message;
}

tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(long menuID, unsigned long itemID, long offset)
{
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

void menuPtSpriteWorkSetMenuItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long itemID)
{
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, itemID);
    MenuSpriteValue *value = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(item->mSpriteID));
    sprite->mSpriteValue = value->mDrawValue;
    sprite->mSpriteX = value->mX;
    sprite->mSpriteY = value->mY;
    sprite->mTextureWidth = value->mWidth;
    sprite->mTextureHeight = value->mHeight;
    sprite->mX = item->mMessageX;
    sprite->mY = item->mOffsetA;
    sprite->mWidth = item->mOffsetB;
    sprite->mHeight = item->mOffsetC;
}

void menuPtSpriteWorkSetSpriteData(tagSPRITE_WORK *sprite, MenuSpriteData *data)
{
    MenuSpriteValue *value = menuSpriteBiosGetValPtr(data);
    sprite->mSpriteValue = value->mDrawValue;
    sprite->mSpriteX = value->mX;
    sprite->mSpriteY = value->mY;
    sprite->mTextureWidth = value->mWidth;
    sprite->mTextureHeight = value->mHeight;
    sprite->mWidth = value->mWidth;
    sprite->mHeight = value->mHeight;
}

void menuPtSpriteWorkSetSpriteId(tagSPRITE_WORK *sprite, unsigned long spriteID)
{
    MenuSpriteData *data = menuSpriteBiosGetPtr(spriteID);
    menuPtSpriteWorkSetSpriteData(sprite, data);
}

void menuPtSpriteWorkSetSw(tagWINDOW_WORK *window, unsigned long itemID, long offset, bool visible)
{
    winSpriteSetDisp(menuPtSpriteWorkGetByItemID(window, itemID, offset), visible);
}

void menuPtSpriteWorkSetSw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, bool visible)
{
    (void)window;
    winSpriteSetDisp(sprite, visible);
}

void menuPtSpriteWorkSetWidthRateColor(tagWINDOW_WORK *window, unsigned long itemID, long offset, float rate, GSgfxColAu8 color)
{
    tagSPRITE_WORK *sprite = menuPtSpriteWorkGetByItemID(window, itemID, offset);
    tagMENU_ITEM_DATA *item = menuPtItemGetBySpriteWork(window, sprite);
    sprite->mColor = GSgfxColAu8To32(color);
    sprite->mWidth = item->mOffsetB * rate;
    winSpriteSetDisp(sprite, rate != 0.0f);
}

void menuPtItemDrawAlpha(MENU_ITEM_dd_ *item, unsigned char alpha)
{
    windowDrawSprite2(item->mMessageX, item->mOffsetA, item->mOffsetB, item->mOffsetC, 0xffffff00 | alpha, 0, item->mSpriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtItemDrawOffset(MENU_ITEM_dd_ *item, long x, long y)
{
    windowDrawSprite2(item->mMessageX + x, item->mOffsetA + y, item->mOffsetB, item->mOffsetC, -1, 0, item->mSpriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtItemDrawOffsetXWColor(MENU_ITEM_dd_ *item, long x, long width, unsigned long color)
{
    windowDrawSprite2(item->mMessageX + x, item->mOffsetA, width, item->mOffsetC, color, 0, item->mSpriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtSpriteDraw(unsigned short spriteID, long x, long y)
{
    windowDrawSprite(x, y, 0, spriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtSpriteDrawColor(unsigned short spriteID, long x, long y, unsigned long color)
{
    MenuSpriteValue *sprite = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    windowDrawSprite2(x, y, sprite->mWidth, sprite->mHeight, color, 0, spriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtSpriteDrawColor(unsigned short spriteID, long x, long y, long width, long height, unsigned long color)
{
    windowDrawSprite2(x, y, width, height, color, 0, spriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtSpriteDrawCenterColor(unsigned short spriteID, long x, long y, unsigned long color)
{
    MenuSpriteValue *sprite = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    x -= sprite->mWidth / 2;
    y -= sprite->mHeight / 2;
    windowDrawSprite2(x, y, sprite->mWidth, sprite->mHeight, color, 0, spriteID, 0, GSGFX_LAYER_MENU);
}

void menuPtSpriteDrawAndRollScale(unsigned short spriteID, unsigned long rgba, float x, float y, float roll, float scale)
{
    MenuSpriteValue *sprite = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
    GStexture *texture = (GStexture *)GSresGetResourceAllGroup(sprite->mDrawValue);
    GSgfxColAu8 color = {0, 0, 0, 0};
    color.r = rgba >> 24;
    color.g = rgba >> 16;
    color.b = rgba >> 8;
    color.a = rgba;
    float sx = sprite->mX;
    float sy = sprite->mY;
    float width = sprite->mWidth;
    float height = sprite->mHeight;
    static GSvec2 xy[4];
    static GSvec2 st[4];
    st[0].x = st[1].x = sx / GStextureGetXsize(texture);
    st[2].x = st[3].x = (sx + width) / GStextureGetXsize(texture);
    st[1].y = st[2].y = sy / GStextureGetYsize(texture);
    st[0].y = st[3].y = (sy + height) / GStextureGetYsize(texture);
    xy[0].x = xy[1].x = 0.0f;
    xy[2].x = xy[3].x = width;
    xy[1].y = xy[2].y = 0.0f;
    xy[0].y = xy[3].y = height;
    for (long i = 0; i < 4; ++i) {
        xy[i].y -= height / 2.0f;
        xy[i].x -= width / 2.0f;
    }
    for (long i = 0; i < 4; ++i) {
        float vx = xy[i].x, vy = xy[i].y;
        xy[i].x = vy * (float)sin(-roll) + vx * (float)cos(-roll);
        xy[i].y = vy * (float)cos(-roll) - vx * (float)sin(-roll);
    }
    for (long i = 0; i < 4; ++i) {
        xy[i].x *= scale;
        xy[i].y *= scale;
    }
    for (long i = 0; i < 4; ++i) {
        xy[i].x += x;
        xy[i].y += y;
    }
    GSprimSetQuadVertexOrder(1);
    GSprimDraw2DTexQuad4V(xy, st, texture, color);
}

void menuPtMenuLoad(long menuID)
{
    menuOpenCustom(menuID, windowGetActiveID(), 0, 4, 0, 0);
}

void menuPtWindowWorkDrawPrime(tagWINDOW_WORK *window)
{
    for (tagSPRITE_WORK *sprite = window->mSpriteWork; sprite != 0; sprite = sprite->mNext) {
        winSpriteDraw(window, sprite);
    }
    GSgfxBlendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA);
}

void menuPtWindowWorkDraw(tagWINDOW_WORK *window)
{
    if (window != 0) {
        GSgfxLayerID layer = floorGetMenuLayerID();
        GSgfxLayerID windowLayer = window->mLayerID;
        spriteSetOffset(windowLayer, window->mScrollX, window->mScrollY);
        spriteClearScissor(windowLayer);
        spriteSetEnv(windowLayer);
        spriteSetOffset(layer, window->mScrollX, window->mScrollY);
        spriteClearScissor(layer);
        spriteSetEnv(layer);
        menuPtWindowWorkDrawPrime(window);
        spriteSetOffset(window->mLayerID, 0, 0);
        spriteClearScissor(window->mLayerID);
        spriteSetEnv(window->mLayerID);
    }
}

void menuPtWindowWorkDraw(long menuID)
{
    menuPtWindowWorkDraw(windowSearchID(menuID));
}

void menuPtPlaySe(unsigned long soundID) { GSsndPlayNow(soundID); }

void menuPtPlayBgm(unsigned long soundID) { soundOpenPlay(soundID, 50, 255); }

void CState::init()
{
    memset(this, 0, sizeof(*this));
    mPrevious = -1;
    mDeltaTime = timeGetLastFrameSec();
}

void CState::addTime()
{
    mFrameCount++;
    mDeltaTime = timeGetLastFrameSec();
    mTime += mDeltaTime;
}

void CState::update()
{
    if (mNext != -1) {
        mPrevious = mCurrent;
        mCurrent = mNext;
        mNext = -1;
        mTime = 0.0f;
        mFrameCount = 0;
        reserved98 = 0;
        reserved9C = 0;
        reservedA0 = 0;
        memcpy(mPreviousWork, mWork, sizeof(mWork));
    } else {
        addTime();
    }
}

extern "C" long menuPocketToolCMetaCtrl(tagWINDOW_WORK *) { return 0; }

extern "C" long menuPocketToolCMetaDraw(tagWINDOW_WORK *)
{
    if (CMetaMenu::pCurrentMenu != 0) CMetaMenu::pCurrentMenu->drawHook();
    return 0;
}

extern "C" long menuPocketToolCMetaButton(tagWINDOW_WORK *) { return 0; }

void CMetaMenu::localWindowSetTable(long *table)
{
    mLocalWindowTable = table;
}

void CMetaMenu::localWindowCreateAll()
{
    menuOpenCustom(MENU_META_BASE, windowGetActiveID(), 0, 0, 0, 0);
    for (long i = 0;; ++i) {
        long id = mLocalWindowTable[i];
        if (id == 0) break;
        menuPtMenuLoad(id);
        menuSetDisp(id, 0);
    }
    pCurrentMenu = this;
}

void CMetaMenu::localWindowDeleteAll()
{
    windowCloseMain(windowSearchID(MENU_META_BASE));
    pCurrentMenu = 0;
    for (long i = 0;; ++i) {
        long id = mLocalWindowTable[i];
        if (id == 0) break;
        windowCloseMain(windowSearchID(id));
    }
}

long CMetaMenu::localWindowSearch(long value) {
    for (long i = 0; i < 64; i++) {
        if (mLocalWindowIds[i] == value) {
            return i;
        }
    }
    return -1;
}

void CMetaMenu::localWindowEntry(long value, long index, void (*callback)(CMetaMenu*, long)) {
    mLocalWindowIds[index] = value;
    mLocalWindowCallbacks[index] = callback;
}

void CMetaMenu::localWindowDrawAll()
{
    localWindowDrawAll(0, 63);
}

void CMetaMenu::localWindowDrawAll(long from, long to)
{
    for (int index = from; index <= to; ++index) {
        if (mLocalWindowIds[index] != 0) {
            if (mLocalWindowCallbacks[index] != 0) {
                mLocalWindowCallbacks[index](this, mLocalWindowIds[index]);
            } else {
                menuPtWindowWorkDraw(mLocalWindowIds[index]);
            }
        }
    }
}

void CMetaMenu::localWindowRemove(long windowID)
{
    long index = localWindowSearch(windowID);
    if (index != -1) {
        mLocalWindowIds[index] = 0;
    }
}

void _tutorialWindowButtonFunc(tagWINDOW_WORK *window)
{
    if (window != 0) {
        unsigned short keys = _pThis->tutorialManage();
        if (keys & MENU_KEY_A) window->mAccept = 1;
        if (keys & MENU_KEY_B) {
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

void _tutorialWindowCursorFunc(tagWINDOW_WORK *window)
{
    if (window != 0) {
        menuDataBiosGetPtr(window->mMenuID);
        unsigned short keys = _pThis->tutorialManage();
        if (keys & MENU_KEY_UP) {
            --window->mCursorIndex;
            if (window->mCursorIndex < 0) window->mCursorIndex = (short)(_selectItemQuantity - 1);
        } else if (keys & MENU_KEY_DOWN) {
            ++window->mCursorIndex;
            if (window->mCursorIndex >= _selectItemQuantity) window->mCursorIndex = 0;
        }
        window->mCursor.y = window->mCursorIndex;
    }
}

long CMetaMenu::actionMenu(unsigned long *messages, int count, long x, long y, long cursor)
{
    MenuData *data = menuDataBiosGetPtr(MENU_ACTION_SELECT);
    MenuData saved;
    if (mTutorialInput) {
        _selectItemQuantity = count;
        _pThis = this;
        saved = *data;
        data->mCursorMode = 4;
        data->mCursorFunction = _tutorialWindowCursorFunc;
        data->mButtonMode = 4;
        data->mButtonFunction = _tutorialWindowButtonFunc;
    }
    long result = menuSubOpenSelect(0, messages, count, x, y, cursor);
    if (result == -1) result = count - 1;
    if (mTutorialInput) *data = saved;
    return result;
}

void CMetaMenu::aButtonWait()
{
    bool finished = false;
    while (!finished) {
        switch (getKeyInfo()->mKeyTrg) {
        case MENU_KEY_A:
            finished = true;
            menuPtPlaySe(mWindowInfo[1]);
            break;
        }
        updateAll();
    }
}

tagWINDOW_KEY *CMetaMenu::getKeyInfo()
{

    if (mTutorialInput) {
        memset(&mKeyInfo, 0, sizeof(mKeyInfo));
        mKeyInfo.mKeyRepeat = mKeyInfo.mKeyTrg = tutorialManage();
        return &mKeyInfo;
    }
    memset(&mKeyInfo, 0, sizeof(mKeyInfo));
    if (windowGetActiveID() != MENU_META_BASE) return &mKeyInfo;
    unsigned short repeat = 0;
    unsigned short trigger = windowGetKeyTrg();
    unsigned short held = windowGetKeyOn();
    bool count = false;
    if (trigger != 0) repeat = trigger;
    else if (held != 0 && held == (unsigned short)mPreviousKeyOn) {
        if (mKeyRepeatFrames >= 12) repeat = held;
        else count = true;
    }
    if (count) mKeyRepeatFrames++;
    else mKeyRepeatFrames = 0;
    mPreviousKeyOn = held;
    mKeyInfo.mKeyRepeat = repeat;
    mKeyInfo.mKeyTrg = trigger;
    return &mKeyInfo;
}

void CMetaMenu::alphaFadeInKick()
{
    mAlphaFadeFrames = (unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    mAlphaFadeState = FADE_IN;
}

void CMetaMenu::alphaFadeOutKick()
{
    mAlphaFadeFrames = (unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    mAlphaFadeState = FADE_OUT;
}

void CMetaMenu::alphaFadeUpdate()
{
    if (mAlphaFadeFrames != 0) {
        mAlphaFadeFrames -= timeGetLastFrameTime();
        if (mAlphaFadeFrames <= 0) {
            if (mAlphaFadeState == FADE_IN) mAlphaFadeState = FADE_OPAQUE;
            else if (mAlphaFadeState == FADE_OUT) mAlphaFadeState = FADE_TRANSPARENT;
        }
    }
}

void CMetaMenu::alphaFadeOutAndWait()
{
    alphaFadeOutKick();
    do {
        updateAll();
    } while (!isAlphaFadeEnd());
    updateAll();
}

unsigned char CMetaMenu::getAlphaFadeAlpha() const
{
    switch (mAlphaFadeState) {
    case FADE_OPAQUE: return 255;
    case FADE_IN: return 255 * ((long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate()) - mAlphaFadeFrames) / (long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    case FADE_OUT: return 255 * mAlphaFadeFrames / (long)(unsigned long)(0.5f * GSgfxVideoGetVsyncRate());
    case FADE_TRANSPARENT: return 0;
    }
    return 0;
}

void CMetaMenu::alphaFadeDraw()
{
    static GSvec2 xy[4] = {{0, 0}, {640, 0}, {640, 480}, {0, 480}};
    GSgfxColAu8 color = {0, 0, 0, 255};
    GSprimSetQuadVertexOrder(1);
    color.a = 255 - getAlphaFadeAlpha();
    if (color.a != 0) GSprimDraw2DQuad4V(xy, color);
}

void CMetaMenu::systemFadeOutKeepAfterFloorPush()
{
    GSthreadSwitch();
    fadeSet(3, 0.0f);
}

void CMetaMenu::systemFadeInKick() { fadeSetToBlackInIfBlack(0.5f); }

void CMetaMenu::systemFadeOutKick(float time)
{
    if (time == -1.0f) time = 0.5f;
    fadeSetToBlackOutIfNotBlack(time);
}

void CMetaMenu::systemFadeOutSetAndWait(float time)
{
    systemFadeOutKick(time);
    systemFadeWaitToEnd();
}

void CMetaMenu::systemFadeWaitToEnd()
{
    while (fadeCheck(0)) updateAll();
}

void CMetaMenu::vsyncUpdate() { GSthreadSwitch(); }

void CMetaMenu::updateAll() { vsyncUpdate(); }

void CMetaMenu::drawHook()
{
    GSgfxLayerSetActive(GSGFX_LAYER_MENU);
    GSgfxLayerID layer = floorGetMenuLayerID();
    spriteSetOffset(7, 0, 0);
    spriteClearScissor(7);
    spriteSetEnv(7);
    spriteSetOffset(layer, 0, 0);
    spriteClearScissor(layer);
    spriteSetEnv(layer);
    drawAll();
    mAnimationTime += timeGetLastFrameTime();
    alphaFadeUpdate();
    tutorialManageUpdate();
    spriteSetOffset(7, 0, 0);
    spriteClearScissor(7);
    spriteClearEnv(7);
    spriteSetOffset(layer, 0, 0);
    spriteClearScissor(layer);
    spriteClearEnv(layer);
    GSgfxLayerSetActive(GSGFX_LAYER_DEFAULT);
}

float CMetaMenu::calcAnimationTimeRate0to1(float time)
{
    unsigned long cycle = time * GSgfxVideoGetVsyncRate();
    return (float)(unsigned long)(mAnimationTime % cycle) / cycle;
}

float CMetaMenu::calcAnimationTimeRate0to1to0(float time)
{
    float rate = calcAnimationTimeRate0to1(time);
    if (rate > 0.5f) rate = 1.0f - rate;
    rate *= 2.0f;
    return rate;
}

long CMetaMenu::calcAnimationTimeRate0toX(float time, long count)
{
    float rate = calcAnimationTimeRate0to1(time);
    rate = count < ((count + 1) * rate) ? count : ((count + 1) * rate);
    return (long)rate;
}

void CMetaMenu::waitAtTime(float time)
{
    float elapsed = 0.0f;
    for (;;) {
        updateAll();
        if (elapsed > time) break;
        elapsed += timeGetLastFrameSec();
    }
}

void CInfoWindow::init(long menuID, unsigned long itemID)
{
    mItemID = 0;
    mKeyTrigger = 0;
    mOpen = false;
    mAnimated = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
    mMenuID = menuID;
    mItemID = itemID;
    close();
}

void CInfoWindow::initTutorialTalk()
{
    mItemID = 0;
    mKeyTrigger = 0;
    mOpen = false;
    mAnimated = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
    mMenuID = MENU_TUTORIAL_TALK;
    tagSPRITE_WORK *work = menuPtGetSpriteWorkByHookFunction(windowSearchID(mMenuID), (void *)winMsgDraw, 0, 0);
    work->mHookFunction = 0;
    work->_05 = 0;
    mItemID = work->mItemID;
    close();
}

void CInfoWindow::close()
{
    if (mAnimated) GSmsgKill(mMessageID);
    mOpen = false;
    mAnimated = false;
    mMessageID = 0;
    memset(mMsgCtrls, 0, sizeof(mMsgCtrls));
}

void CInfoWindow::open(unsigned long messageID, bool animated)
{
    mAnimated = animated;
    if (messageID != 0) {
        mMessageID = messageID;
        if (mAnimated) GSmsgExec(mMessageID, 0, 0);
    }
    mOpen = true;
}

bool CInfoWindow::isMsgEnd() { return !GSmsgIsCheck(mMessageID); }

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
    if (mOpen) {
        menuPtWindowWorkDraw(mMenuID);
        for (unsigned long i = 0; mMsgCtrls[i].mId != 0; ++i) {
            msgctrlSetValue(mMsgCtrls[i].mId, mMsgCtrls[i].mValue);
        }
        tagWINDOW_WORK *window = windowSearchID(mMenuID);
        tagSPRITE_WORK *sprite = menuPtSpriteWorkGetByItemID(window, mItemID, 0);
        GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
        if (mAnimated) {
            spriteSetOffset(layer, (short)(window->mScrollX + sprite->mX), (short)(window->mScrollY + sprite->mY));
            spriteClearScissor(layer);
            spriteSetEnv(layer);
            GSmsgMain(layer, mMessageID, mKeyTrigger, !(window->_00 & 4));
            spriteSetOffset(layer, 0, 0);
            spriteClearScissor(layer);
            spriteSetEnv(layer);
        } else {
            GSmsgPrint2(layer, window->mScrollX + sprite->mX, window->mScrollY + sprite->mY, -1, mMessageID);
        }
    }
}

void CInfoWindow::exec(CMetaMenu &menu, unsigned long messageID, bool closeOnEnd)
{
    open(messageID, true);
    do {
        mKeyTrigger = windowGetKeyTrg();
        menu.updateAll();
    } while (!isMsgEnd());
    if (closeOnEnd) close();
}

void CMenuPokemonCursor::init()
{
    memset(this, 0, sizeof(*this));
    mDraw = true;
    pokemonInit(&mPokemon);
    mAnt.init();
    mBeforAnt.init();
    mHighlight = false;
    CMenuPokemonCursor::defaultPositionTbl.x = 0;
    CMenuPokemonCursor::defaultPositionTbl.y = 0;
    mPositionX = 0;
    mPositionY = 0;
    mPositionTable = &CMenuPokemonCursor::defaultPositionTbl;
}

void CMenuPokemonCursor::setAnimationOff()
{
    mAnimationState = ANIMATION_NONE;
    mAnimationTime = 0.0f;
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
    PocketVec<short> *dummy = &CMenuPokemonCursor::defaultPositionTbl;
    long indexZero = 0;
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    *dummy = pos;
    mPositionTable = dummy;
    mPositionIndex = indexZero;
    mPrevPositionIndex = indexZero;
}

void CMenuPokemonCursor::setPokemon(Pokemon *pokemon)
{
    mBeforAnt = mAnt;

    if (pokemon == 0) {
        pokemonInit(&mPokemon);
        mAnt.init();
    } else {
        memcpy(&mPokemon, pokemon, sizeof(Pokemon));
        mAnt.initByPokemonAndSetColorAnimationInAny(&mPokemon);
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

bool CMenuPokemonCursor::isCatchPokemon() { return mPokemon.checkValid(); }

void CMenuPokemonCursor::draw()
{
    if (!mDraw) return;
    PocketVec<short> position;
    if (mAnimationState == ANIMATION_MOVE) {
        const PocketVec<short> &target = mPositionTable[mPositionIndex];
        position.x = mPositionX + mAnimationTime * (target.x - mPositionX);
        position.y = mPositionY + mAnimationTime * (target.y - mPositionY);
    } else position = mPositionTable[mPositionIndex];
    if (isCatchPokemon()) {
        position.x += 8;
        position.y -= 8;
        if (mAnimationState == ANIMATION_EXCHANGE) {
            float wave = sin(3.1415927f * mAnimationTime);
            short side = 20.0f * wave;
            short lift = -8.0f * mAnimationTime;
            PocketVec<short> previous = position, current = position;
            previous.x += side;
            current.x -= side;
            previous.y -= lift;
            current.y -= -8 - lift;
            if (mAnimationTime < 0.5f) {
                mAnt.drawAndUpdate(current);
                mBeforAnt.drawAndUpdate(previous);
            } else {
                mBeforAnt.drawAndUpdate(previous);
                mAnt.drawAndUpdate(current);
            }
        } else {
            mAnt.drawAndUpdate(position);
        }
    } else if (mAnimationState == ANIMATION_REMOVE) {
        mBeforAnt.mScale = 1.0f - mAnimationTime;
        mBeforAnt.drawAndUpdate();
    }
    float pulse = mCursorTime / 0.5f;
    if (pulse > 0.5f) pulse = 1.0f - pulse;
    pulse = 2.0f * pulse;
    mCursorTime += timeGetLastFrameSec();
    if (mCursorTime >= 0.5f) mCursorTime = fmod(mCursorTime, 0.5);
    position.x -= 45;
    position.y -= 45;
    unsigned long color = mHighlight ? 0xffc800ff : 0xffffffff;
    if (!isCatchPokemon()) {
        MenuSpriteValue *sprite = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(SPRITE_CURSOR_CORNER));
        long width = sprite->mWidth, height = sprite->mHeight;
        long offset = (short)(5.0f * pulse);
        long top, left;
        if (offset > 4) offset = 4;
        left = position.x + offset;
        top = position.y + offset;
        menuPtSpriteDrawColor(SPRITE_CURSOR_CORNER, (short)(left + 4), (short)(top + 4), width, height, color);
        menuPtSpriteDrawColor(SPRITE_CURSOR_CORNER, (short)(position.x + 50 - offset), (short)(top + 4), -width, height, color);
        menuPtSpriteDrawColor(SPRITE_CURSOR_CORNER, (short)(left + 4), (short)(position.y + 50 - offset), width, -height, color);
        menuPtSpriteDrawColor(SPRITE_CURSOR_CORNER, (short)(position.x + 50 - offset), (short)(position.y + 50 - offset), -width, -height, color);
    } else {
        short alpha = 155.0f * pulse + 100.0f;
        menuPtSpriteDrawColor(SPRITE_CURSOR_HELD, (short)(position.x + 14), (short)(position.y + 14), (color & 0xffffff00) | alpha);
    }
    menuPtSpriteDrawColor(SPRITE_CURSOR_POINTER, (short)(position.x + 7), (short)(position.y + 35), color);
}

void CMenuGlobalState::init()
{
    memset(this, 0, sizeof(*this));
    mCursor.init();
    mState.init();
}

u16 pokemonindex(int index)
{
	index++;
	if (index > 0xfb) {
		index += 0x19;
	}
	return index;
}

void menuPocketSetDarkPokemon(Pokemon &pokemon, float rate, unsigned short darkID)
{
    pokemonSetDarkPokemonStatus(&pokemon, darkID);
    unsigned short dp = pokemonBiosGetInitDp(&pokemon);
    pokemon.setDarkPoint(dp * rate);
    DarkPokemon *dark = darkPokemonGetDarkPokemon(darkID);
    dark->setStatus(DARK_POKEMON_SNATCHED);
    dark->setSnatchIndex(darkID);
}

void menuPocketDebugMakePokemonBySN(Pokemon &pokemon, unsigned short serial, bool dark, float rate)
{
    menuPocketDebugMakePokemonByID(pokemon, pokemonindex((int)serial % 386), dark, rate);
}

void makePocketDeubugMakeDarkPokemonReset() { _darkPokemonIndex = 12; }

void menuPocketDebugMakePokemonByID(Pokemon &pokemon, unsigned short species, bool dark, float rate)
{
    pokemonCreate(&pokemon, species, 25, gamedataGetStatus(0, 1));
    if (dark) {
        for (long tries = 0; tries < 10; ++tries) {
            if (++_darkPokemonIndex >= darkPokemonList_number) _darkPokemonIndex = 1;
            menuPocketSetDarkPokemon(pokemon, rate, _darkPokemonIndex);
            if (pokemonBiosGetInitDp(&pokemon) != 0) break;
        }
    }
}

void menuPocketDebugMakeDarkPokemonByID(Pokemon &pokemon, unsigned short darkID, float rate)
{
    unsigned short species = darkPokemonGetDarkPokemon(darkID)->getPokemonID();
    pokemonCreate(&pokemon, species, 25, gamedataGetStatus(0, 1));
    menuPocketSetDarkPokemon(pokemon, rate, darkID);
    pokemonBiosGetInitDp(&pokemon);
}

float menuGetPokemonScale(unsigned long modelID, int pokemonID)
{
    _GSmodel *model = Sequence_GetGSmodel(modelID);
    float size;
    switch (Sequence_GetModelSize(modelID)) {
    case -2: size = 0.6f; break;
    case -1: size = 0.75f; break;
    case 0: size = 1.0f; break;
    case 1: size = 1.15f; break;
    case 2: size = 1.25f; break;
    case 3: size = 1.4f; break;
    default: size = 1.0f; break;
    }
    float scale, angle;
    memoGetScaleAngle(pokemonID, &scale, &angle);
    GSvec bounds;
    GSboundGetSize(GSmodelGetBound(model), &bounds, 0, 0);
    float extent = bounds.x > bounds.y ? bounds.x : bounds.y;
    float result = 11.0f / (scale * (extent / size));
    static float multi = 2.0f;
    return result * multi;
}

void menuPocketButtonWait()
{
    do { GSthreadSwitch(); } while (!(windowGetKeyTrg() & (MENU_KEY_A | MENU_KEY_B)));
}

void menuPtStopSound(MenuPtWorkSound *sound)
{
    unsigned long bgmID = soundGetBGMID();
    unsigned char bgmVolume;
    if (bgmID) {
        bgmVolume = soundGetVolume(bgmID);
        soundOpenPlay(SOUND_SILENT_BGM, 50, 255);
    } else bgmVolume = 0;
    unsigned long envID = soundGetEnvID();
    unsigned char envVolume;
    if (envID != 0) envVolume = soundGetVolume(envID);
    else envVolume = 0;
    sound->mBgmID = bgmID;
    sound->mEnvID = envID;
    sound->mBgmVolume = bgmVolume;
    sound->mEnvVolume = envVolume;
}

void menuPtSaveSound(MenuPtWorkSound *sound)
{
    unsigned long bgmID = soundGetBGMID();
    unsigned char bgmVolume;
    if (bgmID != 0) bgmVolume = soundGetVolume(bgmID);
    else bgmVolume = 0;
    unsigned long envID = soundGetEnvID();
    unsigned char envVolume;
    if (envID != 0) envVolume = soundGetVolume(envID);
    else envVolume = 0;
    sound->mBgmID = bgmID;
    sound->mEnvID = envID;
    sound->mBgmVolume = bgmVolume;
    sound->mEnvVolume = envVolume;
}

void menuPtRestoreSound(MenuPtWorkSound *sound)
{
    if (sound->mBgmID != 0 && sound->mBgmID != soundGetBGMID()) {
        soundOpenPlay(sound->mBgmID, 50, sound->mBgmVolume);
    }
    if (sound->mEnvID != 0 && sound->mEnvID != soundGetEnvID()) {
        soundOpenPlayEnv(sound->mEnvID, 50, sound->mEnvVolume);
    }
}
