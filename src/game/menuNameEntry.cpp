#include <GSgfx.h>
#include <menuNameEntry.h>
#include <winSprite.h>

struct Pokemon;

extern "C" {
void winSpriteSetDisp(tagSPRITE_WORK *sprite, s32 on);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuSubDrawCursorBackAnimeOnOffAuto(tagWINDOW_WORK *window, GSgfxLayerID layer, s32 p1, s32 p2, s16 p3, s16 p4, u8 p5, s32 p6);
GSgfxLayer *GSgfxLayerFindByID(GSgfxLayerID id);
GSgfxVF *GSgfxVFBuildTemp(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void GSgfxRenderDrawEnd(GSgfxLayer *layer);
u16 *GSmsgGetStringPtr(s32 id);
void msgctrlSetValue(s32 id, unsigned long value);
void GSmsgPrint2(GSgfxLayerID layer, s32 x, s32 y, u32 color, s32 msgID);
Pokemon *heroGetStatus(s32 p1, s32 p2, u16 slot);
void heroSetStatus(s32 p1, unsigned long p2, const u16 *name);
Pokemon *fightFloorGetDefPokemonPtr(s32 p1);
u8 pokemonGetSex(Pokemon *pokemon);
u8 pokemonCheckValid(Pokemon *pokemon);
u16 battlePokemonGetSpecies(Pokemon *pokemon);
s32 pokemon_SetNickname(Pokemon *pokemon, const u16 *name);
u16 *pokemon_GetNickname(Pokemon *pokemon);
u16 *pcboxGetPokemonName(s32 box, s8 slot);
void pcboxSetPokemonBoxName(s32 box, s8 slot, const u16 *name);
void dbgMenuSetOpenFlag(s32 flag);
s32 GSmsgGetLength(s32 id);
unsigned long GSmsgGetMessageSize(s32 id);
void windowDrawTextureId(s16 x, s16 y, tagWINDOW_WORK *window, s32 id, s32 unk, GSgfxLayerID layer);
void GSsndPlayNow(s32 id);
void winMsgOpen(s32 p1, s32 msgID, s32 p2, s32 p3);
s32 menuSubOpenYesNo(s32 mode, s32 x, s32 y, s32 flag);
void winMsgClose(s32 p1);
void GScharCpy(u16 *dst, const u16 *src);
s32 GScharCmp(const u16 *a, const u16 *b);
u32 windowGetKeyRepeat(void);
u32 windowGetKeyTrg(void);
long windowGetActiveID(void);
long menuOpenCustom(s32 menuID, long windowID, ...);
void menuClose(s32 menuID);
void menuCloseSync(s32 menuID, s32 flag);
void winSeqSetMenu(long menuID, s32 seq);
void menuModelSetFlag(MENU_MODEL *model, s32 flag);
void menuModelAdjustCamera(MENU_MODEL *model);
void menuModelSetSize(MENU_MODEL *model, s16 x0, s16 y0, s16 x1, s16 y1);
void menuModelSetAngle(MENU_MODEL *model, const f32 *angle);
void menuModelSetZoom(MENU_MODEL *model, f32 zoom);
void menuModelSetOffset(MENU_MODEL *model, const f32 *offset);
GStexture *menuModelRender(MENU_MODEL *model);
void menuModelInit(MENU_MODEL *model, s32 p1, s32 p2);
void menuModelLoad(MENU_MODEL *model, unsigned long id);
void menuModelSetMotion(MENU_MODEL *model, s32 motion);
s32 menuModelCheck(MENU_MODEL *model, s32 flag);
void menuModelLoadPokemon(MENU_MODEL *model, Pokemon *pokemon);
void menuModelTerm(MENU_MODEL *model);
void *peopleInfoBiosGetPtr(unsigned long id);
void peopleInfoBiosGetMotion(void *info, s32 p1, void *p2, void *p3);
void FlashSystemSetPlayFrameLabel(unsigned long p1, const char *start, const char *end);
void FlashSystemSetLoopFlag(unsigned long p1, s32 flag);
void FlashSystemSetNowFrameLabel(unsigned long p1, const char *label);
void fadeSet(s32 p1, f32 p2);
void fadeCheck(s32 p1);
void floorSetFadeScript(s32 p1, s32 script);
void menuTitleBattleSelectFree(void);
void floorPop(void);
void floorPush(s32 floor);
s32 floorWaitForChange(void);
f32 HSD_Randf(void);
void *textureBiosGetDataTable(s32 id);
void *textureBiosGetLanguageEntry(void *table);
void textureBiosLoadSpriteId(s32 x, s32 y, s32 w, s32 h, u32 flags, tagWINDOW_WORK *window, s32 tex, s32 mode, GSgfxLayerID layer);
u8 menuIsOpen(s32 id);
void menuSetDisp(s32 id, s32 on);
double sin(double x);
}

// Default names offered for the hero.
extern "C" const unsigned long DefKenName[3] = { 0x3B20, 0x3B21, 0x3B22 };
extern "C" const unsigned long DefHizukiName[3] = { 0x3B20, 0x3B21, 0x3B22 };

// Per-mode title, character limit and default names.
extern "C" const NameEntryModeData nameEntryModeData[7] = {
    { 0x3B11, 7, DefKenName, 3 },
    { 0x3B11, 7, DefHizukiName, 3 },
    { 0x3B12, 10, 0, 0 },
    { 0x3B13, 8, 0, 0 },
    { 0x3B12, 10, 0, 0 },
    { 0x3B12, 10, 0, 0 },
    { 0x3C33, 10, 0, 0 },
};

// The two 50-on character tables.
extern "C" const NameEntryTableData nameEntry50onData[2] = {
    { 0x0B570200, 0x3B14, { 0x3AFD, 0x3AFE, 0x3AFF, 0x3B00, 0x3BF4, 0x3BF5, 0, 0 }, 0 },
    { 0x023C5700, 0x3B15, { 0x3B01, 0x3B02, 0x3B03, 0x3B04, 0x3BF6, 0x3BF7, 0, 0 }, 0 },
};

// Menus hidden while the name-entry floor is pushed.
extern "C" const unsigned long nameEntryHideMenuIDs[4] = { 0x37, 0x40, 0x38, 0x41 };

// Model ids selected by the name-entry type.
extern "C" const unsigned long nameEntryModelIDs[4] = { 0x16C90400, 0x16C70400, 0, 0x10560400 };

// Background balls.
extern "C" {
BALL_WORK nameEntryBalls[30];

// Model work the background renders.
MENU_MODEL nameEntryModel;

// The name being edited.
NAME_ENTRY_WORK nameEntryWork;
}

// Kana tables the editor steps through; slot 0 is "none".
extern "C" const unsigned long nameEntryKanaTables[2] = { 0, 0x3B1B };

// Position and size the background models are drawn with.
struct ModelExtent {
    s16 mX;
    s16 mY;
};

extern "C" const ModelExtent cPokemonModelPos = { 0, 120 };
extern "C" const ModelExtent cPokemonModelSize = { 300, 300 };
extern "C" const ModelExtent cHumanModelPos = { 0, 80 };
extern "C" const ModelExtent cHumanModelSize = { 520, 300 };

// Editing fields the screen shares with the window code through
// NAME_ENTRY_ARG; inputName binds them to the arg pointers.
extern "C" {
s32 nameEntryTable;
s32 nameEntryColumn;
s32 nameEntryRow;
f32 nameEntryTimer;
s32 nameEntryCursor;
// Purpose not recovered: only the window code (unrecovered) uses these five.
s32 nameEntrySharedVal0;
s32 nameEntrySharedVal1;
s32 nameEntrySharedVal2;
s32 nameEntrySharedVal3;
s32 nameEntrySharedVal4;
}

// Draw the background cursor of the name-entry screen.
int menuNameEntryDrawBkCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s16 cursor;

    winSpriteSetDisp(sprite, 0);
    cursor = window->cursor;
    if (cursor >= 0) {
        menuSubDrawCursorBackAnimeOnOffAuto(window, winSpriteGetLayerID(window, sprite), 0, cursor << 5,
                                            sprite->mX, sprite->mY, window->mRgba.mColorA, 1);
    }
    return 0;
}

// Draw a textured quad for the sprite on the given layer.
void drawTexture(GStexture *texture, tagSPRITE_WORK *sprite, GSgfxLayerID layerID)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);

    if (layer == 0) {
        return;
    }
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(0, 3, 0, 0, 1, 5, 1, 1, 4));
    if (layer->mTextureA != texture || layer->mTextureB != texture || texture->mFormat != 0) {
        layer->mTextureA = texture;
        layer->mTextureStateA |= 1;
        layer->mTextureB = texture;
        layer->mTextureStateB |= 1;
    }
    layer->mPrimitiveType = (GSgfxPrimType)0x98;
    layer->beginSurface(4);

    f32 zero = 0.0f;
    f32 one = 1.0f;
    s16 x;
    s16 y;

    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.f32 = zero;
    GXWGFifo.f32 = zero;
    x = sprite->mX;
    GXWGFifo.s16 = x;
    GXWGFifo.u16 = 0;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.f32 = one;
    GXWGFifo.f32 = zero;
    y = sprite->mY;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = y;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.f32 = zero;
    GXWGFifo.f32 = one;
    x = sprite->mX;
    y = sprite->mY;
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.u8 = 0xFF;
    GXWGFifo.f32 = one;
    GXWGFifo.f32 = one;
    GSgfxRenderDrawEnd(layer);
}

// Mode and message display of the name-entry screen.
int menuNameEntryDispItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;
    Pokemon *pokemon;
    s32 n = 0;
    s32 show = 0;

    switch (sprite->mItemID) {
    case 0x675:
    case 0x676:
    case 0x677:
    case 0x678:
    case 0x679:
    case 0x8EA:
    case 0x8EB:
        if (arg->mMode >= 7 || arg->mMode < 2) {
            show = 1;
        }
        break;
    case 0x683:
        n = 1;
    case 0x682:
        n++;
    case 0x681:
        n++;
    case 0x680:
        n++;
    case 0x67F:
        n++;
    case 0x67E:
        n++;
    case 0x67D:
        n++;
    case 0x67C:
        n++;
    case 0x67B:
        n++;
    case 0x67A:
        switch (arg->mMode) {
        case 2:
        case 4:
        case 5:
        case 6:
            show = 1;
            break;
        case 3:
            if (n != 0 && n != 9) {
                show = 1;
            }
            break;
        default:
            break;
        }
        break;
    case 0x876:
        pokemon = 0;
        switch (arg->mMode) {
        case 2:
            pokemon = heroGetStatus(0, 3, (u16)arg->mParam);
            break;
        case 4:
            pokemon = fightFloorGetDefPokemonPtr(0);
            break;
        case 5:
            pokemon = (Pokemon *)arg->mParam;
            break;
        default:
            break;
        }
        if (pokemon != 0) {
            if (pokemonGetSex(pokemon) == 0) {
                show = 0x9C5;
            } else if (pokemonGetSex(pokemon) == 1) {
                show = 0x9C6;
            }
            if (show != 0) {
                msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(show));
                GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, window->mColor, 0x4275);
                show = 0;
            }
        }
        break;
    default:
        break;
    }

    winSpriteSetDisp(sprite, 0);
    if (show != 0) {
        winSpriteSetDisp(sprite, 1);
    }
    return 0;
}

// Pick the name box style from the menu type.
void menuNameEntryDrawNameBox(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 mode = window->mArg->mMode;

    if (mode == 6) {
        menuNameEntryDrawNameBoxCur(window, sprite, 10, 6);
    } else if (mode == 3) {
        menuNameEntryDrawNameBoxCur(window, sprite, 8, 3);
    } else if (mode == 2) {
        menuNameEntryDrawNameBoxCur(window, sprite, 10, 2);
    } else if (mode == 4) {
        menuNameEntryDrawNameBoxCur(window, sprite, 10, 4);
    } else if (mode == 5) {
        menuNameEntryDrawNameBoxCur(window, sprite, 10, 5);
    } else {
        menuNameEntryDrawNameBoxCur(window, sprite, 7, 0);
    }
}

// Cursor blink of the name box.
int menuNameEntryDrawNameBoxCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 count, s32 mode)
{
    NAME_ENTRY_ARG *arg = window->mArg;
    s32 limit = nameEntryModeData[arg->mMode].mMaxLength;
    s32 pos = *arg->mCursor;
    s32 n = 0;
    s32 show = 0;

    if (pos >= limit) {
        pos = limit - 1;
    }
    winSpriteSetDisp(sprite, 0);
    switch (sprite->mItemID) {
    case 0x8ED:
        n = 1;
    case 0x8EC:
        n++;
    case 0x30B:
        n++;
    case 0x30A:
        n++;
    case 0x309:
        n++;
    case 0x308:
        n++;
    case 0x307:
        if (arg->mMode >= 7 || arg->mMode < 2) {
            show = 1;
        }
        break;
    case 0x68D:
        n = 1;
    case 0x68C:
        n++;
    case 0x68B:
        n++;
    case 0x68A:
        n++;
    case 0x689:
        n++;
    case 0x688:
        n++;
    case 0x687:
        n++;
    case 0x686:
        n++;
    case 0x685:
        n++;
    case 0x684:
        switch (mode) {
        case 2:
        case 4:
        case 5:
        case 6:
            show = 1;
            break;
        case 3:
            if (n != 0 && n != 9) {
                show = 1;
                n--;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    if (show != 0) {
        if (n == pos) {
            float s = sin(3.14f * *arg->mTimer * 2.0f);

            sprite->mAlpha = 128.0f * (1.0f + s);
            winSpriteSetDisp(sprite, 1);
        } else if (n < pos) {
            sprite->mAlpha = 0xFF;
            winSpriteSetDisp(sprite, 1);
        }
    }
    return 0;
}

// Draw the name rule text.
int menuNameEntryDrawNameRuleText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    return menuNameEntryDrawNameText(window, sprite, 10, 6);
}

// Draw the name text of the Ken name screen (mode 0).
int menuNameEntryDrawKenNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    return menuNameEntryDrawNameText(window, sprite, 7, 0);
}

// Draw the name box text.
int menuNameEntryDrawNameBoxText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    return menuNameEntryDrawNameText(window, sprite, 8, 3);
}

// Draw the name text for the window's mode (modes 1, 2, 4 and 5).
int menuNameEntryDrawModeNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 mode = window->mArg->mMode;

    if (mode == 6) {
        return 0;
    }
    if (mode == 3) {
        return 0;
    }
    if (mode == 0) {
        return 0;
    }
    return menuNameEntryDrawNameText(window, sprite, 10, mode);
}

// Draw the characters of the name being edited.
int menuNameEntryDrawNameText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 count, s32 mode)
{
    NAME_ENTRY_ARG *arg = window->mArg;
    s32 x;
    u16 chr;
    u16 *chars;
    u16 blink;
    if (arg->mMode == 1) {
        if (mode == 0) {
            sprite->mAlpha = 0xFF;
        } else {
            sprite->mAlpha = 0;
        }
    } else if (arg->mMode != mode) {
        sprite->mAlpha = 0;
    } else {
        sprite->mAlpha = 0xFF;
    }
    if (sprite->mAlpha != 0) {
        chars = arg->mChars;
        count = 0;
        x = 0;
        while ((chr = *chars) != 0) {
            print1Char(winSpriteGetLayerID(window, sprite), x, 0, window->mRgba.mColorA | 0xFFFFFF00, chr);
            x += 0x1A;
            count++;
            chars++;
        }
        if (count < nameEntryModeData[arg->mMode].mMaxLength && window->mAccept == 0) {
            blink = get50onChar(*arg->mTable, *arg->mColumn, *arg->mRow);
            if (blink != 0 && getCharClass(blink) == 6) {
                float s = sin(3.14f * *arg->mTimer * 2.0f);
                u32 color = (u8)(128.0f * (1.0f + s)) | 0xFFFFFF00;

                print1Char(winSpriteGetLayerID(window, sprite), count * 0x1A, 0, color, blink);
            }
        }
        sprite->mAlpha = 0xFF;
    }
    return 0;
}

// Show the delete/end button cursor of the position selected on the 50-on
// table (row 6 column >6 is delete, row 7 column >6 is end).
void menuNameEntryDispButtonCur(NAME_ENTRY_ARG *arg, tagSPRITE_WORK *sprite)
{
    s32 column = *arg->mColumn;
    s32 row = *arg->mRow;

    winSpriteSetDisp(sprite, 0);
    if (row > 5 && column > 6) {
        s32 id = (row == 6) ? 0x305 : 0x304;

        if (id == sprite->mItemID) {
            winSpriteSetDisp(sprite, 1);
        }
    }
}

// Draw the delete (B) button cursor.
int menuNameEntryDrawBCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    menuNameEntryDispButtonCur(arg, sprite);
    return 0;
}

// Draw the end button cursor.
int menuNameEntryDrawEndCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    menuNameEntryDispButtonCur(arg, sprite);
    return 0;
}

// Draw the cursor over the selected 50-on table cell.
int menuNameEntryDrawCellCur(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    winSpriteSetDisp(sprite, 0);
    if (*arg->mRow < 6) {
        if (*arg->mColumn < 0x0B) {
            windowDrawTextureId(*arg->mColumn * 0x1A, *arg->mRow * 0x1E, window, 0x153, 0, winSpriteGetLayerID(window, sprite));
        }
    } else {
        if (*arg->mColumn < 7) {
            windowDrawTextureId(*arg->mColumn * 0x1A, *arg->mRow * 0x1E, window, 0x153, 0, winSpriteGetLayerID(window, sprite));
        }
    }
    return 0;
}

// Look up the character of the 50-on table cell.
u16 get50onChar(s32 table, s32 column, s32 row)
{
    s32 id;
    u16 *string;
    s32 count;

    if (table < 0 || table >= 2) {
        return 0;
    }
    if (row < 0 || row >= 8) {
        return 0;
    }
    id = nameEntry50onData[table].mRow[row];
    if ((u32)id == 0) {
        return 0;
    }
    count = GSmsgGetLength(id);

    if (column < 0 || column >= count) {
        return 0;
    }
    string = GSmsgGetStringPtr(id);
    if (*string == 0xFFFF) {
        string += 2;
    }
    return string[column];
}

// Length of a GS string.
int getGScharLen(u16 *string)
{
    s32 length = 0;

    while (*string != 0) {
        string++;
        length++;
    }
    return length;
}

// Print one character centered on the sprite.
void print1Char(GSgfxLayerID layer, s32 x, s32 y, u32 color, u16 chr)
{
    u16 text[2];
    s32 width;

    text[0] = chr;
    text[1] = 0;
    msgctrlSetValue(0x37, (unsigned long)text);
    width = (0x1B - (s16)(GSmsgGetMessageSize(0x4275) >> 16)) / 2;
    GSmsgPrint2(layer, x + width, y, color, 0x4275);
}

// Draw the 50-on character table.
int menuNameEntryDraw50Text(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;
    s32 x;
    s32 y;
    s32 column;
    s32 row;
    s32 table;
    u16 chr;

    row = 0;
    y = 0;
    table = *arg->mTable;
    while (row < 8) {
        column = 0;
        x = -2;
        while ((chr = get50onChar(table, column, row)) != 0) {
            GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);

            print1Char(layer, x, y, window->mRgba.mColorA | 0xFFFFFF00, chr);
            x += 0x1A;
            column++;
        }
        y += 0x1E;
        row++;
    }
    return 0;
}

// Draw the title of the 50-on table the hira/kata toggle switches to.
int menuNameEntryDraw50onToggleTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 table = *window->mArg->mTable + 1;

    if (table >= 2) {
        table -= 2;
    }
    if (table >= 0 && table < 2) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    window->mRgba.mColorA | 0xFFFFFF00, nameEntry50onData[table].mTitle);
    }
    return 0;
}

// Draw the name subject (hero, box or Pokemon).
int menuNameEntryDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg;
    s32 width;

    if (window->mMenuID == NAME_ENTRY_MENU_ID_SELECT) {
        width = GSmsgGetMessageSize(0x3B11) >> 16;

        GSmsgPrint2(winSpriteGetLayerID(window, sprite), (sprite->mX - width) / 2, 0,
                    window->mRgba.mColorA | 0xFFFFFF00, 0x3B11);
    } else {
        arg = window->mArg;
        switch (arg->mMode) {
        case 2: {
            Pokemon *pokemon = heroGetStatus(0, 3, (u16)arg->mParam);
            u16 species;

            if (pokemonCheckValid(pokemon)) {
                species = battlePokemonGetSpecies(pokemon);
            } else {
                species = 1;
            }
            msgctrlSetValue(0x4E, species);
            width = GSmsgGetMessageSize(nameEntryModeData[arg->mMode].mMessageID) >> 16;
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), (sprite->mX - width) / 2, 0,
                        window->mRgba.mColorA | 0xFFFFFF00, nameEntryModeData[arg->mMode].mMessageID);
            break;
        }
        case 5: {
            u16 species = battlePokemonGetSpecies((Pokemon *)arg->mParam);
            msgctrlSetValue(0x4E, species);
            width = GSmsgGetMessageSize(nameEntryModeData[arg->mMode].mMessageID) >> 16;
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), (sprite->mX - width) / 2, 0,
                        window->mRgba.mColorA | 0xFFFFFF00, nameEntryModeData[arg->mMode].mMessageID);
            break;
        }
        case 4: {
            Pokemon *pokemon = fightFloorGetDefPokemonPtr(0);
            u16 species;

            if (pokemonCheckValid(pokemon)) {
                species = battlePokemonGetSpecies(pokemon);
            } else {
                species = 1;
            }
            msgctrlSetValue(0x4E, species);
            width = GSmsgGetMessageSize(nameEntryModeData[arg->mMode].mMessageID) >> 16;
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), (sprite->mX - width) / 2, 0,
                        window->mRgba.mColorA | 0xFFFFFF00, nameEntryModeData[arg->mMode].mMessageID);
            break;
        }
        default: {
            width = GSmsgGetMessageSize(nameEntryModeData[arg->mMode].mMessageID) >> 16;

            GSmsgPrint2(winSpriteGetLayerID(window, sprite), (sprite->mX - width) / 2, 0,
                        window->mRgba.mColorA | 0xFFFFFF00, nameEntryModeData[arg->mMode].mMessageID);
            break;
        }
        }
    }
    return 0;
}

// Step the character to the neighboring kana table.
u16 conv50onChar(u16 chr, s32 table)
{
    s32 i = 0;
    s32 n;
    u16 *p;
    s32 j;

    while (i < NAME_ENTRY_TABLE_COUNT) {
        if (nameEntryKanaTables[i] != 0) {
            n = GSmsgGetLength(nameEntryKanaTables[i]);
            p = GSmsgGetStringPtr(nameEntryKanaTables[i]) + 1;
            j = 1;
            while (j < n) {
                if (*p == chr) {
                    break;
                }
                j += 2;
                p += 2;
            }
            if (j < n) {
                break;
            }
        }
        i++;
    }
    if (i >= NAME_ENTRY_TABLE_COUNT) {
        i = 0;
    }
    if (i == table) {
        return chr;
    }
    // The character came from the kana table: step it back one slot.
    if (i == NAME_ENTRY_TABLE_KANA || i == NAME_ENTRY_TABLE_COUNT - 1) {
        chr = p[-1];
    }
    if (table == 0) {
        return chr;
    }
    n = GSmsgGetLength(nameEntryKanaTables[table]);
    p = GSmsgGetStringPtr(nameEntryKanaTables[table]);
    j = 0;
    while (j < n) {
        if (*p == chr) {
            break;
        }
        j += 2;
        p += 2;
    }
    if (j >= n) {
        return 0;
    }
    return p[1];
}

// Confirm the name with a yes/no dialog.
int confirmName(u16 *name)
{
    s8 answer;

    GSsndPlayNow(0x1D);
    msgctrlSetValue(0x4D, (unsigned long)name);
    winMsgOpen(2, 0x3B1A, 1, 0);
    answer = menuSubOpenYesNo(0, 0x2C, 0xC0, 0);
    winMsgClose(1);
    if (answer == 1 || answer == -1) {
        return 0;
    }
    return 1;
}

// Erase the character before the cursor.
u8 backNameCursor(NAME_ENTRY_ARG *arg)
{
    s32 cursor = *arg->mCursor;
    u16 *p;

    if (cursor <= 0) {
        return 0;
    }
    cursor--;
    p = arg->mChars;
    p += cursor;
    *p = 0;
    *arg->mCursor = cursor;
    return 1;
}

// Classify the character for the input state machine.
s32 getCharClass(u16 chr)
{
    u16 *space = GSmsgGetStringPtr(0x3B1E);

    if (chr == *space) {
        return 0;
    }
    return 6;
}

// Toggle the 50-on table.
void toggle50onTable(NAME_ENTRY_ARG *arg)
{
    s32 table = *arg->mTable;

    table++;
    if (table >= 2) {
        table = 0;
    }
    *arg->mTable = table;
}

// Feed one character press into the name buffer.
int input50onCell(NAME_ENTRY_ARG *arg)
{
    u16 chr;
    s32 col = *arg->mColumn;
    s32 act = *arg->mRow;
    s32 charClass = 6; // class of a key that inserts a character.
    s32 ret = 0;
    u32 sound = 0;

    if (act > 5 && col > 6) {
        switch (act) {
        case 7:
            act = 5;
            break;
        default:
            act = 4;
            break;
        }
    } else {
        chr = get50onChar(*arg->mTable, col, act);
        if (chr == 0) {
            chr = *GSmsgGetStringPtr(0x3B1D);
        }
        charClass = getCharClass(chr);
        act = charClass;
    }
    switch (act) {
    case 1:
    case 2: {
        s32 table;

        if (act == 1) {
            table = NAME_ENTRY_TABLE_COUNT - 1;
        } else {
            table = NAME_ENTRY_TABLE_KANA;
        }
        s32 cur = *arg->mCursor - 1;

        if (cur >= 0) {
            u16 *chars = arg->mChars;
            u16 c = conv50onChar(chars[cur], table);

            if (c != 0) {
                chars[cur] = c;
            }
        }
        sound = 0x11;
        break;
    }
    case 4:
        if (backNameCursor(arg)) {
            sound = 0x12;
        }
        break;
    case 5:
        ret = 1;
        break;
    case 3:
        toggle50onTable(arg);
        sound = 0x14;
        break;
    case 0:
        chr = *GSmsgGetStringPtr(0x3B1D);
    default: {
        s32 cur;
        s32 maxLen;
        u16 *p;
        s32 next;

        maxLen = nameEntryModeData[arg->mMode].mMaxLength;
        cur = *arg->mCursor;

        if (cur >= maxLen) {
            cur = maxLen - 1;
        }
        p = &arg->mChars[cur];
        p[0] = chr;
        p[1] = 0;
        next = cur + 1;
        if (next >= maxLen + 1) {
            next = maxLen;
        }
        *arg->mCursor = next;
        sound = 0x11;
        break;
    }
    }
    if (sound != 0) {
        GSsndPlayNow(sound);
    }
    return ret;
}

// Drive the name editor from the pad.
int menuNameEntryInputButton(tagWINDOW_WORK *window)
{
    s32 repeat = windowGetKeyRepeat();
    u16 trg = windowGetKeyTrg();
    NAME_ENTRY_ARG *arg = window->mArg;

    if (trg & 0x40) {
        toggle50onTable(arg);
        GSsndPlayNow(0x14);
        return 0;
    }
    if (trg & 0x10) {
        if (input50onCell(arg) != 0) {
            window->mAccept = 1;
            return 0;
        }
        if (getGScharLen(arg->mChars) >= nameEntryModeData[arg->mMode].mMaxLength) {
            *arg->mColumn = 10;
            *arg->mRow = 7;
        }
        return 0;
    }
    if (trg & 0x20) {
        if (backNameCursor(arg) != 0) {
            GSsndPlayNow(0x12);
        }
        return 0;
    }
    if (trg & 0x400) {
        toggle50onTable(arg);
        GSsndPlayNow(0x14);
        return 0;
    }
    if (trg & 0x800) {
        window->mAccept = 1;
        return 0;
    }
    if ((u16)repeat & 0x8) {
        s32 col = *arg->mColumn + 1;

        if (*arg->mRow < 6) {
            if (col < 11) {
                GSsndPlayNow(0x10);
            } else {
                col = 10;
            }
        } else if (col > 7) {
            col = 7;
        } else {
            GSsndPlayNow(0x10);
        }
        *arg->mColumn = col;
    }
    if ((u16)repeat & 0x4) {
        s32 col = *arg->mColumn - 1;

        if (*arg->mRow > 5 && col > 6) {
            col = 6;
        }
        if (col < 0) {
            col = 0;
        } else {
            GSsndPlayNow(0x10);
        }
        *arg->mColumn = col;
    }
    if ((u16)repeat & 0x2) {
        s32 row = *arg->mRow;

        if (*arg->mColumn >= 11) {
            if (row == 1) {
                row += 2;
            } else {
                row += 1;
            }
        } else {
            row += 1;
        }
        if (row >= 8) {
            row = 7;
        } else {
            GSsndPlayNow(0x10);
        }
        *arg->mRow = row;
    }
    if ((u16)repeat & 0x1) {
        s32 row = *arg->mRow;

        if (*arg->mColumn >= 11) {
            if (row == 2) {
                row -= 2;
            } else {
                row -= 1;
            }
        } else {
            row -= 1;
        }
        if (row < 0) {
            row = 0;
        } else {
            GSsndPlayNow(0x10);
        }
        *arg->mRow = row;
    }
    return 0;
}

// Sequence the name-entry window.
int menuNameEntryCtrl(tagWINDOW_WORK *window)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x58);
            *arg->mTimer = 0.0f;
            window->mFlag = 1;
        }
        break;
    case 2: {
        f32 step = 0.033333335f;
        f32 t = *arg->mTimer + step;

        *arg->mTimer = t;
        if (t >= 1.0f) {
            *arg->mTimer -= 1.0f;
        }
        break;
    }
    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x5C);
            window->mFlag = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

void menuNameEntryInputDummy(void)
{
}

// True when the name is not blank or prohibited.
int chkValidName(u16 *name)
{
    s32 length = getGScharLen(name);
    u16 *prohibited;
    s32 i;

    if (length <= 0) {
        return 0;
    }
    prohibited = GSmsgGetStringPtr(0x3B1D);
    for (i = 0; i < length; i++, name++) {
        if (*name != *prohibited) {
            break;
        }
    }
    if (i >= length) {
        return 0;
    }
    return 1;
}

// Type a name on the input menu.
int inputName(u16 *name, u16 *initial, s32 type, s32 param, s32 skip)
{
    NAME_ENTRY_ARG arg;
    NAME_ENTRY_ARG *argp;
    u16 *chosen;
    s32 accepted;
    s32 done = 0;

    GScharCpy(arg.mInitialName, initial);
    *name = 0;
    arg.mChars = name;
    arg.mMode = type;
    arg.mParam = param;
    nameEntryTable = 0;
    arg.mTable = &nameEntryTable;
    nameEntryColumn = 0;
    arg.mColumn = &nameEntryColumn;
    nameEntryRow = 0;
    arg.mRow = &nameEntryRow;
    nameEntryTimer = 0.0f;
    arg.mTimer = &nameEntryTimer;
    nameEntryCursor = 0;
    arg.mCursor = &nameEntryCursor;
    arg.mUnk38 = &nameEntrySharedVal0;
    arg.mUnk3C = &nameEntrySharedVal1;
    arg.mUnk40 = &nameEntrySharedVal2;
    arg.mUnk44 = &nameEntrySharedVal3;
    arg.mUnk48 = &nameEntrySharedVal4;
    dbgMenuSetOpenFlag(0);
    argp = &arg;
    while (done == 0) {
        menuOpenCustom(NAME_ENTRY_MENU_ID_INPUT, windowGetActiveID(), 0, 0, 1, 1, argp);
        if (chkValidName(arg.mChars)) {
            chosen = arg.mChars;
        } else {
            chosen = arg.mInitialName;
        }
        accepted = confirmName(chosen);
        if (skip == 0) {
            done = 1;
            if (accepted != 0) {
                continue;
            }
            nameEntryWork.mName[0] = 0;
        } else if (accepted != 0) {
            done = 1;
        }
    }
    dbgMenuSetOpenFlag(1);
    menuClose(NAME_ENTRY_MENU_ID_INPUT);
    menuCloseSync(NAME_ENTRY_MENU_ID_INPUT, 1);
    if (accepted != 0) {
        GScharCpy(nameEntryWork.mName, chosen);
        return 1;
    }
    return 0;
}

// Set the default name text of the sprite.
void setDefNameText(tagSPRITE_WORK *sprite, unsigned long stringID)
{
    msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(stringID));
    sprite->mMsgID = 0x4276;
}

// Draw the third default name.
int menuNameEntrySelectDrawSelText3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    setDefNameText(sprite, arg->mSelect.mNameIDs[2]);
    return 0;
}

// Draw the second default name.
int menuNameEntrySelectDrawSelText2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    setDefNameText(sprite, arg->mSelect.mNameIDs[1]);
    return 0;
}

// Draw the first default name.
int menuNameEntrySelectDrawSelText1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    NAME_ENTRY_ARG *arg = window->mArg;

    setDefNameText(sprite, arg->mSelect.mNameIDs[0]);
    return 0;
}

// Button input of the select menu.
int menuNameEntrySelectButton(tagWINDOW_WORK *window)
{
    u16 keys = windowGetKeyTrg();

    if (keys & 0x20) {
        window->mCancel = 1;
        window->mAccept = 1;
    }
    if (keys & 0x10) {
        window->mAccept = 1;
    }
    return keys;
}

// Sequence the select menu window.
int menuNameEntrySelectCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x58);
            window->mFlag = 1;
        }
        break;
    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x5C);
            window->mFlag = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

// Pick a name from the default name list.
int selectName(u16 *name, const unsigned long *nameIDs, s32 param)
{
    struct {
        const unsigned long *table;
        s32 count;
    } args, *argp = &args;
    long result;
    s32 accepted = 0;
    u16 *chosen;

    for (;;) {
        unsigned long id;

        argp->table = nameIDs;
        argp->count = param;
        result = menuOpenCustom(NAME_ENTRY_MENU_ID_SELECT, windowGetActiveID(), 0, 0, 1, 1, argp);
        if (result == 0) {
            GSsndPlayNow(0x11);
            accepted = 0;
            break;
        }
        if (result == 4 || result == -1) {
            GSsndPlayNow(0x11);
            accepted = 0;
            break;
        }
        id = nameIDs[result - 1];
        chosen = GSmsgGetStringPtr(id);
        if (confirmName(chosen) == 0) {
            continue;
        }
        accepted = 1;
        break;
    }
    menuClose(NAME_ENTRY_MENU_ID_SELECT);
    menuCloseSync(NAME_ENTRY_MENU_ID_SELECT, 1);
    if (accepted != 0) {
        GScharCpy(name, chosen);
        return 0;
    }
    if (result != 0) {
        return 2;
    }
    return 1;
}

// Draw the human model behind the name entry.
int menuNameEntryBackDrawHumanModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 type = *(s32 *)window->mArg;

    if (type == 0 || type == 1) {
        ModelExtent pos = cHumanModelPos;
        ModelExtent size = cHumanModelSize;
        f32 angle[3];
        f32 offset[3];
        GStexture *texture;

        angle[0] = 0.0f;
        angle[1] = 0.3f;
        angle[2] = 0.0f;
        offset[0] = -4.0f;
        offset[1] = 0.0f;
        offset[2] = 0.0f;

        menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
        menuModelAdjustCamera(&nameEntryModel);
        menuModelSetSize(&nameEntryModel, pos.mX, pos.mY, size.mX, size.mY);
        menuModelSetFlag(&nameEntryModel, 0);
        menuModelSetAngle(&nameEntryModel, angle);
        menuModelSetZoom(&nameEntryModel, 1.4f);
        menuModelSetOffset(&nameEntryModel, offset);
        texture = menuModelRender(&nameEntryModel);
        if (texture != 0) {
            drawTexture(texture, sprite, winSpriteGetLayerID(window, sprite));
        }
    }
    return 0;
}

// Draw the Pokemon model behind the name entry.
int menuNameEntryBackDrawPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 type = *(s32 *)window->mArg;
    f32 zoom;
    f32 angle[3];
    f32 offset[3];
    GStexture *texture;

    if (type == 2 || type == 4 || type == 5) {
        ModelExtent pos = cPokemonModelPos;
        ModelExtent size = cPokemonModelSize;

        zoom = 1.2f;
        angle[0] = 0.0f;
        angle[1] = 0.3f;
        angle[2] = 0.0f;
        offset[0] = 0.0f;
        offset[1] = 0.0f;
        offset[2] = 0.0f;
        switch (nameEntryModel.mSpecies) {
        case 0x28:
        case 0x3E:
        case 0x47:
        case 0x57:
        case 0x5F:
        case 0x60:
        case 0x6C:
        case 0xA3:
        case 0xAE:
        case 0xC0:
        case 0xE6:
        case 0x154:
        case 0x18E:
            zoom = 1.0f;
            break;
        case 0x1A:
        case 0x7B:
        case 0x97:
        case 0x143:
            zoom = 0.9f;
            break;
        case 0xE9:
            zoom = 0.8f;
            break;
        case 0xD0:
            zoom = 0.95f;
            break;
        default:
            break;
        }
        {
            menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
            menuModelAdjustCamera(&nameEntryModel);
            menuModelSetSize(&nameEntryModel, pos.mX, pos.mY, size.mX, size.mY);
            menuModelSetFlag(&nameEntryModel, 0);
            menuModelSetAngle(&nameEntryModel, angle);
            menuModelSetZoom(&nameEntryModel, zoom);
            menuModelSetOffset(&nameEntryModel, offset);
            texture = menuModelRender(&nameEntryModel);
            if (texture != 0) {
                drawTexture(texture, sprite, winSpriteGetLayerID(window, sprite));
            }
        }
    }
    return 0;
}

// Load the ball textures for the background.
int menuNameEntryBackDrawBalls(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    void *entry = textureBiosGetLanguageEntry(textureBiosGetDataTable(0x1E));
    s32 count;
    s16 texW = *(s16 *)((u8 *)entry + 4);
    s16 texH = *(s16 *)((u8 *)entry + 6);

    for (count = 0; count < 0x1E; count++) {
        BALL_WORK *ball = &nameEntryBalls[count];

        if (ball->mActive != 0) {
            f32 w = (f32)texW * ball->mScale;
            f32 h = (f32)texH * ball->mScale;
            f32 x = ball->mX - w / 2.0f;
            f32 y = ball->mY - h / 2.0f;
            u32 flags = ball->mFlags | ball->mAlpha;

            textureBiosLoadSpriteId((s32)(0.5f + x), (s32)(0.5f + y), (s32)(0.5f + w), (s32)(0.5f + h),
                                    flags, window, 0x1E, 0, winSpriteGetLayerID(window, sprite));
        }
    }
    return 0;
}

// Step one background ball.
void ballMain1(BALL_WORK *ball)
{
    f32 rate;
    f32 t;

    if (ball->mActive == 0) {
        return;
    }
    t = ball->mTime;
    t += 1.0f;
    ball->mTime = t;
    if (t >= ball->mTimeMax) {
        ball->mActive = 0;
    }
    rate = ball->mTime / ball->mTimeMax;
    ball->mScale = ball->mScaleMax * rate;
    ball->mAlpha = (u8)(128.0f * (1.0f - rate));
}

// Spawn a background ball.
void generateBall(void)
{
    s32 i;

    if (HSD_Randf() > 0.1f) {
        return;
    }
    for (i = 0; i < 30; i++) {
        if (nameEntryBalls[i].mActive == 0) {
            break;
        }
    }
    if (i >= 30) {
        return;
    }
    nameEntryBalls[i].mActive = 1;
    nameEntryBalls[i].mX = 640.0f * HSD_Randf();
    nameEntryBalls[i].mY = 480.0f * HSD_Randf();
    nameEntryBalls[i].mScale = 0.001f;
    nameEntryBalls[i].mScaleMax = 1.999f * HSD_Randf() + 0.001f;
    nameEntryBalls[i].mFlags = -0x100;
    nameEntryBalls[i].mAlpha = 0x80;
    nameEntryBalls[i].mTime = 0.0f;
    nameEntryBalls[i].mTimeMax = 60.0f * HSD_Randf() + 120.0f;
}

// Step the background balls.
void ballMain(void)
{
    s32 i;

    generateBall();
    for (i = 0; i < 0x1E; i++) {
        if (nameEntryBalls[i].mActive != 0) {
            ballMain1(&nameEntryBalls[i]);
        }
    }
}

// Sequence the background of the name entry.
int menuNameEntryBackCtrl(tagWINDOW_WORK *window)
{
    s32 i;

    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            for (i = 0; i < 30; i++) {
                nameEntryBalls[i].mActive = 0;
            }
            for (i = 0; i < 600; i++) {
                ballMain();
            }
            window->mFlag = 1;
        }
        break;
    case 2:
        ballMain();
        break;
    case 3:
        if (window->mFlag == 0) {
            window->mFlag = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

// Set up the model the name entry shows.
void menuNameEntrySetupModel(long dataID, MENU_MODEL *model, s32 type, s32 param)
{
    unsigned long ids[4];
    tagMENU_ITEM_DATA *item;
    long menuID;

    {
        s32 i;

        for (i = 0; i < 4; i++) {
            ids[i] = nameEntryModelIDs[i];
        }
    }
    switch (type) {
    case 2:
    case 4:
    case 5:
        menuID = 0x280;
        break;
    case 3:
    case 6:
    default:
        menuID = 0x27F;
        break;
    }
    item = menuItemBiosGetPtr(dataID, menuID);
    menuModelInit(model, item->mOffsetB, item->mOffsetC);
    switch (type) {
    case 0:
    case 1: {
        s32 motion;
        s32 unk;
        void *info;

        menuModelLoad(model, ids[type]);
        info = peopleInfoBiosGetPtr(ids[type]);
        peopleInfoBiosGetMotion(info, 1, &motion, &unk);
        menuModelSetMotion(model, motion);
        break;
    }
    case 2: {
        Pokemon *pokemon = heroGetStatus(0, 3, (u16)param);

        if (pokemonCheckValid(pokemon)) {
            menuModelLoadPokemon(model, pokemon);
        }
        break;
    }
    case 5:
        menuModelLoadPokemon(model, (Pokemon *)param);
        break;
    case 3:
        menuModelLoad(model, ids[type]);
        break;
    case 4: {
        Pokemon *pokemon = fightFloorGetDefPokemonPtr(0);

        if (pokemonCheckValid(pokemon)) {
            menuModelLoadPokemon(model, pokemon);
        }
        break;
    }
    case 6:
    default:
        break;
    }
    menuModelCheck(model, 1);
}

// Run the name-entry screen.
void menuNameEntry(void)
{
    u16 name[12];
    struct {
        s32 type;
        s32 param;
    } args;

    if (getName(name, nameEntryWork.mType, nameEntryWork.mParam) == 0) {
        name[0] = 0;
    }
    menuNameEntrySetupModel(0x68, &nameEntryModel, nameEntryWork.mType, nameEntryWork.mParam);
    {
        static const char startLabel[] = "start";
        static const char endLabel[] = "end";

        FlashSystemSetPlayFrameLabel(0x22063000, startLabel, endLabel);
        FlashSystemSetLoopFlag(0x22063000, 0);
        FlashSystemSetNowFrameLabel(0x22063000, startLabel);
    }
    args.type = nameEntryWork.mType;
    args.param = nameEntryWork.mParam;
    menuOpenCustom(NAME_ENTRY_MENU_ID_MAIN, windowGetActiveID(), 0, 0, 1, 1, &args);
    if (nameEntryWork.mBlock == 0) {
        fadeSet(2, 0.5f);
        fadeCheck(1);
    }
    if (nameEntryModeData[nameEntryWork.mType].mDefaultNameCount > 0) {
        s32 loop = 1;

        while (loop != 0) {
            switch (selectName(nameEntryWork.mName, nameEntryModeData[nameEntryWork.mType].mDefaultNames,
                               nameEntryModeData[nameEntryWork.mType].mDefaultNameCount)) {
            case 1:
                if (inputName(nameEntryWork.mName, name, nameEntryWork.mType, nameEntryWork.mParam, 0) != 0) {
                    loop = 0;
                }
                break;
            case 2:
                loop = 0;
                break;
            default:
                loop = 0;
                break;
            }
        }
    } else {
        inputName(nameEntryWork.mName, name, nameEntryWork.mType, nameEntryWork.mParam, 1);
    }
    fadeSet(3, 0.5f);
    fadeCheck(1);
    menuModelTerm(&nameEntryModel);
    menuClose(NAME_ENTRY_MENU_ID_MAIN);
    menuCloseSync(NAME_ENTRY_MENU_ID_MAIN, 1);
    setName(nameEntryWork.mName, nameEntryWork.mType, nameEntryWork.mParam);
    if (GScharCmp(nameEntryWork.mName, name) == 0) {
        nameEntryWork.mChanged = 0;
    } else {
        nameEntryWork.mChanged = 1;
    }
    if (nameEntryWork.mType == 0) {
        menuTitleBattleSelectFree();
    }
    if (nameEntryWork.mBlock != 0) {
        floorPop();
        if (nameEntryWork.mFade != 0) {
            floorSetFadeScript(0, 0x5960008);
        } else {
            floorSetFadeScript(0, 0);
        }
    }
}

// Store the edited name.
int setName(u16 *name, s32 type, s32 param)
{
    s32 result = 1;
    Pokemon *pokemon;

    switch (type) {
    case 0:
        break;
    case 1:
        heroSetStatus(0, 0x17, name);
        break;
    case 2:
        pokemon = heroGetStatus(0, 3, (u16)param);

        if (pokemonCheckValid(pokemon) == 0) {
            result = 0;
        } else {
            pokemon_SetNickname(pokemon, name);
        }
        break;
    case 5:
        pokemon_SetNickname((Pokemon *)param, name);
        break;
    case 3:
        pcboxSetPokemonBoxName(0, (s8)param, name);
        break;
    case 4:
        pokemon = fightFloorGetDefPokemonPtr(0);

        if (pokemonCheckValid(pokemon) == 0) {
            result = 0;
        } else {
            pokemon_SetNickname(pokemon, name);
        }
        break;
    case 6:
        GScharCpy((u16 *)param, name);
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

// Load the name being edited.
int getName(u16 *name, s32 type, s32 param)
{
    const u16 *src;

    switch (type) {
    case 0:
        src = GSmsgGetStringPtr(DefKenName[0]);
        break;
    case 1:
        src = GSmsgGetStringPtr(DefHizukiName[0]);
        break;
    case 2: {
        Pokemon *pokemon = heroGetStatus(0, 3, (u16)param);

        if (pokemonCheckValid(pokemon) == 0) {
            src = 0;
        } else {
            src = pokemon_GetNickname(pokemon);
        }
        break;
    }
    case 5:
        src = pokemon_GetNickname((Pokemon *)param);
        break;
    case 3:
        src = pcboxGetPokemonName(0, (s8)param);
        break;
    case 4: {
        Pokemon *pokemon = fightFloorGetDefPokemonPtr(0);

        if (pokemonCheckValid(pokemon) == 0) {
            src = 0;
        } else {
            src = pokemon_GetNickname(pokemon);
        }
        break;
    }
    case 6:
        src = (const u16 *)param;
        break;
    default:
        src = 0;
        break;
    }
    if (src == 0) {
        return 0;
    }
    GScharCpy(name, src);
    return 1;
}

// True when the name-entry type can be opened.
int chkParam(s32 type, s32 param)
{
    s32 result = 1;

    switch (type) {
    case 0:
    case 1:
        break;
    case 2:
        if (!pokemonCheckValid(heroGetStatus(0, 3, (u16)param))) {
            result = 0;
        }
        break;
    case 3:
        if (param < 0 || param >= 8) {
            result = 0;
        }
        break;
    case 4:
        if (!pokemonCheckValid(fightFloorGetDefPokemonPtr(0))) {
            result = 0;
        }
        break;
    case 5:
    case 6:
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

// Open the name entry and return whether the name changed.
int menuNameEntryCall(s32 type, s32 param)
{
    NAME_ENTRY_WORK *work;

    if (chkParam(type, param) == 0) {
        return 0;
    }
    work = &nameEntryWork;
    work->mName[0] = 0;
    work->mType = type;
    work->mParam = param;
    work->mChanged = 0;
    work->mBlock = 0;
    menuNameEntry();
    return work->mChanged;
}

// Copy out the name being edited.
void menuNameEntryGetLastName(u16 *name)
{
    GScharCpy(name, nameEntryWork.mName);
}

// Push the name-entry floor.
int pushNameEntryFloor(s32 type, s32 param, s32 fade)
{
    NAME_ENTRY_WORK *work;

    if (chkParam(type, param) == 0) {
        return 0;
    }
    work = &nameEntryWork;
    work->mName[0] = 0;
    work->mType = type;
    work->mParam = param;
    work->mChanged = 0;
    work->mFade = fade;
    work->mBlock = 1;
    floorPush(NAME_ENTRY_FLOOR_ID);
    if (fade == 0) {
        floorSetFadeScript(0, 0x5960008);
    }
    floorWaitForChange();
    return work->mChanged;
}

// Link the name-entry floor.
int floorLink(s32 type, s32 param)
{
    return pushNameEntryFloor(type, param, 0);
}

// Open the name entry, hiding the menus around it.
int menuNameEntryOpen(s32 type, s32 param)
{
    unsigned long ids[4];
    s32 i;
    s32 result;

    ids[0] = nameEntryHideMenuIDs[0];
    ids[1] = nameEntryHideMenuIDs[1];
    ids[2] = nameEntryHideMenuIDs[2];
    ids[3] = nameEntryHideMenuIDs[3];
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i])) {
            menuSetDisp(ids[i], 0);
        }
    }
    result = pushNameEntryFloor(type, param, 1);
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i])) {
            menuSetDisp(ids[i], 1);
        }
    }
    return result;
}

// Static initializer of the menuNameEntry module, run once at startup through the
// .ctors table.
extern "C" void menuNameEntryStaticInit(void)
{
}

extern "C" __declspec(section ".ctors") void (*const menuNameEntryStaticInit_ctor)(void) = menuNameEntryStaticInit;
