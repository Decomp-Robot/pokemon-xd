#include <window.h>
#include <menuDataBios.h>
#include <menuItemBios.h>
#include <menuSpriteBios.h>
#include <menuSeBios.h>
#include <menu.h>
#include <floor.h>
#include <sprite.h>
#include <GSmem.h>
#include <GSlog.h>
#include <GSthread.h>
#include <GSmsg.h>
#include <string.h>
#include <stdarg.h>

const u32 WINDOW_COLOR_WHITE = (0xFFu << 24) | (0xFFu << 16) | (0xFFu << 8) | 0xFFu;
const s32 WINDOW_RESULT_CANCELLED = -1;

enum WindowOpenFlags {
    WINDOW_OPEN_TOP_PRIORITY = 0x2,
    WINDOW_OPEN_NO_ACTIVATE = 0x4
};

const s32 WINDOW_OPEN_PRIORITY_MAX = 100;
const s32 WINDOW_ARG_COUNT_MAX = 8;
const s8 WINDOW_STATE_FLAGS_INITIAL = WINDOW_STATE_FLAG_INUSE | WINDOW_STATE_FLAG_DISP | WINDOW_RUN_UPDATE;
const s32 WINDOW_CURSOR_FOLLOW_SEQUENCE = 321;
const s32 WINDOW_VIEW_WIDTH = 640;
const s32 WINDOW_VIEW_HEIGHT = 480;

void _winCalcWindowSize(s32 menuID, const MENU_ITEM_dd_ *item, s16 *width, s16 *height);
s32 _winInsertWork(tagWINDOW_WORK *window, s8 priority);
tagWINDOW_WORK *_winGetNewWork(void);

const s8 SPRITE_FLAGS_INITIAL = SPRITE_FLAG_INUSE | SPRITE_FLAG_DISP | SPRITE_FLAG_INITIALIZED;

u8 _windowCheckParent(tagWINDOW_WORK *window, tagWINDOW_WORK *parent);
s32 _windowDeleteWork(tagWINDOW_WORK *window);
void _windowSetCloseFlag(tagWINDOW_WORK *window, bool respectMenuFlag);
s32 _windowCreateItemSprite(tagWINDOW_WORK *window);

WINDOW_SYS_WORK winSysWork;
WINDOW_SYS_WORK winSysWork2;

tagWINDOW_KEY* windowGetKeyInfo(void)
{
    return &winSysWork.mKey;
}

tagWINDOW_KEY* windowGetKeyInfo2(void)
{
    return &winSysWork2.mKey;
}

u16 windowGetKeyRepeat(void)
{
    u16 key = windowGetKeyInfo()->mKeyRepeat;
    key |= (windowGetKeyInfo()->mKeyOn & WINDOW_KEY_DIRECTION)
        ? (u16)0 : windowGetKeyInfo2()->mKeyRepeat;
    return key;
}

u16 windowGetKeyOn(void)
{
    u16 key = windowGetKeyInfo()->mKeyOn;
    key |= (key & WINDOW_KEY_DIRECTION)
        ? (u16)0 : windowGetKeyInfo2()->mKeyOn;
    return key;
}

u16 windowGetKeyTrg(void)
{
    u16 key = windowGetKeyInfo()->mKeyTrg;
    key |= (windowGetKeyInfo()->mKeyOn & WINDOW_KEY_DIRECTION)
        ? (u16)0 : windowGetKeyInfo2()->mKeyTrg;
    return key;
}

tagWINDOW_KEY* windowGetPortKeyInfo(u8 portMask)
{
    static tagWINDOW_KEY key;
    u8 portBits[4] = { 0x1, 0x2, 0x4, 0x8 };
    int port;
    tagWINDOW_KEY* portKey;

    memset(&key, 0, sizeof(key));
    for (port = 0; port < 4; port++) {
        portKey = &winSysWork.mPortKeys[port];
        if (portMask & portBits[port]) {
            key.mKeyOn |= portKey->mKeyOn;
            key.mKeyOnPrev |= portKey->mKeyOnPrev;
            key.mKeyTrg |= portKey->mKeyTrg;
            key.mKeyRepeat |= portKey->mKeyRepeat;
            key.mKeyRepeatActive |= portKey->mKeyRepeatActive;
        }
    }
    return &key;
}

tagWINDOW_KEY* windowGetPortKeyInfo2(u8 portMask)
{
    static tagWINDOW_KEY key;
    u8 portBits[4] = { 0x1, 0x2, 0x4, 0x8 };
    int port;
    tagWINDOW_KEY* portKey;

    memset(&key, 0, sizeof(key));
    for (port = 0; port < 4; port++) {
        portKey = &winSysWork2.mPortKeys[port];
        if (portMask & portBits[port]) {
            key.mKeyOn |= portKey->mKeyOn;
            key.mKeyOnPrev |= portKey->mKeyOnPrev;
            key.mKeyTrg |= portKey->mKeyTrg;
            key.mKeyRepeat |= portKey->mKeyRepeat;
            key.mKeyRepeatActive |= portKey->mKeyRepeatActive;
        }
    }
    return &key;
}

void windowInit(u16 capacity)
{
    memset(&winSysWork, 0, sizeof(winSysWork));
    memset(&winSysWork2, 0, sizeof(winSysWork2));
    s32 poolSize = capacity * sizeof(tagWINDOW_WORK);
    winSysWork.mWindowPool = (tagWINDOW_WORK *)GSmemAlloc(poolSize);
    if (!winSysWork.mWindowPool) {
        GSlogWrite("windowInitの初期化に失敗しました\n");
        return;
    }
    winSysWork.mWindowCapacity = capacity;
    memset(winSysWork.mWindowPool, 0, poolSize);
    winSpriteInit();
}

void _winCalcWindowSize(s32 menuID, const MENU_ITEM_dd_ *item, s16 *width, s16 *height)
{
    *width = 0;
    s32 left = WINDOW_VIEW_WIDTH;
    s32 top = WINDOW_VIEW_HEIGHT;
    s32 right = 0;
    s32 bottom = 0;
    *height = 0;
    while (true) {
        menuSpriteBiosGetPtr(item->mSpriteID);
        u32 messageSize;
        if (item->mMessageID) {
            messageSize = GSmsgGetMessageSize(item->mMessageID);
        } else {
            messageSize = 0;
        }
        s16 messageWidth = messageSize >> 16;
        s16 messageHeight = messageSize & 0xFFFF;
        if (left > item->mX) {
            left = item->mX;
        }
        if (right < item->mX + item->mWidth) {
            right = item->mX + item->mWidth;
        }
        if (right < item->mX + messageWidth) {
            right = item->mX + messageWidth;
        }
        if (top > item->mY) {
            top = item->mY;
        }
        if (bottom < item->mY + item->mHeight) {
            bottom = item->mY + item->mHeight;
        }
        if (bottom < item->mY + messageHeight) {
            bottom = item->mY + messageHeight;
        }
        if (item->mChainEndFlag) {
            break;
        }
        item = menuItemBiosGetPtr(menuID, item->mNextID);
    }
    *width = right - left;
    *height = bottom - top;
}

tagWINDOW_WORK *_winGetNewWork(void)
{
    tagWINDOW_WORK *window = winSysWork.mWindowPool;
    for (s32 index = winSysWork.mWindowCapacity; index > 0; index--, window++) {
        if (!window->mStateFlags) {
            memset(window, 0, sizeof(*window));
            window->mStateFlags = WINDOW_STATE_FLAGS_INITIAL;
            window->mScaleX = 1.0f;
            window->mScaleY = 1.0f;
            window->mColor = WINDOW_COLOR_WHITE;
            return window;
        }
    }
    GSlogWrite("Error!! _winGetNewWork() [ワークの確保に失敗]\n");
    return NULL;
}

s32 _winInsertWork(tagWINDOW_WORK *window, s8 priority)
{
    if (!winSysWork.mWindowList) {
        winSysWork.mWindowList = window;
    } else {
        tagWINDOW_WORK *entry = winSysWork.mWindowList;
        while (true) {
            if (entry->mOpenPriority > priority) {
                window->mPrevious = entry->mPrevious;
                window->mNext = entry;
                if (!entry->mPrevious) {
                    winSysWork.mWindowList = window;
                } else {
                    entry->mPrevious->mNext = window;
                }
                entry->mPrevious = window;
                break;
            }
            if (!entry->mNext) {
                entry->mNext = window;
                window->mPrevious = entry;
                window->mNext = NULL;
                break;
            }
            entry = entry->mNext;
        }
    }
    return 0;
}

static inline void _windowFreeAllocMemory(tagWINDOW_WORK *window)
{
    if (window->mAllocMemory) {
        GSmemFree(window->mAllocMemory);
        window->mAllocMemory = NULL;
    }
}

static inline void *_windowGetAllocMemory(tagWINDOW_WORK *window)
{
    return !window->mAllocMemory ? NULL : window->mAllocMemory;
}

s32 _windowDeleteWork(tagWINDOW_WORK *window)
{
    if (!window) {
        return 0;
    }
    if (!window->mPrevious) {
        winSysWork.mWindowList = window->mNext;
    } else {
        window->mPrevious->mNext = window->mNext;
    }
    if (window->mNext) {
        window->mNext->mPrevious = window->mPrevious;
    }
    winSpriteRelease(&window->mSpriteWork);
    winSpriteRelease(&window->mCursorSpriteWork);
    if (window->mAllocMemory) {
        _windowFreeAllocMemory(window);
    }
    window->mStateFlags = 0;
    window->mMenuID = 0;
    return 0;
}

u8 _windowCheckParent(tagWINDOW_WORK *window, tagWINDOW_WORK *parent)
{
    if (window == parent) {
        return 0;
    }
    for (; window; window = window->mParent) {
        if (window->mParent == parent) {
            return 1;
        }
    }
    return 0;
}

tagWINDOW_WORK *windowOpen(s32 *cursor, s32 menuID, s32 parentID, u32 flags,
                          s32 argCount, va_list args)
{
    tagWINDOW_WORK *window;
    tagWINDOW_WORK *parent = windowSearchID(parentID);
    GSgfxLayerID layer;
    if (parent) {
        layer = parent->mLayerID;
    } else {
        layer = menuItemBiosGetLayerID(menuID);
    }
    if (layer == GSGFX_LAYER_INVALID) {
        return NULL;
    }
    MenuData *menu = menuDataBiosGetPtr(menuID);
    window = windowSearchID(menuID);
    if (!window) {
        s8 priority;
        if (menu->mOpenPriorityFlag) {
            priority = WINDOW_OPEN_PRIORITY_MAX;
        } else if (flags & WINDOW_OPEN_TOP_PRIORITY) {
            priority = WINDOW_OPEN_PRIORITY_MAX;
        } else {
            if (!parentID) {
                parentID = windowGetActiveID();
            }
            parent = windowSearchID(parentID);
            if (parent) {
                priority = parent->mOpenPriority + 1;
            } else {
                priority = 0;
            }
            if (priority > WINDOW_OPEN_PRIORITY_MAX) {
                priority = WINDOW_OPEN_PRIORITY_MAX;
            }
        }
        window = _winGetNewWork();
        if (!window) {
            return NULL;
        }
        window->mState = WINDOW_STATE_INIT;
        window->mMenuID = menuID;
        window->mOpenPriority = priority;
        window->mCursorMode = menu->mCursorMode;
        window->mParent = parent;
        window->mScrollX = menu->mPositionX;
        window->mScrollY = menu->mPositionY;
        window->mLayerID = layer;
        window->mItemTable = menu->mItemTable;
        MENU_ITEM_dd_ *item = menuItemBiosGetPtr(menuID, menu->mFirstItemID);
        while (true) {
            if (item->mShownFlag) {
                window->mShownItemCount++;
            }
            if (item->mChainEndFlag) {
                break;
            }
            item = menuItemBiosGetPtr(menuID, item->mNextID);
        }
        _windowCreateItemSprite(window);
        _winInsertWork(window, priority);
        menuPlaySe(menuID, MENU_SOUND_OPEN);
    } else {
        window->mAccept = 0;
        window->mCancel = 0;
        window->mState = WINDOW_STATE_INIT;
        window->mFlag = 1;
    }
    if (!cursor) {
        if (menu->mCursorID) {
            CURSOR pos = cursorBiosGetPos(menu->mCursorID);
            window->mCursor.x = pos.x;
            window->mCursor.y = pos.y;
        }
    } else {
        window->mCursor.x = 0;
        window->mCursor.y = (s8)*cursor;
    }
    windowCreateCursorSprite(window);
    window->mEnablePortMask = menuGetEnablePort();
    window->mOpenFlags = flags;
    if (argCount > WINDOW_ARG_COUNT_MAX) {
        argCount = WINDOW_ARG_COUNT_MAX;
    }
    for (s32 index = 0; index < argCount; index++) {
        window->mArgs[index] = va_arg(args, u32);
    }
    if (!menu->mButtonMode) {
        window->mAccept = 1;
    } else if (!(flags & WINDOW_OPEN_NO_ACTIVATE)) {
        winSysWork.mActiveID = menuID;
    }
    if (winSysWork.mActiveID != menuID) {
        window->mInactive = 1;
    }
    if (menu->mControlFunction) {
        menu->mControlFunction(window);
    }
    if (window->mFlag) {
        window->mCloseRequested = 0;
        window->mState = WINDOW_STATE_ACTIVE;
        window->mFlag = 0;
    }
    return window;
}

void windowCreateCursorSprite(tagWINDOW_WORK *window)
{
    MenuData *menu = menuDataBiosGetPtr(window->mMenuID);
    MENU_ITEM_dd_ *item = windowGetCursorToItem(window);
    if (!item) {
        return;
    }
    winSpriteRelease(&window->mCursorSpriteWork);
    s16 cursorSpriteID = item->mCursorSpriteID;
    if (!cursorSpriteID) {
        return;
    }
    MenuSpriteData *data = menuSpriteBiosGetPtr(cursorSpriteID);
    s32 spriteIndex = 0;
    while (true) {
        MenuSpriteValue *value = menuSpriteBiosGetValPtr(data);
        tagSPRITE_WORK *sprite = winSpriteAdd(&window->mCursorSpriteWork);
        if (!sprite) {
            winSpriteRelease(&window->mCursorSpriteWork);
            return;
        }
        if (data->mValueType == MENU_SPRITE_VALUE_TEXTURE) {
            sprite->mTypeFlags |= SPRITE_TYPE_TEXTURE;
            sprite->mSpriteValue = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_PRIMITIVE) {
            sprite->mTypeFlags |= SPRITE_TYPE_PRIMITIVE;
            sprite->mDrawID = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_FLASH) {
            sprite->mTypeFlags |= SPRITE_TYPE_FLASH;
            sprite->mDrawID = value->mDrawValue;
        }
        sprite->mItemID = menu->mFirstItemID;
        sprite->mX = item->mX + data->mOffsetX;
        sprite->mY = item->mY + data->mOffsetY;
        sprite->mWidth = value->mWidth;
        sprite->mHeight = value->mHeight;
        sprite->mSpriteX = value->mX;
        sprite->mSpriteY = value->mY;
        sprite->mTextureWidth = value->mWidth;
        sprite->mTextureHeight = value->mHeight;
        sprite->mRgba.mAlpha = data->mAlpha;
        if (data->mSequenceID) {
            winSetSequence(&sprite->mSeq, data->mSequenceID);
        }
        if (spriteIndex) {
            winSetSequence(&sprite->mSeq, WINDOW_CURSOR_FOLLOW_SEQUENCE);
        }
        if (data->mChainEndFlag) {
            break;
        }
        data = menuSpriteBiosGetPtr(data->mNextID);
        spriteIndex++;
    }
}

s32 _windowCreateItemSprite(tagWINDOW_WORK *window)
{
    s16 width, height;
    s16 itemID = menuDataBiosGetPtr(window->mMenuID)->mFirstItemID;
    MENU_ITEM_dd_ *item = menuItemBiosGetPtr(window->mMenuID, itemID);
    winSpriteRelease(&window->mSpriteWork);
    _winCalcWindowSize(window->mMenuID, item, &width, &height);
    while (true) {
        item = menuItemBiosGetPtr(window->mMenuID, itemID);
        tagSPRITE_WORK *sprite = winSpriteAdd(&window->mSpriteWork);
        if (!sprite) {
            winSpriteRelease(&window->mSpriteWork);
            return 1;
        }
        sprite->mItemID = itemID;
        sprite->mX = item->mX;
        sprite->mY = item->mY;
        sprite->mWidth = item->mWidth;
        sprite->mHeight = item->mHeight;
        sprite->mRgba.mAlpha = item->mAlpha;
        sprite->mAlphaMode = item->mAlphaMode;
        sprite->mUseFloorMenuLayer = item->mLayer;
        sprite->mAlignMode = item->mAlignMode;
        if (item->mSpriteID) {
            MenuSpriteData *data = menuSpriteBiosGetPtr(item->mSpriteID);
            MenuSpriteValue *value = menuSpriteBiosGetValPtr(data);
            if (data->mValueType == MENU_SPRITE_VALUE_TEXTURE) {
                sprite->mTypeFlags |= SPRITE_TYPE_TEXTURE;
                sprite->mSpriteValue = value->mDrawValue;
            } else if (data->mValueType == MENU_SPRITE_VALUE_PRIMITIVE) {
                sprite->mTypeFlags |= SPRITE_TYPE_PRIMITIVE;
                sprite->mDrawID = value->mDrawValue;
            } else if (data->mValueType == MENU_SPRITE_VALUE_FLASH) {
                sprite->mTypeFlags |= SPRITE_TYPE_FLASH;
                sprite->mDrawID = value->mDrawValue;
            }
            sprite->mSpriteX = value->mX;
            sprite->mSpriteY = value->mY;
            sprite->mTextureWidth = value->mWidth;
            sprite->mTextureHeight = value->mHeight;
            sprite->mRgba.mAlpha = sprite->mRgba.mAlpha * data->mAlpha / MENU_ITEM_ALPHA_OPAQUE;
            if (data->mUseWindowSize) {
                sprite->mWidth = width;
                sprite->mHeight = height;
            }
            if (data->mSequenceID) {
                winSetSequence(&sprite->mSeq, data->mSequenceID);
            }
        }
        sprite->mMsgID = item->mMessageID;
        if (item->mHook) {
            sprite->mTypeFlags |= SPRITE_TYPE_HOOK;
            sprite->mHookFunction = item->mHook;
        }
        if (item->mChainEndFlag) {
            break;
        }
        itemID = item->mNextID;
    }
    return 0;
}

void _windowSetCloseFlag(tagWINDOW_WORK *window, bool respectMenuFlag)
{
    if (!respectMenuFlag || !menuDataBiosGetPtr(window->mMenuID)->mIgnoreCloseRequest) {
        window->mCloseRequested = 1;
    }
}

s32 windowClose(tagWINDOW_WORK *window, u32 flags)
{
    u8 foundActiveWindow = 0;
    tagWINDOW_WORK *child;
    bool respectMenuFlag = flags & WINDOW_CLOSE_RESPECT_MENU_FLAG;
    if (!(flags & WINDOW_CLOSE_KEEP_CHILDREN)) {
        for (child = winSysWork.mWindowList; child; child = child->mNext) {
            if (_windowCheckParent(child, window)) {
                _windowSetCloseFlag(child, respectMenuFlag);
            }
        }
    }
    if (!(flags & WINDOW_CLOSE_CHILDREN_ONLY) && window) {
        _windowSetCloseFlag(window, respectMenuFlag);
        if (flags & WINDOW_CLOSE_KEEP_CHILDREN) {
            for (child = winSysWork.mWindowList; child; child = child->mNext) {
                if (child->mParent == window) {
                    child->mParent = window->mParent;
                }
            }
        }
    }
    window = windowSearchID(windowGetActiveID());
    while (window) {
        if (window->mCloseRequested || window->mInactive) {
            for (child = winSysWork.mWindowList; child; child = child->mNext) {
                if (!child->mCloseRequested && !child->mInactive &&
                    _windowCheckParent(child, window->mParent)) {
                    winSysWork.mActiveID = child->mMenuID;
                    foundActiveWindow = 1;
                }
            }
            if (foundActiveWindow) {
                break;
            }
        } else {
            winSysWork.mActiveID = window->mMenuID;
            break;
        }
        window = window->mParent;
    }
    if (!window) {
        winSysWork.mActiveID = 0;
    }
    return 0;
}

void windowCloseMain(tagWINDOW_WORK *window)
{
    MenuData *menu = menuDataBiosGetPtr(window->mMenuID);
    if (menu->mControlFunction) {
        window->mState = WINDOW_STATE_DELETE;
        menu->mControlFunction(window);
    }
    _windowDeleteWork(window);
}

tagWINDOW_WORK *windowSearchID(s32 windowID)
{
    if (windowID <= 0) return NULL;
    tagWINDOW_WORK *window = winSysWork.mWindowList;
    while (window) {
        if (window->mMenuID == windowID) return window;
        window = window->mNext;
    }
    return NULL;
}

tagSPRITE_WORK *windowSearchItemID(tagWINDOW_WORK *window, u16 itemID)
{
    if (!window) return NULL;
    tagSPRITE_WORK *sprite = window->mSpriteWork;
    while (sprite) {
        if (sprite->mItemID == itemID) return sprite;
        sprite = sprite->mNext;
    }
    return NULL;
}

s32 windowGetActiveID(void)
{
    return winSysWork.mActiveID;
}

u8 windowCheckCursor(s32 windowID, u8 wait)
{
    while (true) {
        tagWINDOW_WORK *window = windowSearchID(windowID);
        if (!window) {
            GSlogWrite("%s(0x%08x)  ウィンドウが存在しない\n", __FUNCTION__, windowID);
            return 0;
        }
        if (window->mAccept) return 0;
        if (window->mCancel) return 0;
        if (!wait) break;
        if (!GSthreadGetCurrentThread()) {
            GSlogWrite("%s(0x%08x)  スレッドから呼ぶようにしてください\n", __FUNCTION__, windowID);
            break;
        }
        GSthreadSwitch();
    }
    return 1;
}

CURSOR windowGetCursor(s32 windowID)
{
    CURSOR cursor = {0, 0};
    tagWINDOW_WORK *window = windowSearchID(windowID);
    if (window) return window->mCursor;
    return cursor;
}

s32 windowGetValue(s32 windowID)
{
    s32 value;
    tagWINDOW_WORK *window = windowSearchID(windowID);
    if (window) {
        if (window->mCancel) value = WINDOW_RESULT_CANCELLED;
        else {
            MenuData *data = menuDataBiosGetPtr(window->mMenuID);
            if (data) {
                switch (data->mWindowValueMode) {
                case WINDOW_VALUE_NONE: value = 0; break;
                case WINDOW_VALUE_CURSOR: value = window->mCursor.x + window->mCursor.y; break;
                case WINDOW_VALUE_INDEX: value = window->mCursorIndex; break;
                case WINDOW_VALUE_ITEM: value = menuGetCursorItemID(windowID); break;
                }
            } else value = window->mCursor.x + window->mCursor.y;
        }
    } else value = WINDOW_RESULT_CANCELLED;
    return value;
}

tagMENU_ITEM_DATA *windowGetCursorToItem(tagWINDOW_WORK *window)
{
    MenuData *data = menuDataBiosGetPtr(window->mMenuID);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, data->mFirstItemID);
    s32 index = 0;
    while (true) {
        if (item->mShownFlag) {
            if (window->mCursor.y == index) return item;
            ++index;
        }
        if (item->mChainEndFlag) break;
        item = menuItemBiosGetPtr(window->mMenuID, item->mNextID);
    }
    return NULL;
}

void windowDrawSprite3(s16 x, s16 y, s16 width, s16 height, u32 color,
                       tagWINDOW_WORK *window, u16 spriteID, s16 textureOffsetX,
                       s16 textureOffsetY, s16 textureWidth, s16 textureHeight,
                       u32 flags, GSgfxLayerID layer)
{
    tagSPRITE_WORK spriteWork;
    tagWINDOW_WORK temporaryWindow;
    MenuSpriteData *data;
    MenuSpriteValue *value;
    tagSPRITE_WORK *sprite = &spriteWork;
    data = menuSpriteBiosGetPtr(spriteID);
    while (true) {
        value = menuSpriteBiosGetValPtr(data);
        memset(sprite, 0, sizeof(*sprite));
        sprite->mFlags = SPRITE_FLAGS_INITIAL;
        sprite->mColor = color;
        sprite->mScaleX = 1.0f;
        sprite->mScaleY = 1.0f;
        if (layer == floorGetMenuLayerID()) {
            sprite->mUseFloorMenuLayer = 1;
        }
        if (data->mValueType == MENU_SPRITE_VALUE_TEXTURE) {
            sprite->mTypeFlags |= SPRITE_TYPE_TEXTURE;
            sprite->mSpriteValue = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_PRIMITIVE) {
            sprite->mTypeFlags |= SPRITE_TYPE_PRIMITIVE;
            sprite->mDrawID = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_FLASH) {
            sprite->mTypeFlags |= SPRITE_TYPE_FLASH;
            sprite->mDrawID = value->mDrawValue;
        }
        sprite->mX = x + data->mOffsetX;
        sprite->mY = y + data->mOffsetY;
        sprite->mWidth = width;
        sprite->mHeight = height;
        sprite->mSpriteX = value->mX + textureOffsetX;
        sprite->mSpriteY = value->mY + textureOffsetY;
        sprite->mTextureWidth = textureWidth;
        sprite->mTextureHeight = textureHeight;
        sprite->mRgba.mAlpha = data->mAlpha * sprite->mRgba.mAlpha / MENU_ITEM_ALPHA_OPAQUE;
        if (flags & WINDOW_DRAW_FLIP_X) {
            sprite->mWidth = -sprite->mWidth;
        }
        if (flags & WINDOW_DRAW_FLIP_Y) {
            sprite->mHeight = -sprite->mHeight;
        }
        if (!window) {
            window = &temporaryWindow;
            memset(window, 0, sizeof(*window));
            spriteGetOffset(layer, &window->mScrollX, &window->mScrollY);
            window->mLayerID = layer;
            window->mColor = WINDOW_COLOR_WHITE;
        }
        winSpriteDraw(window, sprite);
        if (data->mChainEndFlag) {
            break;
        }
        data = menuSpriteBiosGetPtr(data->mNextID);
    }
}

void windowDrawSprite2(s16 x, s16 y, s16 width, s16 height, u32 color,
                       tagWINDOW_WORK *window, u16 spriteID, s32 flags,
                       GSgfxLayerID layer)
{
    tagSPRITE_WORK spriteWork;
    tagWINDOW_WORK temporaryWindow;
    MenuSpriteData *data;
    MenuSpriteValue *value;
    tagSPRITE_WORK *sprite = &spriteWork;
    data = menuSpriteBiosGetPtr(spriteID);
    while (true) {
        value = menuSpriteBiosGetValPtr(data);
        memset(sprite, 0, sizeof(*sprite));
        sprite->mFlags = SPRITE_FLAGS_INITIAL;
        sprite->mColor = color;
        sprite->mScaleX = 1.0f;
        sprite->mScaleY = 1.0f;
        if (layer == floorGetMenuLayerID()) {
            sprite->mUseFloorMenuLayer = 1;
        }
        if (data->mValueType == MENU_SPRITE_VALUE_TEXTURE) {
            sprite->mTypeFlags |= SPRITE_TYPE_TEXTURE;
            sprite->mSpriteValue = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_PRIMITIVE) {
            sprite->mTypeFlags |= SPRITE_TYPE_PRIMITIVE;
            sprite->mDrawID = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_FLASH) {
            sprite->mTypeFlags |= SPRITE_TYPE_FLASH;
            sprite->mDrawID = value->mDrawValue;
        }
        sprite->mX = x + data->mOffsetX;
        sprite->mY = y + data->mOffsetY;
        sprite->mWidth = width;
        sprite->mHeight = height;
        sprite->mSpriteX = value->mX;
        sprite->mSpriteY = value->mY;
        sprite->mTextureWidth = value->mWidth;
        sprite->mTextureHeight = value->mHeight;
        sprite->mRgba.mAlpha = data->mAlpha * sprite->mRgba.mAlpha / MENU_ITEM_ALPHA_OPAQUE;
        if ((u32)flags & WINDOW_DRAW_FLIP_X) {
            sprite->mWidth = -sprite->mWidth;
        }
        if ((u32)flags & WINDOW_DRAW_FLIP_Y) {
            sprite->mHeight = -sprite->mHeight;
        }
        if (!window) {
            window = &temporaryWindow;
            memset(window, 0, sizeof(*window));
            spriteGetOffset(layer, &window->mScrollX, &window->mScrollY);
            window->mLayerID = layer;
            window->mColor = WINDOW_COLOR_WHITE;
        }
        winSpriteDraw(window, sprite);
        if (data->mChainEndFlag) {
            break;
        }
        data = menuSpriteBiosGetPtr(data->mNextID);
    }
}

void windowDrawSpriteCursor(const s16 x, const s16 y, const u32 color, tagWINDOW_WORK *window,
                            const u16 spriteID, const u32 flags, const s32 layer, f32 *offset,
                            const u32 followingColor)
{
    tagSPRITE_WORK spriteWork;
    tagWINDOW_WORK temporaryWindow;
    tagSPRITE_WORK *sprite = &spriteWork;
    MenuSpriteData *data;
    MenuSpriteValue *value;
    s32 part = 0;
    data = menuSpriteBiosGetPtr(spriteID);
    while (true) {
        value = menuSpriteBiosGetValPtr(data);
        memset(sprite, 0, sizeof(*sprite));
        sprite->mFlags = SPRITE_FLAGS_INITIAL;
        if (part == 0) {
            sprite->mColor = color;
        } else {
            sprite->mColor = followingColor;
        }
        sprite->mScaleX = 1.0f;
        sprite->mScaleY = 1.0f;
        if (layer == floorGetMenuLayerID()) {
            sprite->mUseFloorMenuLayer = 1;
        }
        if (data->mValueType == MENU_SPRITE_VALUE_TEXTURE) {
            sprite->mTypeFlags |= SPRITE_TYPE_TEXTURE;
            sprite->mSpriteValue = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_PRIMITIVE) {
            sprite->mTypeFlags |= SPRITE_TYPE_PRIMITIVE;
            sprite->mDrawID = value->mDrawValue;
        } else if (data->mValueType == MENU_SPRITE_VALUE_FLASH) {
            sprite->mTypeFlags |= SPRITE_TYPE_FLASH;
            sprite->mDrawID = value->mDrawValue;
        }
        if (part == 0) {
            sprite->mX = x + data->mOffsetX;
            sprite->mY = y + data->mOffsetY;
        } else {
            sprite->mX = x + data->mOffsetX + (s32)offset[0];
            sprite->mY = y + data->mOffsetY + (s32)offset[1];
        }
        sprite->mWidth = value->mWidth;
        sprite->mHeight = value->mHeight;
        sprite->mSpriteX = value->mX;
        sprite->mSpriteY = value->mY;
        sprite->mTextureWidth = value->mWidth;
        sprite->mTextureHeight = value->mHeight;
        sprite->mRgba.mAlpha = data->mAlpha * sprite->mRgba.mAlpha / MENU_ITEM_ALPHA_OPAQUE;
        if (flags & WINDOW_DRAW_FLIP_X) {
            sprite->mWidth = -sprite->mWidth;
        }
        if (flags & WINDOW_DRAW_FLIP_Y) {
            sprite->mHeight = -sprite->mHeight;
        }
        if (!window) {
            window = &temporaryWindow;
            memset(window, 0, sizeof(*window));
            spriteGetOffset(layer, &window->mScrollX, &window->mScrollY);
            window->mLayerID = (GSgfxLayerID)layer;
            window->mColor = WINDOW_COLOR_WHITE;
        }
        winSpriteDraw(window, sprite);
        if (data->mChainEndFlag) {
            break;
        }
        ++part;
        data = menuSpriteBiosGetPtr(data->mNextID);
    }
}

void windowDrawSprite(s16 x, s16 y, tagWINDOW_WORK *window, u16 spriteID,
                      s32 flags, GSgfxLayerID layer)
{
    if (spriteID) {
        MenuSpriteValue *value = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(spriteID));
        windowDrawSprite2(x, y, value->mWidth, value->mHeight, WINDOW_COLOR_WHITE, window,
                          spriteID, flags, layer);
    }
}

s32 windowGetParam(tagWINDOW_WORK *window, s32 index)
{
    if (window) return window->mArgs[index];
    return 0;
}

void windowSetParam(tagWINDOW_WORK *window, s32 index, s32 value)
{
    if (window) window->mArgs[index] = value;
}

WINDOW_FREE_WORK *windowGetFreeWork(tagWINDOW_WORK *window)
{
    if (window) return &window->mFreeWork;
    return NULL;
}

void *windowAllocMemory(tagWINDOW_WORK *window, s32 size)
{
    if (!window) return NULL;
    _windowFreeAllocMemory(window);
    if (size <= 0) return NULL;
    window->mAllocMemory = GSmemAlloc(size);
    if (!window->mAllocMemory) return NULL;
    return _windowGetAllocMemory(window);
}

void *windowGetAllocPtr(tagWINDOW_WORK *window)
{
    if (window) return window->mAllocMemory;
    return NULL;
}
