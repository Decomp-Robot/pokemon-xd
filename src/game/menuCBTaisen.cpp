#include <types.h>

#include <GSgfx.h>
#include <Pokemon.h>
#include <CAnt.h>
#include <winSprite.h>
#include <menuItemBios.h>
#include <menuCBBios.h>

/*
 * menuCBTaisen.cpp
 *
 * Callbacks shared by the link battle (Taisen) entry screens: the menus that
 * show the trainers and their Pokemon while the battle is being set up. The
 * menu system (menuOpen/menuCloseCustom) drives those screens through the
 * hooks in this file, which draw the messages, the trainer names, the
 * condition marks and the status numbers of the windows and animate the 2D
 * Pokemon models.
 *
 * The Pokemon themselves are read from wherever they currently live -- the
 * trainer parties, the hero party, the PC box or the Relive hall -- through
 * menuCBPokemonGetPtr, whose bank argument selects the source.
 */

/* -------------------------------------------------------------------------
 * Engine and library entry points
 * ---------------------------------------------------------------------- */

class PCBOX {
public:
    static PCBOX *getPointer();
    Pokemon *getPokemon(signed char box, signed char slot);
};

class CReliveStage;

class CReliveHall {
public:
    static CReliveHall *getPointer();
    CReliveStage *getStage(long stage);
};

class CReliveStage {
public:
    Pokemon *getVisitorPointer();
    Pokemon *getDancerPointer(long slot);
    void insertDancer(Pokemon &pokemon, long slot);
};

extern "C" {
void GSthreadSwitch(void);
void *memcpy(void *dst, const void *src, u32 size);
double fmod(double x, double y);

s32 gameGetRegion(void);
void GSfsysReadWait(s32 group);
void GSfsysFree(s32 group);
void GSresFreeGroup(s32 group);
void fadeSet(s32 kind, f32 value);
void GSsndPlay(s32 id, s32 a, s32 b);
void GSsndStop(s32 id, s32 a, s32 b);
void GSmsgPrint2(s32 layer, s32 x, s32 y, u32 color, s32 msgID);
u32 GSmsgGetMessageSize(s32 msgID); // (width << 16) | height
u16 *GSmsgGetStringPtr(s32 msgID);
void msgctrlSetValue(s32 id, unsigned long value);
s32 menuOpen(s32 menuID, s32 arg);
s32 menuCloseCustom(s32 menuID, s32 arg, s32 flag);
void menuSetDisp(s32 menuID, s32 on);
void menuCB_MenuDispCheckONorOFF(s32 a, s32 on);
struct tagWINDOW_KEY *windowGetKeyState(void);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void spriteSetScissor(s32 layer, s32 x, s32 y, s32 w, s32 h);
void spriteClearScissor(s32 layer);
// Draw texture `textureID` at its own size and the given window colour.
void menuPtSpriteDrawTexture(long textureID, long x, long y, u32 color);
f32 timeGetLastFrameSec(void);
// Texture bios: the table of texture `id`, and its entry for the current
// language. The entry +4/+6 halfwords are the texture width/height.
s16 *textureBiosGetDataTable(s32 id, u32 color);
s16 *textureBiosGetLanguageEntry(s16 *table);
// Fade alpha the menu windows are drawn with.
u32 animAlpha(void);
// The full-screen base window (menu 0x119) the entry screens open on entry
// and close on exit: a single item, a 640x480 sprite, which the flash start
// drops to window level 0 so it stays under the screen's other windows.
void menuCBBaseWindowOpen(void);
// Start the connect battle screen flash, at its end frame unless
// playFromStart; waits until the flash system reports its last frame.
void menuCBFlashStartWait(s32 playFromStart);
void menuCBBaseWindowClose(s32 close);
// Source bank/index the 2D Pokemon screen takes its selected Pokemon from.
void menuCB2DPokemonSetSource(void);
// Return the Pokemon the 2D screen has selected to its party.
void menuCBPokemonReturnSelected(void);
// One condition bit of one entry slot; the index selects which condition.
u8 menuCBEntrySlotConditionGet(s32 slot, s32 condition);
// Drop every condition of one entry slot before its Pokemon is re-checked.
void menuCBEntrySlotFlagsClear(s32 entry, s32 slot);
// Run the current rule set's checks over `src` against the party based at
// `party` and record them as the conditions of entry slot `index`.
void menuCBEntrySlotConditionsBuild(Pokemon *party, Pokemon *src, s32 index);
Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index);
s32 getTrainerDataStartOffset(s32 a, s32 b);
Pokemon *heroBiosGetPokemonPtr(long base, u16 slot);
u32 itemDataBiosGetDescriptionId(void *itemData);
u32 menuCBRule_RegulationItemID(s32 index);
u8 itemDataCheckValid(u32 itemID);
void *itemDataBiosGetPtr(u32 itemID);
int pokemonCheckValid(Pokemon *pokemon);
int pokemonGetStatus(Pokemon *pokemon, s32 a, s32 b, s32 c);
void pokemonAllKaihuku(Pokemon *pokemon);
void battlePokemonGetPokeBody(CAnt *ant, Pokemon *pokemon);

void winMsgOpen(s32 mode, u32 msgID, s32 a, s32 b);
void winMsgClose(s32 id);
}

void menuPtSpriteDrawColor(unsigned short id, long w, long h, long x, long y,
                           unsigned long color);

/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/* Bank/index pairs the entry screens park Pokemon in while they are moved
   around; menuCBPokemonReturnToParty compacts them. */
extern u32 backup_bank[6];
extern u32 backup_index[6];

/* 2D Pokemon model states of the entry screens: the three rows of CAnt models
   the screens animate. */
CAnt gEntryAntRowFirst[36];
CAnt gEntryAntRowSecond[30];
CAnt gEntryAntRowThird[30];

/* Item lists of the submenus the entry screens open: the choice message IDs of
   the rule submenu and of the three entry submenus, and the item positions of
   the first of them. */
#pragma force_active on
u32 submenurule_msg1[3] = { 0x4391, 0x4392, 0x4395 };
u32 submenurule_msg2[4] = { 0x4391, 0x4393, 0x4394, 0x4395 };
static u32 submenu_msg1[3] = { 0x4398, 0x4399, 0x439A };
static u32 submenu_msg2[4] = { 0x439B, 0x4399, 0x439C, 0x439A };
static u32 submenu_msg3[3] = { 0x439D, 0x439E, 0x439A };
/* Item positions of the first submenu, indexed with the cursor: its x table
   holds every item at 0x28, and its y table the first eighteen at 0x136 and
   the remaining twelve at 0x64. */
static s16 submenu1_x[30] = {
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
};
static s16 submenu1_y[30] = {
    0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136,
    0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x136, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
};
/* Alpha the rule cursor mark fades through: 0xFF down to 0x80 and back. */
static u8 _ruleCursolDispColorATbl[64] = {
    0xFF, 0xFB, 0xF7, 0xF3, 0xEF, 0xEA, 0xE6, 0xE2,
    0xDE, 0xD9, 0xD5, 0xD1, 0xCD, 0xC8, 0xC4, 0xC0,
    0xBC, 0xB8, 0xB3, 0xAF, 0xAB, 0xA7, 0xA2, 0x9E,
    0x9A, 0x96, 0x91, 0x8D, 0x89, 0x85, 0x80, 0x80,
    0x85, 0x89, 0x8D, 0x91, 0x96, 0x9A, 0x9E, 0xA2,
    0xA7, 0xAB, 0xAF, 0xB3, 0xB8, 0xBC, 0xC0, 0xC4,
    0xC8, 0xCD, 0xD1, 0xD5, 0xD9, 0xDE, 0xE2, 0xE6,
    0xEA, 0xEF, 0xF3, 0xF7, 0xFB, 0xFF, 0xFF, 0xFF,
};
#pragma force_active reset

/* -------------------------------------------------------------------------
 * Pokemon sources
 * ---------------------------------------------------------------------- */

/* Where the entry screens read a Pokemon from; the value is the bank their
   accessors switch on. */
enum {
    menuCBPokemonLinkEntry  = 0, // 0..3: the party the link cable player on
                                 //       that port entered.
    menuCBPokemonHeroParty  = 4, // the hero party as the save holds it.
    menuCBPokemonPcBox      = 5, // the hero's PC boxes (box << 16 | slot).
    menuCBPokemonReliveHall = 6, // the Relive hall (stage << 16 | slot).
    menuCBPokemonWorkParty  = 7, // the party copy in menuCBTaisenWork.
};

/* The entry slots of that copy, in the hero party slot numbering the hero
   bios accessor uses: menuCBPokemonGetPtr resolves menuCBEntrySlotFirst up to
   menuCBEntrySlotLast into the copy and refuses an index from menuCBSlotCount
   on. */
enum {
    menuCBEntrySlotFirst = 0x1E,
    menuCBEntrySlotLast  = 0x23,
    menuCBSlotCount      = 0x24,
};

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

extern "C" {
void menuCB2DItemMsgDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u32 menuCBRegionMsgID(u32 msgID);
s32 menuCBRegionMsgIDValid(s32 msgID);
void menuCBEntryFlagsReset(void);
void menuCB2DPokemonPageScroll(unsigned char key);
void menuCBMsgPopup(unsigned long msgID);
void menuCBBattleMenuOpen(u32 load);
void menuCBBattleMenuClose(u32 unload);
void menuCBPokemonReturnToParty(unsigned long);
u8 menuCBPokemonMsgCheck(unsigned long bank, unsigned long index);
Pokemon *menuCBPokemonGetPtr(unsigned long bank, unsigned long index);
void menuCBDrawStringCenter(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 *str,
                            long x, long y, unsigned long msgID);
void menuCBSEPlay(unsigned long seID);
void menuCBSpriteAlphaSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag);
void menuCBWindowColorSave(tagWINDOW_WORK *window);
void menuCBAccessFlashDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned char flag);
void menuCBTrainerNameDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCBSpriteColorSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag);
void menuCBSpriteColorIfMatch(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 value);
void menuCBTrainerMarksDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCBRegulationItemDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCBStatusNumDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 kind);
void menuCBRegionMsgDrawOrSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID);
void menuCBMsgDrawOnSprite(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned long msgID);
void menuCBMessageYCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID);
void menuCB_GCPokemonHPandWAZA_MAX(void);
}

void _menuCB2DPokemonNewSet2(unsigned long bank, unsigned long page,
                             unsigned long count, unsigned char mode);
void _menuCB_errorMsgDisp(unsigned long msgID);
u8 _menuCBPokemonEntryGetlevel(Pokemon *pokemon);
unsigned long menuCBPokemonGetIndex(unsigned long menuID, unsigned long index);
void _menuTitleSetSE(unsigned long seID);
void _drawStrings(tagWINDOW_WORK *window, u16 *str, long x, long y,
                  unsigned long msgID);

/* Frame countdowns the entry screens arm for a sprite fade, at 30 and 135
   frames, and run down one per unpaused frame. The fade itself only starts on
   the frame the first one reaches zero, the frame it also plays SE 0x5D0; the
   second one stops that SE once it is down to 55 frames. */
u16 menuCBFadeSoundCount;
u16 menuCBFadeSoundStopCount;

/* Phase of the access flash the entry screens show while loading. */
f32 menuCBAccessFlashPhase = 0.0f;

/* Constants of that flash: the length of one blink cycle, the unity the
   mirrored phase is folded down from, the gain that turns the folded phase
   into the flash strength, the period the accumulated phase wraps at, and the
   strength to pixel size scale of the marks. */
extern const f32 menuCBAccessFlashCyclePeriod;
extern const f32 menuCBAccessFlashUnity;
extern const f32 menuCBAccessFlashGain;
extern const f64 menuCBAccessFlashWrapPeriod;
extern const f32 menuCBAccessFlashSizeScale;

/* Draw the message some fixed items of the entry screens show, or queue it
   on the sprite. */
void menuCB2DItemMsgDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case 0x2B7:
        menuCBMsgDrawOnSprite(window, sprite, 0xD11E);
        break;
    case 0x2B8:
        menuCBMsgDrawOnSprite(window, sprite, 0xD11F);
        break;
    case 0x4D1:
        sprite->mMsgID = 0xD160;
        break;
    case 0x433:
        menuCBMsgDrawOnSprite(window, sprite, 0x3BD6);
        break;
    }
}

u32 menuCBRegionMsgID(u32 msgID)
{
    switch (gameGetRegion()) {
    case 0:
        return msgID;

    case 1:
    case 2:
        switch (msgID) {
        case 0x4278:
            return 0x4279;
        case 0x4276:
            return 0x4277;
        case 0x427C:
            return 0x4352;
        case 0x427B:
            return 0x4351;
        }
        break;
    }

    return 0x56C;
}

s32 menuCBRegionMsgIDValid(s32 msgID)
{
    if (msgID >= 0x176)
        return msgID;
    if (msgID < 0x164)
        return msgID;
    return 0;
}

void menuCBEntryFlagsReset(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].marked) {
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43b0 = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43b0b = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43ad = 0;
        }

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43a8b) {
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43b0 = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43b0b = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43ad = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].marked = 1;
        }

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].msg43ac &&
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].pending)
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].pending = 0;
    }
}

void _menuCB2DPokemonNewSet2(unsigned long bank, unsigned long page,
                             unsigned long count, unsigned char mode)
{
    /* Row the screens show: 0 is the +36 row, 1 the +66 row, 2 the +0 row. */
    CAnt *row36 = gEntryAntRowSecond;
    CAnt *row66 = gEntryAntRowThird;
    CAnt *row00 = gEntryAntRowFirst;

    unsigned long row = mode;
    Pokemon *pokemon;
    unsigned long i;

    for (i = 0; i < count; i++) {
        pokemon = menuCBPokemonGetPtr(bank, page | i);

        if (pokemon != 0 && (u8)pokemonCheckValid(pokemon) != 0) {
            switch (row) {
            case 0:
                battlePokemonGetPokeBody(row36, pokemon);
                row36->setAnimation(0);
                break;
            case 1:
                battlePokemonGetPokeBody(row66, pokemon);
                row66->setAnimation(0);
                break;
            case 2:
                battlePokemonGetPokeBody(row00, pokemon);
                row00->setAnimation(0);
                break;
            }
        }

        row36++;
        row66++;
        row00++;
    }
}

/* Page the 2D Pokemon screens when the cursor leaves a page edge. */
void menuCB2DPokemonPageScroll(unsigned char key)
{
    if (key == 2) {
        _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
        return;
    }

    long lastPage2D = 7;

    switch (menuCBTaisenWork.battleMode2D) {
    case 0:
        switch (key) {
        case 0:
            _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0, 0x1E, key);
            break;
        case 1:
            if (menuCBTaisenWork.pageWrap == 1)
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall, 0x80000, 5, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0x70000, 0x1E, key);
            break;
        }
        break;

    case 1:
        switch (key) {
        case 0:
            if (menuCBTaisenWork.page2D == lastPage2D) {
                if (menuCBTaisenWork.pageWrap == 1)
                    _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall, 0, 5, key);
                else
                    _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            } else {
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox,
                                       (menuCBTaisenWork.page2D + 1) << 16,
                                       0x1E, key);
            }
            break;
        case 1:
            if (menuCBTaisenWork.page2D == 0)
                _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox,
                                       (menuCBTaisenWork.page2D - 1) << 16,
                                       0x1E, key);
            break;
        }
        break;

    case 2:
        switch (key) {
        case 0:
            if (menuCBTaisenWork.page2DSub == 8)
                _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall,
                                       (menuCBTaisenWork.page2DSub + 1) << 16,
                                       5, key);
            break;
        case 1:
            if (menuCBTaisenWork.page2DSub == 0)
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0x70000, 0x1E, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall,
                                       (menuCBTaisenWork.page2DSub - 1) << 16,
                                       5, key);
            break;
        }
        break;
    }
}

/* -------------------------------------------------------------------------
 * Message helpers
 * ---------------------------------------------------------------------- */

/* Show message msgID in the message window without waiting for it. */
void menuCBMsgPopup(unsigned long msgID)
{
    winMsgOpen(2, msgID, 1, 0);
    winMsgClose(1);
}

/* Show message msgID in the message window and wait for a button press. */
void _menuCB_errorMsgDisp(unsigned long msgID)
{
    winMsgOpen(2, msgID, 1, 1);

    do {
        GSthreadSwitch();
    } while ((windowGetKeyState()->mKeyTrg & 0x30) == 0);

    winMsgClose(1);
}

void menuCBBattleMenuOpen(u32 load)
{
    if (load == 0) {
        GSfsysReadWait(0x88C);
        menuCBBaseWindowOpen();
        menuCBFlashStartWait(0);
        fadeSet(2, menuCBAccessFlashCyclePeriod);
    }

    menuCBSEPlay(0x14);
    menuOpen(0xBB, 0);
    menuOpen(0xBD, 0);
    menuOpen(0xBE, 0);
    menuOpen(0xBF, 0);
    menuOpen(0x10D, 0);
    menuOpen(0x10F, 0);
    menuOpen(0x10E, 0);
    menuOpen(0xC2, 0);
}

void menuCBBattleMenuClose(u32 unload)
{
    if (unload == 0)
        menuCBBaseWindowClose(1);

    menuCBSEPlay(0x15);
    menuCloseCustom(0xBC, 0, 0);
    menuCloseCustom(0xC2, 0, 0);
    menuCloseCustom(0x10E, 0, 0);
    menuCloseCustom(0x10F, 0, 0);
    menuCloseCustom(0x10D, 0, 0);
    menuCloseCustom(0xBF, 0, 0);
    menuCloseCustom(0xBE, 0, 0);
    menuCloseCustom(0xBD, 0, 0);
    menuCloseCustom(0xBB, 0, 1);

    if (unload == 0) {
        GSfsysFree(0x88C);
        GSresFreeGroup(0x88C);
    }
}

void menuCBPokemonReturnToParty(unsigned long)
{
    u32 sel;
    u32 index;
    Pokemon *dst;
    Pokemon *next;

    switch (menuCBTaisenWork.pokeSelect) {
    case menuCBEntrySlotFirst:
        sel = 0;
        break;
    case menuCBEntrySlotFirst + 1:
        sel = 1;
        break;
    case menuCBEntrySlotFirst + 2:
        sel = 2;
        break;
    case menuCBEntrySlotFirst + 3:
        sel = 3;
        break;
    case menuCBEntrySlotFirst + 4:
        sel = 4;
        break;
    case menuCBEntrySlotLast:
        sel = 5;
        break;
    }

    {
        {
            u32 bank = backup_bank[sel];

            index = backup_index[sel];

            backup_bank[sel] = 0;
            backup_index[sel] = 0;
            menuCB2DPokemonSetSource();

            dst = menuCBPokemonGetPtr(menuCBTaisenWork.dstBank, menuCBTaisenWork.dstIndex);
            s32 region = gameGetRegion();

            switch (region) {
            case 0:
                if (bank != menuCBPokemonReliveHall) {
                    next = menuCBPokemonGetPtr(bank, index);

                    memcpy(next, dst, sizeof(Pokemon));
                    pokemonInit(dst);
                } else {
                    CReliveStage *stage =
                        CReliveHall::getPointer()->getStage(index >> 16);
                    stage->insertDancer(*dst, 0);
                }
                break;
            case 1:
            case 2: {
                next = menuCBPokemonGetPtr(bank, index);

                memcpy(next, dst, sizeof(Pokemon));
                pokemonInit(dst);
                break;
            }
            }
        }

        for (; sel < 5; sel++) {
            Pokemon *slot = menuCBPokemonGetPtr(menuCBPokemonWorkParty, sel);

            backup_bank[sel] = backup_bank[sel + 1];
            backup_index[sel] = backup_index[sel + 1];
            backup_bank[sel + 1] = 0;
            backup_index[sel + 1] = 0;

            next = menuCBPokemonGetPtr(menuCBPokemonWorkParty, sel + 1);
            memcpy(slot, next, sizeof(Pokemon));
            pokemonInit(next);

        }
    }

    menuCBTaisenWork.selectCount--;
}

u8 menuCBPokemonMsgCheck(unsigned long bank, unsigned long index)
{
    s32 sel = menuCBTaisenWork.selectCount - 1;
    Pokemon *pokemon = menuCBPokemonGetPtr(bank, index);

    menuCBEntrySlotFlagsClear(0, sel);
    menuCBEntrySlotConditionsBuild(&menuCBTaisenWork.mParty[0], pokemon, sel);

    menuCBTaisenWork.entryNo = 0;
    if (menuCBEntrySlotConditionGet(sel, -1) != 0) {
        menuCBPokemonReturnSelected();
        menuCBTaisenWork.dispState = 2;

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43af) {
            menuCBTaisenWork.pokeMsgID = 0x43AF;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a2) {
            menuCBTaisenWork.pokeMsgID = 0x43A2;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43b0 || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43b0b) {
            menuCBTaisenWork.pokeMsgID = 0x43B0;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a5) {
            menuCBTaisenWork.pokeMsgID = 0x43A5;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].marked) {
            menuCBTaisenWork.pokeMsgID = 0x43A6;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a7) {
            menuCBTaisenWork.pokeMsgID = 0x43A7;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a8 || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a8b) {
            menuCBTaisenWork.pokeMsgID = 0x43A8;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43a9) {
            menuCBTaisenWork.pokeMsgID = 0x43A9;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].pending) {
            menuCBTaisenWork.pokeMsgID = 0x43AA;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43ab) {
            menuCBTaisenWork.pokeMsgID = 0x43AB;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43ac) {
            menuCBTaisenWork.pokeMsgID = 0x43AC;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43ad) {
            menuCBTaisenWork.pokeMsgID = 0x43AD;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].msg43ae) {
            menuCBTaisenWork.pokeMsgID = 0x43AE;
            return 0;
        }
        return 0;
    }

    return 1;
}

/* Level of pokemon, as the entry screens read it. */
u8 _menuCBPokemonEntryGetlevel(Pokemon *pokemon)
{
    return pokemonGetStatus(pokemon, 0, 0x7A, 0);
}

unsigned long menuCBPokemonGetIndex(unsigned long menuID, unsigned long index)
{
    unsigned long ret = index;

    switch (menuID) {
    case menuCBPokemonHeroParty:
        break;

    case menuCBPokemonPcBox:
        ret = (unsigned short)index;
        break;

    case menuCBPokemonReliveHall:
        ret = (unsigned short)index;
        break;

    case menuCBPokemonWorkParty:
        ret = index + menuCBEntrySlotFirst;
        break;

    default:
        ret = index;
        break;
    }

    return ret;
}

Pokemon *menuCBPokemonGetPtr(unsigned long bank, unsigned long index)
{
    u16 boxSlot;
    u32 box;
    Pokemon *pokemon;
    u16 slot = (u16)index;

    if (slot >= menuCBSlotCount)
        return 0;

    if (slot >= menuCBEntrySlotFirst && slot <= menuCBEntrySlotLast) {
        bank = menuCBPokemonWorkParty;
        index = slot - menuCBEntrySlotFirst;
    }

    switch (bank) {
    case menuCBPokemonHeroParty:
        pokemon = heroBiosGetPokemonPtr(getTrainerDataStartOffset(0, 2),
                                        (u16)index);
        break;

    case menuCBPokemonPcBox:
        pokemon = PCBOX::getPointer()->getPokemon((signed char)(index >> 16),
                                                  (signed char)(u16)index);
        break;

    case menuCBPokemonReliveHall: {
        CReliveHall *hall = CReliveHall::getPointer();
        box = index >> 16;
        CReliveStage *stage = hall->getStage(box);
        boxSlot = (u16)index;

        if (boxSlot == 4) {
            pokemon = stage->getVisitorPointer();
        } else {
            s32 region = gameGetRegion();

            switch (region) {
            case 0:
                pokemon = stage->getDancerPointer(boxSlot);
                break;
            case 1:
            case 2:
                pokemon = menuCBTaisenWork.reliveDancers[box][boxSlot];
                break;
            }
        }
        break;
    }

    case menuCBPokemonWorkParty:
        pokemon = heroBiosGetPokemonPtr((long)&menuCBTaisenWork.mParty[0], (u16)index);
        break;

    case menuCBPokemonLinkEntry:
    case 1:
    case 2:
    case 3:
        pokemon = toolentryTaisenGetPokemonPtr(bank, index);
        break;

    default:
        break;
    }

    return pokemon;
}

void menuCBDrawStringCenter(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 *str,
                            long x, long y, unsigned long msgID)
{
    s32 width;

    msgctrlSetValue(0x37, (unsigned long)str);
    width = (s16)(GSmsgGetMessageSize(msgID) >> 16);
    GSmsgPrint2(window->mLayerID, (s16)(sprite->mWidth / 2 - width / 2), y,
                window->mRgba.mColorA | 0xFFFFFF00, msgID);
}

/* Print str at x/y; the message msgID picks the frame drawn around it. */
void _drawStrings(tagWINDOW_WORK *window, u16 *str, long x, long y,
                  unsigned long msgID)
{
    msgctrlSetValue(0x37, (unsigned long)str);
    GSmsgPrint2(window->mLayerID, x, y, window->mRgba.mColorA | 0xFFFFFF00, msgID);
}

/* -------------------------------------------------------------------------
 * Sound
 * ---------------------------------------------------------------------- */

/* Stop the sound effect seID. */
void _menuTitleSetSE(unsigned long seID)
{
    GSsndStop(seID, 0, 0);
}

/* Play the screen sound effect seID. */
void menuCBSEPlay(unsigned long seID)
{
    GSsndPlay(seID, 0, 0);
}

/* Alpha the sprite is drawn with, from the saved window colour. */
void menuCBSpriteAlphaSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag)
{
    if (flag != 0) {
        if (window->mRgba.mColorA == 0xFF)
            sprite->mRgba.mAlpha = menuCBTaisenWork.savedColor;
        else
            sprite->mRgba.mAlpha = window->mRgba.mColorA;
    } else {
        if ((u32)window->mRgba.mColorA >= 0x7F)
            sprite->mRgba.mAlpha = 0x7F;
        else
            sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

void menuCBWindowColorSave(tagWINDOW_WORK *window)
{
    if (window->mRgba.mColorA == 0xFF)
        menuCBTaisenWork.savedColor = animAlpha() & 0xFF;
    else
        menuCBTaisenWork.savedColor = window->mRgba.mColorA;
}

/* -------------------------------------------------------------------------
 * Access flash
 * ---------------------------------------------------------------------- */

/* Blink the access marks around the sprite while the disc is being read. */
void menuCBAccessFlashDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned char flag)
{
    if (flag != 0) {
        f32 phase = menuCBAccessFlashPhase;
        f32 tri;
        s32 size;

        phase /= menuCBAccessFlashCyclePeriod;

        if (phase > menuCBAccessFlashCyclePeriod)
            phase = menuCBAccessFlashUnity - phase;
        tri = menuCBAccessFlashGain * phase;

        menuCBAccessFlashPhase = menuCBAccessFlashPhase + timeGetLastFrameSec();
        if (menuCBAccessFlashPhase >= menuCBAccessFlashCyclePeriod)
            menuCBAccessFlashPhase =
                fmod(menuCBAccessFlashPhase, menuCBAccessFlashWrapPeriod);

        u32 color = window->mRgba.mColorA | 0xFFFFFF00;
        s16 *tex = textureBiosGetLanguageEntry(
            textureBiosGetDataTable(0x216, window->mRgba.mColorA));
        s32 w = tex[2];
        s32 h = tex[3];

        size = (s16)(menuCBAccessFlashSizeScale * tri);
        if (size > 4)
            size = 4;

        menuPtSpriteDrawColor(0x216, (s16)(size + 4), (s16)(size + 4), w, h, color);
        menuPtSpriteDrawColor(0x216, (s16)(0x32 - size), (s16)(size + 4), -w, h,
                              color);
        menuPtSpriteDrawColor(0x216, (s16)(size + 4), (s16)(0x32 - size), w, -h,
                              color);
        menuPtSpriteDrawColor(0x216, (s16)(0x32 - size), (s16)(0x32 - size), -w,
                              -h, color);
        menuPtSpriteDrawTexture(0x214, 7, 0x23, color);
    }
}

/* Heal every Pokemon of the party copy: HP, status and moves. */
void menuCB_GCPokemonHPandWAZA_MAX(void)
{
    u32 i;

    for (i = 0; i < 6; i++) {
        Pokemon *pokemon = menuCBPokemonGetPtr(menuCBPokemonHeroParty, i);

        if (pokemon != 0)
            pokemonAllKaihuku(pokemon);
    }
}

void menuCBTrainerNameDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer)
{
    u16 name[0x1F];
    u32 msgID;
    s16 x;

    if (trainer >= 3) {
        msgctrlSetValue(0x37,
                        (unsigned long)(menuCBTaisenWork.mRulesetData[trainer].m1E));
    } else {
        const u16 *src;
        s32 i;

        switch (trainer) {
        case 0:
            src = GSmsgGetStringPtr(0x4387);
            break;
        case 1:
            src = GSmsgGetStringPtr(0x4389);
            break;
        case 2:
            src = GSmsgGetStringPtr(0x4388);
            break;
        default:
            break;
        }

        for (i = 0; i < 0x1F; i++) {
            if (src != 0) {
                if (*src == 0xFFFF)
                    src += 2;
                name[i] = *src;
                if (*src != 0)
                    src++;
            } else {
                name[i] = 0;
            }
        }

        msgctrlSetValue(0x37, (unsigned long)name);
    }

    msgID = menuCBRegionMsgID(0x4276);
    x = sprite->mWidth / 2 - (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2;
    GSmsgPrint2(window->mLayerID, x, 0,
                (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA,
                menuCBRegionMsgID(0x4276));
}

void menuCBSpriteColorSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag)
{
    if (flag != 0) {
        sprite->mRgba.mBlue = 0xFF;
        sprite->mRgba.mGreen = 0xFF;
        sprite->mRgba.mRed = 0xFF;
        sprite->mRgba.mAlpha = menuCBTaisenWork.savedColor;
    } else {
        sprite->mRgba.mBlue = 0x7F;
        sprite->mRgba.mGreen = 0x7F;
        sprite->mRgba.mRed = 0x7F;
        sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

/* menuCBSpriteColorSet, but white only while value is the selected entry. */
void menuCBSpriteColorIfMatch(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 value)
{
    if (value == menuCBTaisenWork.selectedValue) {
        sprite->mRgba.mBlue = 0xFF;
        sprite->mRgba.mGreen = 0xFF;
        sprite->mRgba.mRed = 0xFF;
        sprite->mRgba.mAlpha = menuCBTaisenWork.savedColor;
    } else {
        sprite->mRgba.mBlue = 0x7F;
        sprite->mRgba.mGreen = 0x7F;
        sprite->mRgba.mRed = 0x7F;
        sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

void menuCBTrainerMarksDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 layer = winSpriteGetLayerID(window, sprite);
    tagMENU_ITEM_DATA *itemTop = menuItemBiosGetPtr(window->mMenuID, 0x3AB);
    tagMENU_ITEM_DATA *itemRow = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 row = menuCBTaisenWork.listRow;
    s16 y = -(row * 0x18);
    s32 first;
    s32 last;
    s32 i;

    if (row == 0) {
        first = 0;
    } else {
        first = row - 1;
        y = y + first * 0x18;
    }

    last = first + 0xD;
    if (last >= 0x3E)
        last = 0x3E;

    spriteSetScissor(layer, itemTop->mMessageX - itemRow->mMessageX,
                     itemTop->mOffsetA - itemRow->mOffsetA, itemRow->mOffsetB,
                     itemRow->mOffsetC);

    for (i = first; i <= last; i++, y += 0x18) {
        if (i == 0x3E) {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        1);
        } else if (menuCBTaisenWork.mEntryRule->m52[i] != 0) {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFF3F0000,
                        0x43E4);
        } else {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        0x43E3);
        }
    }

    spriteClearScissor(layer);
}

void menuCBRegulationItemDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 layer = winSpriteGetLayerID(window, sprite);
    tagMENU_ITEM_DATA *itemTop = menuItemBiosGetPtr(window->mMenuID, 0x3AC);
    tagMENU_ITEM_DATA *itemRow = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 row = menuCBTaisenWork.listRow;
    s16 y = -(row * 0x18);
    s32 first;
    s32 last;
    u32 itemID;

    if (row == 0) {
        first = 0;
    } else {
        first = row - 1;
        y = y + first * 0x18;
    }

    last = first + 0xD;
    if (last >= 0x3E)
        last = 0x3E;

    spriteSetScissor(layer, itemTop->mMessageX - itemRow->mMessageX,
                     itemTop->mOffsetA - itemRow->mOffsetA, itemRow->mOffsetB,
                     itemRow->mOffsetC);

    for (s32 i = first; i <= last; i++, y += 0x18) {
        itemID = menuCBRule_RegulationItemID(i);

        if ((u16)itemID == 0) {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        0x4345);
        } else {
            if (itemDataCheckValid(itemID)) {
                u32 msgID = itemDataBiosGetDescriptionId(itemDataBiosGetPtr(itemID));

                msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(msgID));
            }

            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        menuCBRegionMsgID(0x4276));
        }
    }

    spriteClearScissor(layer);
}

void menuCBStatusNumDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 kind)
{
    s16 x;
    s32 num;
    u32 msgID;

    switch (kind) {
    case 0:
        num = menuCBTaisenWork.mEntryRule->m00 / 100;
        break;
    case 1:
        num = (menuCBTaisenWork.mEntryRule->m00 % 100) / 10;
        break;
    case 2:
        num = menuCBTaisenWork.mEntryRule->m00 % 10;
        break;
    case 3:
        num = menuCBTaisenWork.mEntryRule->m02 / 100;
        break;
    case 4:
        num = (menuCBTaisenWork.mEntryRule->m02 % 100) / 10;
        break;
    case 5:
        num = menuCBTaisenWork.mEntryRule->m02 % 10;
        break;
    case 6:
        num = menuCBTaisenWork.mEntryRule->m04 / 100;
        break;
    case 7:
        num = (menuCBTaisenWork.mEntryRule->m04 % 100) / 10;
        break;
    case 8:
        num = menuCBTaisenWork.mEntryRule->m04 % 10;
        break;

    case 9: {
        FightRuleData *rule = menuCBTaisenWork.mEntryRule;

        if (rule->m14 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = rule->m14 / 10;
        break;
    }
    case 10:
        if (menuCBTaisenWork.mEntryRule->m14 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = menuCBTaisenWork.mEntryRule->m14 % 10;
        break;
    case 11:
        if (menuCBTaisenWork.mEntryRule->m16 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = menuCBTaisenWork.mEntryRule->m16 / 10;
        break;
    case 12:
        if (menuCBTaisenWork.mEntryRule->m16 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = menuCBTaisenWork.mEntryRule->m16 % 10;
        break;

    case 13:
        if (menuCBTaisenWork.mEntryRule->m1C == 1) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = menuCBTaisenWork.mEntryRule->m1A % 10;
        break;
    }

    msgctrlSetValue(0x34, num);
    msgctrlSetValue(0x2F, num);
    msgID = menuCBRegionMsgID(0x427B);
    x = (s16)(sprite->mWidth / 2 - (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2);
    GSmsgPrint2(window->mLayerID, x, 0, window->mRgba.mColorA | 0xFFFFFF00,
                menuCBRegionMsgID(0x427B));
}

/* Draw message msgID centred on the sprite, both axes. */
void menuCBMessageYCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID)
{
    GSmsgPrint2(window->mLayerID, 0,
                (s16)(sprite->mHeight / 2 - (s16)(u16)GSmsgGetMessageSize(msgID) / 2),
                window->mRgba.mColorA | 0xFFFFFF00, msgID);
}

void menuCBRegionMsgDrawOrSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID)
{
    s32 region = gameGetRegion();

    switch (region) {
    case 0:
        sprite->mMsgID = msgID;
        break;
    case 1:
    case 2:
        menuCBMsgDrawOnSprite(window, sprite, msgID);
        break;
    }
}

void menuCBMsgDrawOnSprite(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned long msgID)
{
    u32 color = (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA;

    GSmsgPrint2(window->mLayerID,
                (s16)(sprite->mWidth / 2 - (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2),
                0, color, msgID);
}