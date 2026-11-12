#include <types.h>
#include <winSprite.h>
#include <menuCBBios.h>
#include <menuCBCommon.h>
#include <menuCB_Sub1.h>
#include <window.h>
#include <menuSpriteBios.h>

struct FIELD_ANIME_PANEL {
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    u32 mColor;
    u16 mSpriteID;
    s16 mSpriteX;
    s16 mSpriteY;
    u32 mFlags;
};

// Slide work of the tunaide disp callbacks: the panel the field moved to, and
// the one it is pushing out of the window.
struct FIELD_ANIME_WORK {
    FIELD_ANIME_PANEL mPanel;
    FIELD_ANIME_PANEL mOutgoing;
};

FIELD_ANIME_WORK field_anime;

extern const u32 tunaide02_face_sprite[7] = {
    513, 514, 515, 516, 517, 518, 519
};

extern const u32 tunaide_gc_entry_sprite[8] = {
    805, 806, 807, 808, 809, 810, 811, 812
};

extern const u32 field_sprite[7] = {
    679, 681, 680, 678, 677, 676, 727
};

enum FieldSlideState {
    FIELD_SLIDE_IDLE = 0,
    FIELD_SLIDE_FROM_TOP_START = 1,
    FIELD_SLIDE_FROM_TOP = 2,
    FIELD_SLIDE_FROM_BOTTOM_START = 3,
    FIELD_SLIDE_FROM_BOTTOM = 4
};

enum {
    TUNAIDE_RULE_SELECT_LAST = 1
};

enum {
    TUNAIDE_FIELD_FIRST = 0,
    TUNAIDE_FIELD_LAST = 6
};

enum {
    TUNAIDE_FIELD_ITEM = 675,
    TUNAIDE_FIELD_OUTGOING_ITEM = 728,
    TUNAIDE_FIELD_INCOMING_ITEM = 729,
    TUNAIDE_FIELD_NEXT_ARROW_ITEM = 673,
    TUNAIDE_FIELD_PREVIOUS_ARROW_ITEM = 674,
    TUNAIDE_CURSOR_ITEM_FIRST = 1005,
    TUNAIDE_CURSOR_ITEM_SECOND = 1006,
    TUNAIDE_CURSOR_ITEM_THIRD = 1007
};

extern "C" {
void menuCB_Tunaide01_3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
}

// Disp callback of the tunaide carousel. The window shows one entry of
// the field sprite table; when the selection moves, the two panels are slid past each
// other by _CB.tunaideSlide before the new entry is shown.
void menuCB_Tunaide01_3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case TUNAIDE_FIELD_ITEM:
        if (_CB.tunaideSlide == FIELD_SLIDE_IDLE) {
            menuCBSpriteSetData(
                sprite, menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
            break;
        }
        winSpriteSetDisp(sprite, false);
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            switch (_CB.tunaideSlide) {
            case FIELD_SLIDE_FROM_TOP_START:
                field_anime.mPanel.mX = 0;
                field_anime.mPanel.mY = 0;
                field_anime.mPanel.mWidth = 278;
                field_anime.mPanel.mHeight = 0;
                field_anime.mPanel.mColor = 0xFFFFFFFF;
                field_anime.mPanel.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelect];
                field_anime.mPanel.mSpriteX = 0;
                field_anime.mPanel.mSpriteY = 167;
                field_anime.mPanel.mFlags = 0;
                field_anime.mOutgoing.mX = 0;
                field_anime.mOutgoing.mY = 0;
                field_anime.mOutgoing.mWidth = 278;
                field_anime.mOutgoing.mHeight = 167;
                field_anime.mOutgoing.mColor = 0xFFFFFFFF;
                field_anime.mOutgoing.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelectPrev];
                field_anime.mOutgoing.mSpriteX = 0;
                field_anime.mOutgoing.mSpriteY = 0;
                field_anime.mOutgoing.mFlags = 0;
                _CB.tunaideSlide = FIELD_SLIDE_FROM_TOP;
                break;
            case FIELD_SLIDE_FROM_BOTTOM_START:
                field_anime.mPanel.mX = 0;
                field_anime.mPanel.mY = 167;
                field_anime.mPanel.mWidth = 278;
                field_anime.mPanel.mHeight = 0;
                field_anime.mPanel.mColor = 0xFFFFFFFF;
                field_anime.mPanel.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelect];
                field_anime.mPanel.mSpriteX = 0;
                field_anime.mPanel.mSpriteY = 0;
                field_anime.mPanel.mFlags = 0;
                field_anime.mOutgoing.mX = 0;
                field_anime.mOutgoing.mY = 0;
                field_anime.mOutgoing.mWidth = 278;
                field_anime.mOutgoing.mHeight = 167;
                field_anime.mOutgoing.mColor = 0xFFFFFFFF;
                field_anime.mOutgoing.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelectPrev];
                field_anime.mOutgoing.mSpriteX = 0;
                field_anime.mOutgoing.mSpriteY = 0;
                field_anime.mOutgoing.mFlags = 0;
                _CB.tunaideSlide = FIELD_SLIDE_FROM_BOTTOM;
                break;
            case FIELD_SLIDE_FROM_TOP:
                field_anime.mPanel.mHeight += 15;
                if (field_anime.mPanel.mHeight >= 167) {
                    field_anime.mPanel.mHeight = 0;
                    _CB.tunaideSlide = FIELD_SLIDE_IDLE;
                    winSpriteSetDisp(sprite, true);
                    menuCBSpriteSetData(
                        sprite,
                        menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
                }
                {
                    FIELD_ANIME_PANEL *panel = &field_anime.mPanel;
                    panel->mSpriteY = 167 - panel->mHeight;
                    field_anime.mOutgoing.mY = panel->mHeight;
                    field_anime.mOutgoing.mHeight = 167 - panel->mHeight;
                }
                break;
            case FIELD_SLIDE_FROM_BOTTOM:
                field_anime.mPanel.mHeight += 15;
                if (field_anime.mPanel.mHeight >= 167) {
                    field_anime.mPanel.mHeight = 0;
                    _CB.tunaideSlide = FIELD_SLIDE_IDLE;
                    winSpriteSetDisp(sprite, true);
                    menuCBSpriteSetData(
                        sprite,
                        menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
                }
                {
                    FIELD_ANIME_PANEL *panel = &field_anime.mPanel;
                    panel->mY = 167 - panel->mHeight;
                    field_anime.mOutgoing.mSpriteY = panel->mHeight;
                    field_anime.mOutgoing.mHeight = 167 - panel->mHeight;
                }
                break;
            }
        }
        break;
    case TUNAIDE_FIELD_INCOMING_ITEM:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            windowDrawSprite3(field_anime.mPanel.mX, field_anime.mPanel.mY,
                        field_anime.mPanel.mWidth, field_anime.mPanel.mHeight,
                        field_anime.mPanel.mColor, window,
                        field_anime.mPanel.mSpriteID,
                        field_anime.mPanel.mSpriteX,
                        field_anime.mPanel.mSpriteY,
                        field_anime.mPanel.mWidth,
                        field_anime.mPanel.mHeight,
                        field_anime.mPanel.mFlags, window->mLayerID);
        }
        break;
    case TUNAIDE_FIELD_OUTGOING_ITEM:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            windowDrawSprite3(field_anime.mOutgoing.mX,
                        field_anime.mOutgoing.mY,
                        field_anime.mOutgoing.mWidth,
                        field_anime.mOutgoing.mHeight,
                        field_anime.mOutgoing.mColor, window,
                        field_anime.mOutgoing.mSpriteID,
                        field_anime.mOutgoing.mSpriteX,
                        field_anime.mOutgoing.mSpriteY,
                        field_anime.mOutgoing.mWidth,
                        field_anime.mOutgoing.mHeight,
                        field_anime.mOutgoing.mFlags, window->mLayerID);
        }
        break;
    case TUNAIDE_FIELD_NEXT_ARROW_ITEM:
        if (_CB.mRuleSelect != TUNAIDE_RULE_SELECT_LAST) {
            sprite->mRgba.mAlpha = 127;
        }
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.tunaideSelect != TUNAIDE_FIELD_LAST);
        break;
    case TUNAIDE_FIELD_PREVIOUS_ARROW_ITEM:
        if (_CB.mRuleSelect != TUNAIDE_RULE_SELECT_LAST) {
            sprite->mRgba.mAlpha = 127;
        }
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.tunaideSelect != TUNAIDE_FIELD_FIRST);
        break;
    case TUNAIDE_CURSOR_ITEM_FIRST:
    case TUNAIDE_CURSOR_ITEM_SECOND:
    case TUNAIDE_CURSOR_ITEM_THIRD:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.mRuleSelect == TUNAIDE_RULE_SELECT_LAST);
        }
        break;
    }
}
