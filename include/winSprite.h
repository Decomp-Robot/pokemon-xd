#ifndef WIN_SPRITE_H
#define WIN_SPRITE_H

#include <types.h>
#include <cursorBios.h>
#include <GSgfxTypes.h>

// Usable from C as well as C++.
typedef struct tagSPRITE_WORK tagSPRITE_WORK;
typedef struct tagWINDOW_WORK tagWINDOW_WORK;
typedef struct tagWINDOW_KEY tagWINDOW_KEY;
typedef struct NAME_ENTRY_ARG NAME_ENTRY_ARG;

enum WINDOW_STATE {
    WINDOW_STATE_INIT = 0,
    WINDOW_STATE_ACTIVE = 2,
    WINDOW_STATE_CLOSING = 3,
};

enum WindowSequenceState {
    WINDOW_SEQUENCE_OPEN = WINDOW_STATE_INIT,
    WINDOW_SEQUENCE_CLOSE = WINDOW_STATE_CLOSING
};

enum {
    // Set in tagWINDOW_WORK::mStateFlags while the window is running: it keeps
    // the window's per-frame update and its message printer advancing.
    WINDOW_RUN_UPDATE = 4
};

// Per-window drawing and input state.
struct tagWINDOW_WORK {
    s8 mStateFlags;              // Window state flags.
    s8 mState;                   // Menu sequence step.
    s8 mFlag;                    // Set by the step's hook, cleared when the step advances.
    u8 _03;
    long mMenuID;                // menu data id used for item lookups.
    u8 _08;
    u8 mOpenPriority;            // window ordering priority.
    u8 _0A[6];
    tagWINDOW_WORK *mNext;       // next window in the window list.
    u8 _14[8];
    GSgfxLayerID mLayerID;               // GSgfx layer the window draws on.
    u8 _20[4];
    tagSPRITE_WORK *mSpriteWork; // head of the window sprite-work list.
    u8 _28[4];
    u8 mSeq[60];                // Sequence record of the menu the window runs.
    union {
        u32 mArgs[8];
        NAME_ENTRY_ARG *mArg;        // argument the custom menu was opened with.
    };
    long mCursorIndex;
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
    u8 _94[8];
    CURSOR mCursor;              // cursor position in the window.
    u8 _A0[2];
    s16 cursorTo;                // cursor the window is scrolled to.
    u8 mAccept;                  // set when the menu is confirmed.
    u8 mCancel;                  // set when the menu is cancelled.
};

// Sprite work node used by menu entries.
struct tagSPRITE_WORK {
    tagSPRITE_WORK *mNext; // next node in the list.
    u8 _04;
    u8 _05;
    s16 mItemID;           // item id the node displays for the menu.
    u8 _08[64];
    void *mHookFunction;   // hook the node was registered with.
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
    u8 _70[4];
    u8 mUseFloorMenuLayer;
};

// Key state read by windowGetKeyOn/windowGetKeyTrg/windowGetKeyRepeat and by the debug menus.
struct tagWINDOW_KEY {
    u16 mKeyOn;     // keys held this frame.
    u16 mKeyState;
    u16 mKeyTrg;    // keys pressed this frame.
    u16 mKeyRepeat; // repeating keys.
    u8 _08[18];
};

#ifdef __cplusplus
extern "C" {
#endif
struct MENU_ITEM_dd_* windowGetCursorToItem(tagWINDOW_WORK* window);
void winSpriteDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

// Key state of the window, also read directly by the debug menus.
struct tagWINDOW_KEY *windowGetKeyInfo(void);
struct tagWINDOW_KEY *windowGetKeyInfo2(void);
tagWINDOW_WORK *windowSearchID(s32 windowID);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
#ifdef __cplusplus
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
