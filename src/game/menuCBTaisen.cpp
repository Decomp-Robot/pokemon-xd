#include <menuPocketTool.h>
#include <heroItem.h>
#include <fade.h>
#include <savedata.h>
#include <heroBios.h>
#include <types.h>

#include <GSgfx.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <CAnt.h>
#include <winSprite.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <menuSub.h>
#include <textureBios.h>
#include <menuItemBios.h>
#include <menuCBBios.h>
#include <menuCBRule.h>
#include <menuPocket2.h>
extern "C" void *itemDataBiosGetPtr(u32 itemID);
extern "C" u32 itemDataBiosGetNameMsgID(void *itemData);
#include <menuPokemonStatus.h>
#include <menu.h>
#include <winMsg.h>
#include <GSthread.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <menuSub.h>

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
    void pickupDancer(Pokemon &pokemon, long slot);
};

extern "C" {
void *memcpy(void *dst, const void *src, u32 size);
double fmod(double x, double y);

s32 gameGetRegion(void);
void GSfsysReadWait(s32 group);
void GSfsysFree(s32 group);
void GSresFreeGroup(s32 group);
void GSsndPlay(s32 id, s32 a, s32 b);
void GSsndStop(s32 id, s32 a, s32 b);
void menuCB_MenuDispCheckONorOFF(tagWINDOW_WORK *window, bool on);
void spriteSetScissor(s32 layer, s32 x, s32 y, s32 w, s32 h);
void spriteClearScissor(s32 layer);
f32 timeGetLastFrameSec(void);
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
Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index);
u32 menuCBRule_RegulationItemID(s32 index);
u8 itemDataCheckValid(u32 itemID);
void *itemDataBiosGetPtr(u32 itemID);
void pokemonAllKaihuku(Pokemon *pokemon);
void battlePokemonGetPokeBody(CAnt *ant, Pokemon *pokemon);

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
void menuCBPokemonReturnToParty(void);
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
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].eggOrInvalidPokemon) {
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].levelBelowMinimum = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].levelAboveMaximum = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].partyLevelSumExceeded = 0;
        }

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].invalidPokemon) {
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].levelBelowMinimum = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].levelAboveMaximum = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].partyLevelSumExceeded = 0;
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].eggOrInvalidPokemon = 1;
        }

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].invalidHeldItem &&
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].heldItemRestricted)
            menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][i].heldItemRestricted = 0;
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
    } while ((windowGetKeyInfo()->mKeyTrg & 0x30) == 0);

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

void menuCBPokemonReturnToParty(void)
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
    menuCBEntrySlotConditionsBuild(&menuCBTaisenWork.mEntryHero, pokemon, sel);

    menuCBTaisenWork.entryNo = 0;
    if (menuCBEntrySlotConditionGet(sel, -1) != 0) {
        menuCBPokemonReturnSelected();
        menuCBTaisenWork.dispState = 2;

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].legendPokemon) {
            menuCBTaisenWork.pokeMsgID = 0x43AF;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].duplicateSpecies) {
            menuCBTaisenWork.pokeMsgID = 0x43A2;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].levelBelowMinimum || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].levelAboveMaximum) {
            menuCBTaisenWork.pokeMsgID = 0x43B0;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].shadowPokemon) {
            menuCBTaisenWork.pokeMsgID = 0x43A5;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].eggOrInvalidPokemon) {
            menuCBTaisenWork.pokeMsgID = 0x43A6;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidMove) {
            menuCBTaisenWork.pokeMsgID = 0x43A7;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].illegalPokemon || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidPokemon) {
            menuCBTaisenWork.pokeMsgID = 0x43A8;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].heldItemForbidden) {
            menuCBTaisenWork.pokeMsgID = 0x43A9;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].heldItemRestricted) {
            menuCBTaisenWork.pokeMsgID = 0x43AA;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].duplicateHeldItem) {
            menuCBTaisenWork.pokeMsgID = 0x43AB;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidHeldItem) {
            menuCBTaisenWork.pokeMsgID = 0x43AC;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].partyLevelSumExceeded) {
            menuCBTaisenWork.pokeMsgID = 0x43AD;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].insufficientPokemon) {
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
    return (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
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
        pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, SAVE_DATA_HERO),
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
        pokemon = heroBiosGetPokemonPtr(&menuCBTaisenWork.mEntryHero, (u16)index);
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
        MenuSpriteImage *image = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(0x216));
        s32 w = image->mWidth;
        s32 h = image->mHeight;

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
        menuPtSpriteDrawColor(0x214, 7, 0x23, color);
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
                u32 msgID = itemDataBiosGetNameMsgID(itemDataBiosGetPtr(itemID));

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
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelMin / 100;
        break;
    case 1:
        num = (menuCBTaisenWork.mEntryRule->mPokemonLevelMin % 100) / 10;
        break;
    case 2:
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelMin % 10;
        break;
    case 3:
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelMax / 100;
        break;
    case 4:
        num = (menuCBTaisenWork.mEntryRule->mPokemonLevelMax % 100) / 10;
        break;
    case 5:
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelMax % 10;
        break;
    case 6:
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelSumMax / 100;
        break;
    case 7:
        num = (menuCBTaisenWork.mEntryRule->mPokemonLevelSumMax % 100) / 10;
        break;
    case 8:
        num = menuCBTaisenWork.mEntryRule->mPokemonLevelSumMax % 10;
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
        if (menuCBTaisenWork.mEntryRule->mPokemonCountMode == 1) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = menuCBTaisenWork.mEntryRule->mPokemonCount % 10;
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
/* -------------------------------------------------------------------------
 * Entry-screen menus, rows and windows
 *
 * The link battle entry screens (the Pokemon the players bring) run on the
 * menu set menuCBPokemonEntryOpen/menuCBPokemonEntryClose open and close, and drive their windows
 * through the checks below. The action submenu (menuCBPokemonEntryActionSelect) and the item
 * submenu (menuCBPokemonEntryGiveItem) are the two ways a Pokemon is worked on; the entry
 * flow in the entry module runs their result.
 * ---------------------------------------------------------------------- */

extern "C" {
/* Entry-screen menu set and the windows' row checks. */
void menuCBPokemonEntryOpen(u8 load);
void menuCBPokemonEntryClose(u8 unload);
void menuCBRuleCursorWindowColor(tagWINDOW_WORK *window, s32 index);
void menuCBEntryCursorWindowColor(tagWINDOW_WORK *window, s32 index);
void menuCB_SpriteDispCheckONorOFF(tagSPRITE_WORK *sprite, bool on);

/* Submenus and the Pokemon work of the entry screens. */
void menuCBPokemonEntryTakeItem(void);
void menuCBPokemonEntryGiveItem(void);
void menuCBRuleWindowDisp(tagWINDOW_WORK *window, s32 trainer);
void menuCBPortTrainerNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCBPokemonEntryStatusOpen(void);
s32 menuCBPokemonEntryActionSelect(void);
void menuCBRuleSelect(void);
void menuCBPokemonEntryCheck(void);
void menuCBPokemonEntryDispPokemonFace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                       s32 arg, s32 slot);
u8 menuCBPokemonEntryDispPokemonFace_AFTER(tagWINDOW_WORK *window,
                                           tagSPRITE_WORK *sprite, s32 arg, s32 slot);
u8 menuCBPokemonEntryDispPokemonFace_BEFORE(tagWINDOW_WORK *window,
                                            tagSPRITE_WORK *sprite, s32 arg, s32 slot);

/* Entry-slot faces and the entry screens' checks. */
void menuCBPokemonEntryFacePlace(tagSPRITE_WORK *sprite, s32 port, u32 slot);
void menuCBPokemonEntryFaceDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot);
void menuCB_BattleEntryPokemonSelectDisp(tagSPRITE_WORK *sprite, s32 bank, s32 index);
s32 menuCB_GCStatusDispMode(tagSPRITE_WORK *sprite, s32 mode);
u8 menuCBPokemonEntryDisp(tagSPRITE_WORK *sprite);
void menuCBPokemonEntryGCControlDisp(tagSPRITE_WORK *sprite, s32 slot);
void menuCB2DPokemonSetSource(void);
void menuCBPokemonReturnSelected(void);
void menuCBPokemonReturnToParty(void);
u8 menuCBPokemonCheck(unsigned long bank, unsigned long index);
void menuCB_WaitGCControler1(void);
void menuCB_GBADisconnectedErrorJob(void);
void menuCBPokemonEntryAdd(void);
void menuCB2DPokemonSetPage(void);
void menuCBPCBOXTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 box,
                              s32 x);
void menuCBReliveTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID,
                               s32 x);
void menuCBPokemonEntryTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x);

/* The fight module's archive members: fightReadResource frees the member the
   group held before, reads the requested one in when it is not in memory yet
   and returns 1 when it had to, 0 when it was already there; fightFreeResource
   releases one member. */
u8 fightReadResource(u32 group, u32 addr);
void fightFreeResource(u32 group, u32 addr);

/* Pokemon nicknames. */
u16 *pokemon_GetNickname(Pokemon *pokemon);

/* Submenus and the entry screens' sources. */
FightRuleData *menuCBBios_CurrentRule(void);
u8 menuCBPokemonGetDataId(s32 port, s32 slot);
s32 menuCBBattleStartGetStatus(void);
u32 menuCBBios_GetPokemonNumLimit(void);
u16 toolentryTaisenGetPokemonNum(s32 port);
s32 toolentryTaisengetEtnryPokemonOrderNum(s32 port);
u16 toolentryTaisenGetPokemonHomePlace(s32 port);
u8 pauseCheck(void);
s32 menuCB_GetDisconnectedGBA(void);
u8 gbaCommandIsConnected(s32 port);
u8 GSinputIsConnected(s32 pad);
u16 *pcboxGetPokemonBoxName(PCBOX *pcbox, s8 box);

/* The two-entry message table the entry select falls back on once all six
   slots are filled: the second and third choice of submenu_msg1, the first
   one (0x4398) being unavailable while no slot is left. */
extern u32 submenu_msgFull[2];
}

void menuCBPokemonEntryOpen(u8 load)
{
    if (load != 0) {
        GSfsysReadWait(0x88C);
        menuCBBaseWindowOpen();
        menuCBFlashStartWait(0);
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
    }

    menuCBSEPlay(0x14);
    menuOpen(0xA9, 0);
    menuOpen(0xCE, 0);
    menuOpen(0xCB, 0);
    menuOpen(0xCC, 0);
    menuOpen(0xCD, 0);
    menuOpen(0xAA, 0);
    menuOpen(0xF7, 0);
}

void menuCBPokemonEntryClose(u8 unload)
{
    if (unload != 0)
        menuCBBaseWindowClose(1);

    menuCBSEPlay(0x15);
    menuCloseCustom(0xF7, 0, 0);
    menuCloseCustom(0xAA, 0, 0);
    menuCloseCustom(0xCD, 0, 0);
    menuCloseCustom(0xCC, 0, 0);
    menuCloseCustom(0xCB, 0, 0);
    menuCloseCustom(0xCE, 0, 0);
    menuCloseCustom(0xA9, 0, 1);

    if (unload != 0) {
        if (fightReadResource(0x842, 0x195F3400) == 0)
            fightFreeResource(0x842, 0x195F3400);

        GSfsysFree(0x88C);
        GSresFreeGroup(0x88C);
    }
}

void menuCBRuleCursorWindowColor(tagWINDOW_WORK *window, s32 index)
{
    if (menuCBTaisenWork.mRuleCursorPrev != -1) {
        menuSetDisp(window->mMenuID, 0);

        if (menuCBTaisenWork.mRuleCursorPrev == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)menuCBTaisenWork.mRuleCursorPrevColor;
        }

        if (menuCBTaisenWork.mRuleCursor == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)menuCBTaisenWork.mRuleCursorColor;
        }
    } else if (index == 1) {
        menuCB_MenuDispCheckONorOFF(window, menuCBTaisenWork.mRuleCursor == index ||
                                               menuCBTaisenWork.mRuleCursor == 5);
    } else {
        menuCB_MenuDispCheckONorOFF(window, menuCBTaisenWork.mRuleCursor == index);
    }
}

void menuCBEntryCursorWindowColor(tagWINDOW_WORK *window, s32 index)
{
    if (menuCBTaisenWork.mEntryCursorPrev != -1) {
        menuSetDisp(window->mMenuID, 0);

        if (menuCBTaisenWork.mEntryCursorPrev == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)menuCBTaisenWork.mEntryCursorPrevColor;
        }

        if (menuCBTaisenWork.mEntryCursor == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)menuCBTaisenWork.mEntryCursorColor;
        }
    } else {
        menuCB_MenuDispCheckONorOFF(window, menuCBTaisenWork.mEntryCursor == index);
    }
}

void menuCB_MenuDispCheckONorOFF(tagWINDOW_WORK *window, bool on)
{
    if (on != 0)
        menuSetDisp(window->mMenuID, 1);
    else
        menuSetDisp(window->mMenuID, 0);
}

void menuCB_SpriteDispCheckONorOFF(tagSPRITE_WORK *sprite, bool on)
{
    if (on != 0)
        winSpriteSetDisp(sprite, 1);
    else
        winSpriteSetDisp(sprite, 0);
}

void menuCBPokemonEntryTakeItem(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(menuCBTaisenWork.dstBank,
                                           menuCBTaisenWork.dstIndex);
    u16 *name;
    Hero *hero;
    u16 itemID;

    name = pokemon_GetNickname(pokemon);
    hero = &menuCBTaisenWork.mEntryHero;

    msgctrlSetValue(0x32, (unsigned long)name);

    itemID = pokemonGetSoubiItemDataId(pokemon);

    if (itemID != 0) {
        if (heroItemCheckAddItemDataId(hero, itemID) <= 0) {
            menuCBMsgPopup(0x3ACF);
        } else {
            u16 soubi;

            heroItemAddItemDataId(hero, itemID, 1, -1);
            soubi = pokemonDoItemSoubi(pokemon, 0, 0);
            msgctrlSetValue(0x2D, soubi);
            menuCBMsgPopup(0x3ACD);
        }
    } else {
        menuCBMsgPopup(0x3ACE);
    }
}

void menuCBPokemonEntryGiveItem(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(menuCBTaisenWork.dstBank,
                                           menuCBTaisenWork.dstIndex);
    Hero *hero;
    u16 select;
    u8 pocketSlot;
    u16 itemID;
    u16 oldItem;
    s32 answer;
    u16 *name;
    s32 slot;

    name = pokemon_GetNickname(pokemon);
    hero = &menuCBTaisenWork.mEntryHero;

    msgctrlSetValue(0x32, (unsigned long)name);

    if ((s32)menuCBBios_CurrentRule()->mHeldItemMode == HELD_ITEMS_FORBIDDEN) {
        _menuCB_errorMsgDisp(0x43A9);
        return;
    }

    menuOpen(0xE6, 1);
    select = menuPocket2Call(5, 0, hero);
    pocketSlot = menuPocket2GetLastSlotID();
    menuCloseCustom(0xE6, 0, 1);

    if (select == 0)
        return;

    slot = (u16)menuCBTaisenWork.dstIndex - menuCBEntrySlotFirst;

    menuCBEntrySlotFlagsClear(0, slot);

    oldItem = pokemonDoItemSoubi(pokemon, 0, 0);
    pokemonDoItemSoubi(pokemon, select, 1);
    menuCBEntrySlotConditionsSetItemRegulation(hero, pokemon, slot);
    pokemonDoItemSoubi(pokemon, 0, 0);
    pokemonDoItemSoubi(pokemon, oldItem, 1);

    if (menuCBEntrySlotConditionGet(slot, 9) != 0) {
        _menuCB_errorMsgDisp(0x43AA);
        return;
    }

    if (menuCBEntrySlotConditionGet(slot, 0xA) != 0) {
        _menuCB_errorMsgDisp(0x43AB);
        return;
    }

    if (menuCBEntrySlotConditionGet(slot, 0xB) != 0) {
        _menuCB_errorMsgDisp(0x43AC);
        return;
    }

    itemID = pokemonGetSoubiItemDataId(pokemon);

    if (itemID != 0) {
        msgctrlSetValue(0x2D, itemID);
        winMsgOpen(2, 0x3ACA, 1, 0);

        switch (gameGetRegion()) {
        case 0:
            answer = menuSubOpenYesNo(0, 0x3C, 0xD2, 0);
            break;
        case 1:
        case 2:
            answer = menuSubOpenYesNo(0, 0x3C, 0xC8, 0);
            break;
        }

        winMsgClose(1);

        if (answer != 0)
            return;

        if (heroItemDecItemDataId(hero, select, 1, pocketSlot) != 0) {
            menuCBMsgPopup(0x3ACF);
            return;
        }

        if (heroItemCheckAddItemDataId(hero, itemID) <= 0) {
            heroItemAddItemDataId(hero, select, 1, -1);
            menuCBMsgPopup(0x3ACF);
        } else {
            heroItemAddItemDataId(hero, itemID, 1, -1);

            oldItem = pokemonDoItemSoubi(pokemon, 0, 0);
            pokemonDoItemSoubi(pokemon, select, 1);
            msgctrlSetValue(0x2D, oldItem);
            msgctrlSetValue(0x2E, select);
            menuCBMsgPopup(0x3ACB);
        }
    } else {
        if (heroItemDecItemDataId(hero, select, 1, pocketSlot) != 0)
            return;

        pokemonDoItemSoubi(pokemon, select, 1);
        msgctrlSetValue(0x2D, select);
        menuCBMsgPopup(0x3ACC);
    }
}

void menuCBRuleWindowDisp(tagWINDOW_WORK *window, s32 trainer)
{
    u16 name[0x1F];
    s32 i;

    if (trainer >= 3) {
        msgctrlSetValue(0x37, (unsigned long)&menuCBTaisenWork.mRulesetData[trainer].m1E);
    } else {
        const u16 *src;

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

    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x4275);

    if (menuCBTaisenWork.mRuleCursor == 5)
        msgctrlSetValue(0x34, 2);
    else
        msgctrlSetValue(0x34, menuCBTaisenWork.mRuleCursor + 1);

    msgctrlSetValue(0x2F, menuCBTaisenWork.mSlotNo);

    GSmsgPrint2(window->mLayerID, 0x1CE, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
    GSmsgPrint2(window->mLayerID, 0x1E4, 2, window->mRgba.mColorA | 0xFFFFFF00, 0x4417);

    if (menuCBTaisenWork.mEntrySelect != -1)
        i = 5;
    else
        i = 4;

    msgctrlSetValue(0x34, i);
    msgctrlSetValue(0x2F, i);

    GSmsgPrint2(window->mLayerID, 0x1FA, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
}

void menuCBPortTrainerNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer)
{
    msgctrlSetValue(0x37, (unsigned long)&menuCBTaisenWork.mRulesetData[trainer].m1E);

    GSmsgPrint2(window->mLayerID, 0, 0,
                (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA, 0x4277);
}

void menuCBPokemonEntryStatusOpen(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(menuCBTaisenWork.dstBank,
                                           menuCBTaisenWork.dstIndex);

    menuCBBattleMenuClose(0);
    menuPokemonStatusOpenFromPokemonChange(pokemon, 0, 0);
    menuCBBattleMenuOpen(0);
}

s32 menuCBPokemonEntryActionSelect(void)
{
    u16 index;

    if (menuCBTaisenWork.mSubmenuConfirm == 1) {
        winMsgOpen(2, 0x43A1, 1, 0);

        switch (gameGetRegion()) {
        case 0:
            menuCBTaisenWork.selectResult = menuSubOpenYesNo(0, 0x3C, 0xD2, 0);
            break;
        case 1:
        case 2:
            menuCBTaisenWork.selectResult = menuSubOpenYesNo(0, 0x3C, 0xC8, 0);
            break;
        }

        if (menuCBTaisenWork.selectResult == -1)
            menuCBTaisenWork.selectResult = 1;

        winMsgClose(0);
        return menuCBTaisenWork.selectResult;
    }

    menuCB2DPokemonSetSource();

    index = (u16)menuCBTaisenWork.dstIndex;

    if (index >= menuCBEntrySlotFirst && index <= menuCBEntrySlotLast) {
        menuCBTaisenWork.selectResult =
            menuSubOpenSelect2(0, submenu_msg2, 4, 0x28, 0x64, 0, 4, 1);

        if (menuCBTaisenWork.selectResult == -1)
            menuCBTaisenWork.selectResult = 3;

        switch (menuCBTaisenWork.selectResult) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            winMsgOpen(2, 0x43A0, 0, 0);
            menuCBTaisenWork.selectResult =
                menuSubOpenSelect2(0, submenu_msg3, 3, 0x28, 0x64, 0, 3, 1);

            if (menuCBTaisenWork.selectResult == -1)
                menuCBTaisenWork.selectResult = 2;

            switch (menuCBTaisenWork.selectResult) {
            case 0:
                winMsgClose(0);
                return 4;
            case 1:
                return 5;
            case 2:
                winMsgClose(0);
                return 6;
            }
        case 3:
            return 6;
        }
    } else {
        s16 x = 0x104;
        s16 y = 0xC8;

        switch (menuCBTaisenWork.battleMode2D) {
        case 1:
            x = submenu1_x[index];
            y = submenu1_y[index];
            break;
        case 0:
        case 2:
        case 3:
            break;
        }

        if (menuCBTaisenWork.selectCount == 6) {
            menuCBTaisenWork.selectResult =
                menuSubOpenSelect2(0, submenu_msgFull, 2, x, y, 0, 2, 1);

            if (menuCBTaisenWork.selectResult == -1)
                menuCBTaisenWork.selectResult = 1;
        } else {
            menuCBTaisenWork.selectResult =
                menuSubOpenSelect2(0, submenu_msg1, 3, x, y, 0, 3, 1);

            if (menuCBTaisenWork.selectResult == -1)
                menuCBTaisenWork.selectResult = 2;
        }

        if (menuCBTaisenWork.selectCount == 6) {
            switch (menuCBTaisenWork.selectResult) {
            case 0:
                return 2;
            case 1:
                return 6;
            }
        } else {
            switch (menuCBTaisenWork.selectResult) {
            case 0:
                return 0;
            case 1:
                return 2;
            case 2:
                return 6;
            }
        }
    }

    return 6;
}

void menuCBRuleSelect(void)
{
    switch (menuCBTaisenWork.mRuleNo) {
    case 0:
    case 1:
    case 2:
        menuCBTaisenWork.selectResult =
            menuSubOpenSelect2(0, submenurule_msg1, 3, 0x28, 0xF4,
                               menuCBTaisenWork.selectResult, 3, 1);

        if (menuCBTaisenWork.selectResult == -1)
            menuCBTaisenWork.selectResult = 2;
        break;

    case 3:
    case 4:
    case 5:
        menuCBTaisenWork.selectResult =
            menuSubOpenSelect2(0, submenurule_msg2, 4, 0x28, 0xD4,
                               menuCBTaisenWork.selectResult, 4, 1);

        if (menuCBTaisenWork.selectResult == -1)
            menuCBTaisenWork.selectResult = 3;

        menuCBTaisenWork.selectResult += 0x10;
        break;
    }

    switch (menuCBTaisenWork.selectResult) {
    case 0:
    case 0x10:
        menuCBTaisenWork.mRuleSelect = 0;
        break;
    case 2:
    case 0x13:
        menuCBTaisenWork.mRuleSelect = 1;
        break;
    }
}

void menuCBPokemonEntryCheck(void)
{
    Hero *hero;
    u32 slot;

    for (u32 i = 0; i < 6; i++)
        menuCBEntrySlotFlagsClear(0, i);

    hero = &menuCBTaisenWork.mEntryHero;

    menuCBEntrySlotConditionsBuildAll(hero, 0);

    for (slot = 0; slot < 6; slot++) {
        if (menuCBEntrySlotConditionGet(slot, 8) != 0) {
            menuCBTaisenWork.pokeMsgID = 0x43A9;
            menuCBTaisenWork.dispState = 2;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, 9) != 0) {
            menuCBTaisenWork.mSlotNo = slot + 1;
            menuCBTaisenWork.pokeMsgID = 0x4410;
            menuCBTaisenWork.dispState = 2;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, 0xA) != 0) {
            menuCBTaisenWork.pokeMsgID = 0x43AB;
            menuCBTaisenWork.dispState = 2;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, 0xB) != 0) {
            menuCBTaisenWork.pokeMsgID = 0x43AC;
            menuCBTaisenWork.dispState = 2;
            return;
        }
    }

    for (u32 i = 0; i < 6; i++)
        menuCBEntrySlotFlagsClear(0, i);

    menuCBEntrySlotConditionsSetTrainerRegulation(hero);

    if (menuCBEntrySlotConditionGet(0, -1) != 0) {
        menuCBTaisenWork.dispState = 2;

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][0].insufficientPokemon)
            menuCBTaisenWork.pokeMsgID = 0x43AE;
    }
}

void menuCB_WaitGCControler1(void)
{
    if (GSinputIsConnected(1) == 0) {
        s32 msgID;

        if (gbaCommandIsConnected(1) != 0)
            msgID = 0x442D;
        else
            msgID = 0x4436;

        winMsgOpen(2, msgID, 0, 0);

        while (GSinputIsConnected(1) == 0)
            GSthreadSwitch();

        winMsgClose(1);
    }
}

void menuCB_GBADisconnectedErrorJob(void)
{
    s32 port = menuCB_GetDisconnectedGBA();

    switch (port) {
    case 0:
        if (gbaCommandIsConnected(1) != 0) {
            winMsgOpen(2, 0x442D, 0, 0);

            while (GSinputIsConnected(1) == 0)
                GSthreadSwitch();
        } else {
            winMsgOpen(2, 0x442C, 1, 0);
        }
        break;

    case 1:
        winMsgOpen(2, 0x4438, 0, 0);

        while (GSinputIsConnected(1) == 0)
            GSthreadSwitch();
        break;

    default:
        if (gbaCommandIsConnected(1) != 0) {
            winMsgOpen(2, 0x442D, 0, 0);

            while (GSinputIsConnected(1) == 0)
                GSthreadSwitch();
        } else {
            msgctrlSetValue(0x2F, port);
            winMsgOpen(2, 0x442B, 1, 0);
        }
        break;
    }

    winMsgClose(1);
    menuCB_WaitGCControler1();
}

void menuCBPokemonEntryGCControlDisp(tagSPRITE_WORK *sprite, s32 slot)
{
    if (menuCBPokemonEntryDisp(sprite) != 0 && menuCBPokemonGetDataId(0, slot) == 0)
        winSpriteSetDisp(sprite, 0);
}

void menuCBPokemonEntryFacePlace(tagSPRITE_WORK *sprite, s32 port, u32 slot)
{
    u16 limit = (u16)menuCBBios_GetPokemonNumLimit();
    u16 num = toolentryTaisenGetPokemonNum(port);
    u16 bound;

    menuCBTaisenWork.mPartySlotX[port][slot - 1] = sprite->mX;

    if (menuCBTaisenWork.mPartySlotFlag[port][slot - 1] == 0)
        menuCBTaisenWork.mPartySlotValue[port][slot - 1] = 0x32;

    if (num < limit)
        bound = num;
    else
        bound = limit;

    if (slot > (u16)bound)
        winSpriteSetDisp(sprite, 0);
    else
        winSpriteSetDisp(sprite, 1);
}

void menuCBPokemonEntryFaceDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot)
{
    u16 limit = (u16)menuCBBios_GetPokemonNumLimit();
    u16 num = toolentryTaisenGetPokemonNum(port);
    s32 order = (u16)toolentryTaisengetEtnryPokemonOrderNum(port);
    u16 bound;
    s16 x;

    if (num < limit)
        bound = num;
    else
        bound = limit;

    if (slot > bound || slot > order) {
        if (pauseCheck()) {
            if (menuCBTaisenWork.mPartySlotFlag[port][slot - 1] == 0)
                winSpriteSetDisp(sprite, 0);
            else
                winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
            menuCBTaisenWork.mPartySlotFlag[port][slot - 1] = 0;
        }
        return;
    }

    if (pauseCheck()) {
        if (menuCBTaisenWork.mPartySlotFlag[port][slot - 1] != 1) {
            winSpriteSetDisp(sprite, 0);
            menuCBTaisenWork.mPartySlotFlag[port][slot - 1] = 0;
            return;
        }
    }

    menuCBTaisenWork.mPartySlotFlag[port][slot - 1] = 1;

    x = menuCBTaisenWork.mPartySlotX[port][slot - 1] +
        menuCBTaisenWork.mPartySlotValue[port][slot - 1];
    sprite->mX = x;

    if (menuCBTaisenWork.mPartySlotValue[port][slot - 1] != 0)
        menuCBTaisenWork.mPartySlotValue[port][slot - 1] -= 5;

    winSpriteSetDisp(sprite, 1);

    if (menuCBTaisenWork.mTrainer[port].mEntryList[slot - 1] == 0xF)
        winSpriteSetDisp(sprite, 0);
}

u8 menuCBPokemonEntryDispPokemonFace_AFTER(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                            s32 arg, s32 slot)
{
    if (menuCBBattleStartGetStatus() != 0) {
        menuCBPokemonEntryDispPokemonFace(window, sprite, arg, slot);
        return 1;
    }

    return 0;
}

u8 menuCBPokemonEntryDispPokemonFace_BEFORE(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                            s32 arg, s32 slot)
{
    if (menuCBBattleStartGetStatus() == 0) {
        menuCBPokemonEntryDispPokemonFace(window, sprite, arg, slot);
        return 1;
    }

    return 0;
}

void menuCB_BattleEntryPokemonSelectDisp(tagSPRITE_WORK *sprite, s32 bank, s32 index)
{
    menuCB_SpriteDispCheckONorOFF(sprite, menuCBPokemonCheck(bank, index) == 0);
}

s32 menuCB_GCStatusDispMode(tagSPRITE_WORK *sprite, s32 mode)
{
    if (menuCBTaisenWork.battleMode == 2) {
        if (mode == 0) {
            winSpriteSetDisp(sprite, 1);
            return 1;
        }

        winSpriteSetDisp(sprite, 0);
        return 0;
    }

    if (mode == 1) {
        winSpriteSetDisp(sprite, 1);
        return 1;
    }

    winSpriteSetDisp(sprite, 0);
    return 0;
}

u8 menuCBPokemonEntryDisp(tagSPRITE_WORK *sprite)
{
    u8 disp = 1;
    s32 home = toolentryTaisenGetPokemonHomePlace(0);

    switch (home) {
    case 0:
        winSpriteSetDisp(sprite, 1);
        break;
    default:
        winSpriteSetDisp(sprite, 0);
        disp = 0;
        break;
    }

    return disp;
}

void menuCB2DPokemonSetSource(void)
{
    switch (menuCBTaisenWork.battleMode2D) {
    case 0:
        menuCBTaisenWork.dstBank = menuCBPokemonHeroParty;
        menuCBTaisenWork.dstIndex = menuCBTaisenWork.pokeSelect;
        return;

    case 1:
        menuCBTaisenWork.dstBank = menuCBPokemonPcBox;
        menuCBTaisenWork.dstIndex = (u16)menuCBTaisenWork.pokeSelect |
                                    ((menuCBTaisenWork.page2D << 16) & 0xFFFF0000);
        return;

    case 2:
        menuCBTaisenWork.dstBank = menuCBPokemonReliveHall;
        menuCBTaisenWork.dstIndex = (u16)menuCBTaisenWork.pokeSelect |
                                    ((menuCBTaisenWork.page2DSub << 16) & 0xFFFF0000);
        return;

    case 3:
        menuCBTaisenWork.dstBank = menuCBPokemonWorkParty;
        menuCBTaisenWork.dstIndex = menuCBTaisenWork.pokeSelect;
        return;
    }
}

void menuCBPokemonReturnSelected(void)
{
    s32 select = menuCBTaisenWork.pokeSelect;

    switch (menuCBTaisenWork.selectCount) {
    case 1:
        menuCBTaisenWork.pokeSelect = 0x1E;
        break;
    case 2:
        menuCBTaisenWork.pokeSelect = 0x1F;
        break;
    case 3:
        menuCBTaisenWork.pokeSelect = 0x20;
        break;
    case 4:
        menuCBTaisenWork.pokeSelect = 0x21;
        break;
    case 5:
        menuCBTaisenWork.pokeSelect = 0x22;
        break;
    case 6:
        menuCBTaisenWork.pokeSelect = 0x23;
        break;
    }

    menuCBPokemonReturnToParty();
    menuCBTaisenWork.pokeSelect = select;
}

void menuCBPokemonEntryAdd(void)
{
    u32 bank;
    u32 index;
    Pokemon *src;
    Pokemon *pokemon;

    menuCB2DPokemonSetSource();

    if (menuCBPokemonCheck(menuCBTaisenWork.dstBank, menuCBTaisenWork.dstIndex) == 0)
        return;

    if (menuCBTaisenWork.pokeSelect >= menuCBEntrySlotFirst) {
        menuCBPokemonReturnToParty();
        return;
    }

    if (menuCBTaisenWork.selectCount == 6)
        return;

    menuCB2DPokemonSetSource();

    bank = menuCBTaisenWork.dstBank;
    index = menuCBTaisenWork.dstIndex;

    backup_bank[menuCBTaisenWork.selectCount] = bank;
    backup_index[menuCBTaisenWork.selectCount] = index;

    pokemon = menuCBPokemonGetPtr(menuCBPokemonWorkParty, menuCBTaisenWork.selectCount);

    switch (gameGetRegion()) {
    case 0:
        if (bank != menuCBPokemonReliveHall) {
            src = menuCBPokemonGetPtr(bank, index);

            memcpy(pokemon, src, sizeof(Pokemon));
            pokemonInit(src);
        } else if (menuCBTaisenWork.pokeSelect == 4) {
            backup_bank[menuCBTaisenWork.selectCount] = 0;
            backup_index[menuCBTaisenWork.selectCount] = 0;
            menuCBTaisenWork.dispState = 2;
            menuCBTaisenWork.pokeMsgID = 0x43A5;
            return;
        } else {
            CReliveStage *stage = CReliveHall::getPointer()->getStage(index >> 16);

            stage->pickupDancer(*pokemon, index & 0xFFFF);
        }
        break;

    case 1:
    case 2:
        if (bank == menuCBPokemonReliveHall && menuCBTaisenWork.pokeSelect == 4) {
            backup_bank[menuCBTaisenWork.selectCount] = 0;
            backup_index[menuCBTaisenWork.selectCount] = 0;
            menuCBTaisenWork.dispState = 2;
            menuCBTaisenWork.pokeMsgID = 0x43A5;
            return;
        }

        src = menuCBPokemonGetPtr(bank, index);

        memcpy(pokemon, src, sizeof(Pokemon));
        pokemonInit(src);
        break;
    }

    menuCBTaisenWork.selectCount++;

    if (menuCBPokemonMsgCheck(bank, menuCBTaisenWork.selectCount + 0x1D) != 0 &&
        menuCBTaisenWork.selectCount == 6)
        menuCBTaisenWork.pokeSelect = 0x25;
}

void menuCB2DPokemonSetPage(void)
{
    s32 base;
    s32 page;
    u32 i;

    page = menuCBTaisenWork.page2DSub;

    if (menuCBTaisenWork.pokeSelect > 4)
        return;

    base = (page << 16) & 0xFFFF0000;

    if (menuCBPokemonCheck(menuCBPokemonReliveHall, menuCBTaisenWork.pokeSelect | base) != 0)
        return;

    for (i = 0; i < 5; i++) {
        if (menuCBPokemonCheck(menuCBPokemonReliveHall, i | base) != 0) {
            menuCBTaisenWork.pokeSelect = (s8)i;
            break;
        }
    }

    if (menuCBTaisenWork.pokeSelect != i)
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst;
}

void menuCBPCBOXTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 box, s32 x)
{
    s16 x2;
    s16 width;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(0x37, (unsigned long)pcboxGetPokemonBoxName(PCBOX::getPointer(), (s8)box));

    width = (s16)(GSmsgGetMessageSize(0x4277) >> 16);
    x2 = (s16)(sprite->mWidth / 2 - width / 2);
    x2 += x;
    GSmsgPrint2(window->mLayerID, x2, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x4277);

    spriteClearScissor(window->mLayerID);
}

void menuCBReliveTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID,
                               s32 x)
{
    s16 width;
    s16 x2;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(msgID));

    width = (s16)(GSmsgGetMessageSize(0x4277) >> 16);
    x2 = (s16)(sprite->mWidth / 2 - width / 2);
    x2 += x;
    GSmsgPrint2(window->mLayerID, x2, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x4277);

    spriteClearScissor(window->mLayerID);
}

void menuCBPokemonEntryTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x)
{
    s16 width;
    s16 x2;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(0x37, (unsigned long)heroBiosGetNamePtr(
                              &menuCBTaisenWork.mEntryHero));

    width = (s16)(GSmsgGetMessageSize(msgID) >> 16);
    x2 = (s16)(sprite->mWidth / 2 - width / 2);
    x2 += x;
    GSmsgPrint2(window->mLayerID, x2, 0, window->mRgba.mColorA | 0xFFFFFF00, msgID);

    spriteClearScissor(window->mLayerID);
}

u8 menuCBPokemonCheck(unsigned long bank, unsigned long index)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank, index);

    if (pokemon != 0 && (u8)pokemonCheckValid(pokemon) != 0)
        return 1;

    return 0;
}
