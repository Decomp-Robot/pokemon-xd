#ifndef WIN_SPRITE_H
#define WIN_SPRITE_H

#include <types.h>
#include <cursorBios.h>
#include <GSgfxTypes.h>
#include <menuItemBios.h>

// Usable from C as well as C++.
typedef struct tagSPRITE_WORK tagSPRITE_WORK;
typedef struct tagWINDOW_WORK tagWINDOW_WORK;
typedef struct tagWINDOW_KEY tagWINDOW_KEY;
typedef struct NAME_ENTRY_ARG NAME_ENTRY_ARG;

enum WINDOW_STATE {
    WINDOW_STATE_INIT = 0,
    WINDOW_STATE_WAITING = 1,
    WINDOW_STATE_ACTIVE = 2,
    WINDOW_STATE_CLOSING = 3,
    WINDOW_STATE_DELETE = 5,
};

enum WindowSequenceState {
    WINDOW_SEQUENCE_OPEN = WINDOW_STATE_INIT,
    WINDOW_SEQUENCE_CLOSE = WINDOW_STATE_CLOSING
};

enum WindowStateFlags {
    WINDOW_STATE_FLAG_INUSE = 1,
    WINDOW_STATE_FLAG_DISP = 2,
    // Set in tagWINDOW_WORK::mStateFlags while the window is running: it keeps
    // the window's per-frame update and its message printer advancing.
    WINDOW_RUN_UPDATE = 4
};

// Sequence record the menu windows and the menu sprites play back: the
// record winSetSequence fills in, and the record the sequence mover advances.
typedef struct WINDOW_SEQUENCE {
    void *mData;        // sequence data the record plays back.
    u16 mSequenceID;    // id of the sequence, 0 clears the record.
    u8 _06[54];
} WINDOW_SEQUENCE;

typedef struct WINDOW_FREE_WORK WINDOW_FREE_WORK;
struct WINDOW_FREE_WORK {
    u32 mMessageID;
    u8 mKeyWait;
    u8 mSound;
    u8 mUnknown06[2];
    u32 mKeyTrigger;
    s16 mMessageWidth;
    s16 mMessageHeight;
};

// Per-window drawing and input state.
struct tagWINDOW_WORK {
    s8 mStateFlags;              // Window state flags.
    s8 mState;                   // Menu sequence step.
    s8 mFlag;                    // Set by the step's hook, cleared when the step advances.
    u8 _03;
    s32 mMenuID;                 // menu data id used for item lookups.
    u8 _08;
    s8 mOpenPriority;            // window ordering priority.
    u8 mCloseRequested;
    u8 mEnablePortMask;
    tagWINDOW_WORK *mParent;
    tagWINDOW_WORK *mNext;       // next window in the window list.
    tagWINDOW_WORK *mPrevious;
    u8 mInactive;
    u8 mShownItemCount;
    u8 mCursorMode;
    u8 mOpenFlags;
    GSgfxLayerID mLayerID;               // GSgfx layer the window draws on.
    u8 mItemTable;
    tagSPRITE_WORK *mSpriteWork; // head of the window sprite-work list.
    tagSPRITE_WORK *mCursorSpriteWork;
    WINDOW_SEQUENCE mSeq;       // Sequence record of the menu the window runs.
    union {
        u32 mArgs[8];
        NAME_ENTRY_ARG *mArg;        // argument the custom menu was opened with.
    };
    s32 mCursorIndex;
    s16 mScrollX;                // horizontal content offset.
    s16 mScrollY;
    union {
        u32 mColor;              // RGBA colour passed to the message printer.
        struct {
            u8 mColorR;
            u8 mColorG;
            u8 mColorB;
            u8 mColorA; // alpha channel; also read on its own.
        } mRgba;
    };
    f32 mScaleX;
    f32 mScaleY;
    CURSOR mCursor;              // cursor position in the window.
    CURSOR mPreviousCursor;
    u8 mAccept;                  // set when the menu is confirmed.
    u8 mCancel;                  // set when the menu is cancelled.
    u8 _A6[2];
    WINDOW_FREE_WORK mFreeWork;
    void *mAllocMemory;
};

// Flag bits of tagSPRITE_WORK::mFlags.
enum SpriteFlags {
    SPRITE_FLAG_INUSE = 1,
    SPRITE_FLAG_DISP = 2, // set while the node is drawn.
    SPRITE_FLAG_UNUSED = 4
};

enum SpriteTypeFlags {
    SPRITE_TYPE_TEXTURE = 0x1,
    SPRITE_TYPE_PRIMITIVE = 0x2,
    SPRITE_TYPE_HOOK = 0x8,
    SPRITE_TYPE_FLASH = 0x10
};

// Sprite work node used by menu entries.
struct tagSPRITE_WORK {
    tagSPRITE_WORK *mNext; // next node in the list.
    s8 mFlags;             // node flags, see SPRITE_FLAG_*.
    u8 mTypeFlags;
    s16 mItemID;           // item id the node displays for the menu.
    u32 mDrawID;
    WINDOW_SEQUENCE mSeq;  // sequence the node's row hook plays.
    MENU_ITEM_HOOK mHookFunction; // hook the node was registered with.
    u32 mMsgID;            // message id the node prints.
    s16 mX;                // x position the row hooks place the sprite at.
    s16 mY;                // y position the row hooks place the sprite at.
    s16 mWidth;            // width of the sprite.
    s16 mHeight;           // height of the sprite.
    u32 mSpriteValue;       // draw value selected by the sprite type.
    s16 mSpriteX;           // sprite texture x.
    s16 mSpriteY;           // sprite texture y.
    s16 mTextureWidth;
    s16 mTextureHeight;
    union {
        u32 mColor;            // whole colour word.
        struct {
            u8 mRed;
            u8 mGreen;
            u8 mBlue;
            u8 mAlpha; // alpha the sprite is drawn with.
        } mRgba;
    };
    f32 mScaleX;            // horizontal scale.
    f32 mScaleY;            // vertical scale.
    f32 mYaw;               // rotation about the Y axis, in radians.
    u8 mUseFloorMenuLayer;
    u8 mAlphaMode;
    u8 mAlignMode;
};

// Key mask bits held in tagWINDOW_KEY: direction pad, accept/cancel, X/Z,
// START and the L/R triggers.
enum WindowKeyMask {
    WINDOW_KEY_UP = 0x1,
    WINDOW_KEY_DOWN = 0x2,
    WINDOW_KEY_LEFT = 0x4,
    WINDOW_KEY_RIGHT = 0x8,
    WINDOW_KEY_ACCEPT = 0x10,
    WINDOW_KEY_CANCEL = 0x20,
    WINDOW_KEY_X = 0x40,
    WINDOW_KEY_Z = 0x100,
    WINDOW_KEY_TRIGGER_L = 0x200,
    WINDOW_KEY_TRIGGER_R = 0x400,
    WINDOW_KEY_START = 0x800,
    WINDOW_KEY_DIRECTION_VERTICAL = WINDOW_KEY_UP | WINDOW_KEY_DOWN,
    WINDOW_KEY_DIRECTION_HORIZONTAL = WINDOW_KEY_LEFT | WINDOW_KEY_RIGHT,
    WINDOW_KEY_DIRECTION = WINDOW_KEY_DIRECTION_VERTICAL | WINDOW_KEY_DIRECTION_HORIZONTAL,
};

// Key state read by windowGetKeyOn/windowGetKeyTrg/windowGetKeyRepeat and by the debug menus.
struct tagWINDOW_KEY {
    u16 mKeyOn;     // keys held this frame.
    u16 mKeyOnPrev;
    u16 mKeyTrg;    // keys pressed this frame.
    u16 mKeyRepeat; // repeating keys.
    u16 mKeyRepeatActive;
    s8 mKeyRepeatTimer[16];
};

#ifdef __cplusplus
extern "C" {
#endif
struct MENU_ITEM_dd_* windowGetCursorToItem(tagWINDOW_WORK* window);
void winSpriteDrawTexture(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void winSpriteDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void winAnimeCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void winSetSequence(WINDOW_SEQUENCE *sequence, u16 sequenceID);

// Key state of the window, also read directly by the debug menus.
struct tagWINDOW_KEY *windowGetKeyInfo(void);
struct tagWINDOW_KEY *windowGetKeyInfo2(void);
tagWINDOW_WORK *windowSearchID(s32 windowID);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
#ifdef __cplusplus
u8 winSpriteGetDisp(tagSPRITE_WORK* sprite);
void winSpriteRelease(tagSPRITE_WORK **list);
tagSPRITE_WORK *winSpriteAdd(tagSPRITE_WORK **list);
void winSpriteInit(void);
void winSpriteSetDisp(tagSPRITE_WORK* sprite, bool visible);
#endif

void spriteClearScissor(s32 layer);
void spriteSetOffset(s32 layer, s16 x, s16 y);
void spriteSetEnv(s32 layer);
void winSeqSetMenu(long menuID, s32 seq);

u8 winSeqCheckMove(s32 menuID);
void windowDrawSpriteCursor(s16 x, s16 y, u32 color, tagWINDOW_WORK *window, u16 itemID, u32 flags,
                            s32 layer, f32 *values, u32 animation);

// Key state of the window the debug menus read directly.
struct tagWINDOW_KEY *windowGetKeyInfo();

#ifdef __cplusplus
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
#endif
tagWINDOW_WORK *windowSearchID(s32 id);

void winSpriteDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
#ifdef __cplusplus
}
#endif

#endif
