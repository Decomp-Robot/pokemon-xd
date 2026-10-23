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

// Per-window drawing and input state.
struct tagWINDOW_WORK {
    s8 _00;                      // 0x00: unknown.
    s8 mState;                   // 0x01: menu sequence step the window is in.
    s8 mFlag;                    // 0x02: set by the step's hook, cleared when the step advances.
    u8 _03;                      // 0x03: unknown.
    long mMenuID;                // 0x04: menu data id used for item lookups.
    u8 _08[0x08];                // 0x08-0x0F: unknown.
    tagWINDOW_WORK *mNext;       // 0x10: next window in the window list.
    u8 _14[0x08];                // 0x14-0x1B: unknown.
    GSgfxLayerID mLayerID;               // 0x1C: GSgfx layer the window draws on.
    u8 _20[0x04];                // 0x20-0x23: unknown.
    tagSPRITE_WORK *mSpriteWork; // 0x24: head of the window sprite-work list.
    u8 _28[0x04];                // 0x28-0x2B: head of a second work list.
    u8 mSeq[0x3C];               // 0x2C: sequence record of the menu the window runs.
    union {
        u32 mArgs[8];
        NAME_ENTRY_ARG *mArg;        // 0x68: argument the custom menu was opened with.
    };
    long mCursorIndex;
    s16 mScrollX;                // 0x8C: horizontal content offset.
    s16 mScrollY;
    union {
        u32 mColor;              // 0x90: RGBA colour passed to the message printer.
        struct {
            u8 mColorR;          // 0x90
            u8 mColorG;          // 0x91
            u8 mColorB;          // 0x92
            u8 mColorA;          // 0x93: alpha channel; also read on its own.
        } mRgba;
    };
    u8 _94[0x08];                // 0x94-0x9B: unknown.
    CURSOR mCursor;              // 0x9C: cursor position in the window.
    u8 _A0[0x02];                // 0xA0-0xA1: unknown.
    s16 cursorTo;                // 0xA2: cursor the window is scrolled to.
    u8 mAccept;                  // 0xA4: set when the menu is confirmed.
    u8 mCancel;                  // 0xA5: set when the menu is cancelled.
};

// Sprite work node (0x78 bytes). winSpriteAdd hands these out of a fixed pool
// and chains them through +0x00.
struct tagSPRITE_WORK {
    tagSPRITE_WORK *mNext; // 0x00: next node in the list.
    u8 _04;
    u8 _05;          // 0x04-0x05: unknown.
    s16 mItemID;           // 0x06: item id the node displays for the menu.
    u8 _08[0x40];          // 0x08-0x47: unknown.
    void *mHookFunction;   // 0x48: hook the node was registered with.
    u32 mMsgID;            // 0x4C: message id the node prints.
    s16 mX;                // 0x50: x position the row hooks place the sprite at.
    s16 mY;                // 0x52: y position the row hooks place the sprite at.
    s16 mWidth;            // 0x54: width of the sprite.
    s16 mHeight;           // 0x56: height of the sprite.
    u32 mSpriteValue;       // 0x58: draw value selected by the sprite type.
    s16 mSpriteX;           // 0x5C: sprite texture x.
    s16 mSpriteY;           // 0x5E: sprite texture y.
    s16 mTextureWidth;
    s16 mTextureHeight;
    /* 0x64-0x67: the sprite colour word. The title cursor hook clears the
     * whole word to -1 to drop whatever blend override is in place; the entry
     * menus set the channels apart on it, and everything else writes only the
     * alpha byte at the top of it. */
    union {
        u32 mColor;            // 0x64: whole colour word.
        struct {
            u8 mRed;           // 0x64
            u8 mGreen;         // 0x65
            u8 mBlue;          // 0x66
            u8 mAlpha;         // 0x67: alpha the sprite is drawn with.
        } mRgba;
    };
    f32 mScaleX;            // 0x68: horizontal scale.
    f32 mScaleY;            // 0x6C: vertical scale.
    u8 _70[4];             // 0x70-0x73: unknown.
    u8 mUnk74;             // 0x74: flag read by winSpriteGetLayerID.
};

// Key state read by windowGetKeyOn/windowGetKeyTrg/windowGetKeyRepeat
// (menu.a window.o) and directly by the debug menus.
struct tagWINDOW_KEY {
    u16 mKeyOn;     // 0x00: keys held this frame.
    u16 mKeyState;  // 0x02
    u16 mKeyTrg;    // 0x04: keys pressed this frame.
    u16 mKeyRepeat; // 0x06: repeating keys.
    u8 _08[0x12];
};

#ifdef __cplusplus
extern "C" {
#endif
struct MENU_ITEM_dd_* windowGetCursorToItem(tagWINDOW_WORK* window);
void winSpriteDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

// Key state of the window the debug menus read directly (menu.a window.o).
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

// Key state of the window the debug menus read directly (menu.a window.o).
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
