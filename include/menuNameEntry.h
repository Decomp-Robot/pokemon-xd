#ifndef MENU_NAME_ENTRY_H
#define MENU_NAME_ENTRY_H

#include <GSgfx.h>
#include <menuItemBios.h>
#include <menuModel.h>
#include <types.h>
#include <winSprite.h>

// One entry of the name-entry mode table (rodata 0x802EA910, 7 entries).
struct NameEntryModeData {
    unsigned long mMessageID;           // 0x00: title message id.
    s32 mMaxLength;                     // 0x04: character limit of the name.
    const unsigned long *mDefaultNames; // 0x08: optional default name ids.
    s32 mDefaultNameCount;              // 0x0C: number of default names.
};

// One 50-on character table (rodata 0x802EA980, 2 entries). Each row holds
// the message id of the character the row draws.
struct NameEntryTableData {
    unsigned long mPacked; // 0x00
    unsigned long mTitle;  // 0x04: title message id of the table.
    unsigned long mRow[8]; // 0x08: row message ids.
    unsigned long mUnk28;  // 0x28: unknown.
};

// Argument block the name-entry menus read through tagWINDOW_WORK+0x68. The
// head at 0x00 is the initial name for the input menu (0x66) and the default
// name id table for the select menu (0x65); the buffer at 0x18 is the
// character string being edited; the fields from 0x24 on point at the .sbss
// editing values.
struct NAME_ENTRY_ARG {
    union {
        u16 mInitialName[12]; // 0x00: initial name (menu 0x66).
        struct {
            const unsigned long *mNameIDs; // 0x00: default name ids (menu 0x65).
            s32 mNameIDCount;              // 0x04: number of default names.
            u8 _08[0x10];                  // 0x08-0x17: unknown.
        } mSelect;
    };
    u16 *mChars;  // 0x18: character buffer being edited.
    s32 mMode;    // 0x1C: name-entry mode (mode table index).
    s32 mParam;   // 0x20: party slot or Pokemon pointer, by mode.
    s32 *mTable;  // 0x24: selected 50-on table (0 or 1).
    s32 *mColumn; // 0x28: selected column.
    s32 *mRow;    // 0x2C: selected row.
    f32 *mTimer;  // 0x30: cursor blink timer.
    s32 *mCursor; // 0x34: character position in the buffer.
    s32 *mUnk38;  // 0x38: five more pointers to shared editing values; the
    s32 *mUnk3C;  // 0x3C: window code uses them, their semantics are not
    s32 *mUnk40;  // 0x40: recovered (see nameEntrySharedVal0-4).
    s32 *mUnk44;  // 0x44
    s32 *mUnk48;  // 0x48
};

// The name the screen is editing, kept across the menu (bss 0x80429794).
struct NAME_ENTRY_WORK {
    u16 mName[12]; // 0x00: the name string.
    s32 mType;     // 0x18: name-entry type (mode table index).
    s32 mParam;    // 0x1C: party slot or Pokemon pointer, by type.
    s32 mChanged;  // 0x20: nonzero when the name was changed this time.
    s32 mFade;     // 0x24: nonzero when the floor fade script is needed.
    s32 mBlock;    // 0x28: nonzero when the screen owns the floor.
    u8 _2C[0x08];  // 0x2C-0x33: unknown.
};

// One floating "ball" of the name-entry background (bss 0x80429270, 30 of
// them, 0x24 bytes each).
struct BALL_WORK {
    s32 mActive;   // 0x00: nonzero while the ball is alive.
    f32 mX;        // 0x04: position.
    f32 mY;        // 0x08: position.
    f32 mScale;    // 0x0C: current scale.
    f32 mScaleMax; // 0x10: scale reached at the end of the life.
    s32 mFlags;    // 0x14: layer flags OR'ed with the alpha byte.
    u8 mAlpha;     // 0x18: alpha, and the second half of the layer flags.
    u8 _19[0x03];
    f32 mTime;     // 0x1C: elapsed life.
    f32 mTimeMax;  // 0x20: total life.
};

// Name-entry menu data ids.
enum {
    NAME_ENTRY_MENU_ID_SELECT = 0x65,
    NAME_ENTRY_MENU_ID_INPUT = 0x66,
    NAME_ENTRY_MENU_ID_MAIN = 0x68,
    NAME_ENTRY_FLOOR_ID = 0x390,
};

// The kana table list the editor steps through. Slot 0 holds no characters;
// the editor moves a character through the kana table itself.
enum {
    NAME_ENTRY_TABLE_KANA = 1,
    NAME_ENTRY_TABLE_COUNT = 2,
};

// Mode table and 50-on tables the screen shares with the window code.
extern "C" {
extern const NameEntryModeData nameEntryModeData[7];
extern const NameEntryTableData nameEntry50onData[2];
extern const unsigned long DefKenName[3];
extern const unsigned long DefHizukiName[3];
}

extern "C" {

int menuNameEntryCall(s32 type, s32 param);
int menuNameEntryOpen(s32 type, s32 param);
int floorLink(s32 type, s32 param);
void menuNameEntryGetLastName(u16 *name);
void menuNameEntry(void);
int menuNameEntryBackCtrl(tagWINDOW_WORK *window);
int menuNameEntryCtrl(tagWINDOW_WORK *window);
int menuNameEntryBackDrawHumanModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryBackDrawPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void zz_0032a94_(long dataID, MENU_MODEL *model, s32 type, s32 param);
int menuNameEntryDrawBkCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawBCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDraw50Text(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawNameRuleText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawKenNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawNameBoxText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawModeNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawEndCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDraw50onToggleTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntrySelectButton(tagWINDOW_WORK *window);
int menuNameEntrySelectCtrl(tagWINDOW_WORK *window);
int menuNameEntrySelectDrawSelText1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntrySelectDrawSelText2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntrySelectDrawSelText3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int zz_0032544_(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDispItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuNameEntryDrawNameBox(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
int menuNameEntryDrawNameBoxCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 count, s32 mode);
void menuNameEntryDispButtonCur(NAME_ENTRY_ARG *arg, tagSPRITE_WORK *sprite);
int menuNameEntryDrawCellCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u16 get50onChar(s32 table, s32 column, s32 row);
void print1Char(GSgfxLayerID layer, s32 x, s32 y, u32 color, u16 chr);
u16 conv50onChar(u16 chr, s32 table);
int confirmName(u16 *name);
s32 getCharClass(u16 chr);
void toggle50onTable(NAME_ENTRY_ARG *arg);
int input50onCell(NAME_ENTRY_ARG *arg);
int menuNameEntryInputButton(tagWINDOW_WORK *window);
void menuNameEntryInputDummy(void);

} // extern "C"

int menuNameEntryDrawNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 count, s32 mode);
void drawTexture(GStexture *texture, tagSPRITE_WORK *sprite, GSgfxLayerID layer);
int getGScharLen(u16 *string);
int chkValidName(u16 *name);
u8 backNameCursor(NAME_ENTRY_ARG *arg);
int inputName(u16 *name, u16 *initial, s32 type, s32 param, s32 skip);
int selectName(u16 *name, const unsigned long *nameIDs, s32 param);
void setDefNameText(tagSPRITE_WORK *sprite, unsigned long stringID);
void ballMain1(BALL_WORK *ball);
void generateBall(void);
void ballMain(void);
int setName(u16 *name, s32 type, s32 param);
int getName(u16 *name, s32 type, s32 param);
int chkParam(s32 type, s32 param);
int pushNameEntryFloor(s32 type, s32 param, s32 fade);

#endif
