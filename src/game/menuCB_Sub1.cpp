#include <GSmsg.h>
#include <menu.h>
#include <itemData.h>
#include <menuCB_Sub1.h>
#include <menuBBS.h>
#include <menuPocketTool.h>
#include <winMsg.h>
#include <winSpriteDisplay.h>
#include <types.h>
#include <savedata.h>
#include <textureBios.h>

#include <GSgfx.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <waza.h>
#include <zokusei.h>
#include <pokemonBios.h>
#include <CAnt.h>
#include <winSprite.h>
#include <menuItemBios.h>
#include <menuCBBios.h>
#include <hero.h>
#include <heroBios.h>
#include <heroItem.h>
#include <menuPocket2.h>
#include <menuSub.h>
#include <msgctrl.h>
#include <menuPokemonMessages.h>

enum {
    menuCBDisplayEntryError = 2,
    menuCBMsgProhibitedSpecies = 0x43AF,
    menuCBMsgDuplicateSpecies = 0x43A2,
    menuCBMsgLevelOutsideAllowedRange = 0x43B0,
    menuCBMsgShadowPokemonNotAllowed = 0x43A5,
    menuCBMsgEggNotAllowed = 0x43A6,
    menuCBMsgUnknownMove = 0x43A7,
    menuCBMsgInvalidPokemonData = 0x43A8,
    menuCBMsgHeldItemNotAllowed = 0x43A9,
    menuCBMsgProhibitedHeldItem = 0x43AA,
    menuCBMsgDuplicateHeldItem = 0x43AB,
    menuCBMsgIllegalHeldItem = 0x43AC,
    menuCBMsgCombinedLevelTooHigh = 0x43AD,
    menuCBMsgIncorrectEntryCount = 0x43AE,
    menuCBMsgConfirmHeldItemReplacement = 0x3ACA,
    menuCBMsgHeldItemReplaced = 0x3ACB,
    menuCBMsgHeldItemGiven = 0x3ACC,
    menuCBMsgHeldItemTaken = 0x3ACD,
    menuCBMsgNoHeldItem = 0x3ACE,
    menuCBMsgItemTransferFailed = 0x3ACF,
    menuCBMsgEmptyMoveSlot = 0x3AF4,
    menuCBMsgAttackLabel = 0x43F5,
    menuCBMsgDefenseLabel = 0x43F6,
    menuCBMsgSpecialAttackLabel = 0x43F7,
    menuCBMsgSpecialDefenseLabel = 0x43F8,
    menuCBMsgSpeedLabel = 0x43F9,
    menuCBMsgEggName = 0x56B,
    menuCBMsgInvalidPokemonName = 0x56C,
    menuCBMsgCompactSexMark = 0x4278,
    menuCBMsgCompactPokemonName = 0x4279,
    menuCBMsgLargePanelNumber = 0x427B,
    menuCBMsgPanelNumber = 0x427C,
    menuCBMsgRegionalLargePanelNumber = 0x4351,
    menuCBMsgRegionalPanelNumber = 0x4352,
    menuCBMsgAbilityLabel = 0x435A,
    menuCBMsgRegionalAbilityLabel = 0x445B,
    menuCBMsgHPLabel = 0x43EA,
    menuCBMsgLevelLabel = 0x43E9,
    menuCBMsgLargeHPLabel = 0x4397,
    menuCBMsgLargeLevelLabel = 0x4396,
};

/*
 * menuCB_Sub1.cpp
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
s32 menuOpen(s32 menuID, s32 arg);
void menuCB_MenuDispCheckONorOFF(tagWINDOW_WORK *window, bool on);
struct tagWINDOW_KEY *windowGetKeyInfo(void);
void spriteSetScissor(s32 layer, s32 x, s32 y, s32 w, s32 h);
void spriteClearScissor(s32 layer);
// Draw texture `textureID` at its own size and the given window colour.
f32 timeGetLastFrameSec(void);
// Texture bios: the table of texture `id`, and its entry for the current
// language. The entry +4/+6 halfwords are the texture width/height.

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
// `hero` and record them as the conditions of entry slot `index`.
void menuCBEntrySlotConditionsBuild(Hero *hero, Pokemon *src, s32 index);
Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index);
u16 menuCBRule_RegulationItemID(s32 index);
u8 itemDataCheckValid(u16 itemID);
void pokemonAllKaihuku(Pokemon *pokemon);
void battlePokemonGetPokeBody(CAnt *ant, Pokemon *pokemon);

}


/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/* Bank/index pairs the entry screens park Pokemon in while they are moved
   around; _menuCBGCDelPokemon compacts them. */
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

void menuCB2DPokemonPageScroll(unsigned char key);
void menuCBBattleMenuOpen(unsigned long load);
void menuCBBattleMenuClose(unsigned long unload);
u8 menuCBPokemonMsgCheck(unsigned long bank, unsigned long index);
void _menuCB2DPokemonNewSet2(unsigned long bank, unsigned long page,
                             unsigned long count, unsigned char mode);
void _menuCB_errorMsgDisp(unsigned long msgID);
u8 _menuCBPokemonEntryGetlevel(Pokemon *pokemon);
unsigned long menuCBPokemonGetIndex(unsigned long menuID, unsigned long index);
extern "C" void menuCB_SE_Stop(unsigned long seID);
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

static const u32 battleXPTextures[4] = {527,526,525,524};
static const u32 battleHPBarTextures[3] = {696,697,698};
static const u32 battleFacePanelTextures[2] = {563,564};

/* Blink timing and strength of the loading-access indicator. */
static const f32 menuCBAccessFlashCyclePeriod = 0.5f;
static const f32 menuCBAccessFlashUnity = 1.0f;
static const f32 menuCBAccessFlashGain = 2.0f;
static const f64 menuCBAccessFlashWrapPeriod = 0.5;
static const f32 menuCBAccessFlashSizeScale = 5.0f;

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
        case menuCBMsgCompactSexMark:
            return menuCBMsgCompactPokemonName;
        case MSG_SEX_MARK:
            return MSG_POKEMON_NAME;
        case menuCBMsgPanelNumber:
            return menuCBMsgRegionalPanelNumber;
        case menuCBMsgLargePanelNumber:
            return menuCBMsgRegionalLargePanelNumber;
        }
        break;
    }

    return menuCBMsgInvalidPokemonName;
}

long _menuCB_NotDarkWaza(long wazaID)
{
    if (wazaID >= 0x176)
        return wazaID;
    if (wazaID < 0x164)
        return wazaID;
    return 0;
}

void _menuCB_ErrorTAMAGOCheck(void)
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
void _menuCB_errorMsgDisp2(unsigned long msgID)
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

void menuCBBattleMenuOpen(unsigned long load)
{
    if (load == 0) {
        GSfsysReadWait(0x88C);
        menuCBBaseWindowOpen();
        menuCBFlashStartWait(0);
        fadeSet(2, 0.5f);
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

void menuCBBattleMenuClose(unsigned long unload)
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

void _menuCBGCDelPokemon(void)
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
    menuCBEntrySlotConditionsBuild(&menuCBTaisenWork.mHero, pokemon, sel);

    menuCBTaisenWork.entryNo = 0;
    if (menuCBEntrySlotConditionGet(sel, MENU_CB_ENTRY_CONDITION_ANY) != 0) {
        menuCBPokemonReturnSelected();
        menuCBTaisenWork.dispState = menuCBDisplayEntryError;

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].legendPokemon) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgProhibitedSpecies;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].duplicateSpecies) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgDuplicateSpecies;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].levelBelowMinimum || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].levelAboveMaximum) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgLevelOutsideAllowedRange;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].shadowPokemon) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].eggOrInvalidPokemon) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgEggNotAllowed;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidMove) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgUnknownMove;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].illegalPokemon || menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidPokemon) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgInvalidPokemonData;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].heldItemForbidden) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgHeldItemNotAllowed;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].heldItemRestricted) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgProhibitedHeldItem;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].duplicateHeldItem) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgDuplicateHeldItem;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].invalidHeldItem) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgIllegalHeldItem;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].partyLevelSumExceeded) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgCombinedLevelTooHigh;
            return 0;
        }
        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][sel].insufficientPokemon) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgIncorrectEntryCount;
            return 0;
        }
        return 0;
    }

    return 1;
}

/* Level of pokemon, as the entry screens read it. */
u8 _menuCBPokemonEntryGetlevel(Pokemon *pokemon)
{
    return pokemonGetStatus(pokemon, 0, POKEMON_QUERY_LEVEL, 0);
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
        pokemon = heroBiosGetPokemonPtr((Hero *)savedataGetStatus(0, 2),
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
        pokemon = heroBiosGetPokemonPtr(&menuCBTaisenWork.mHero, (u16)index);
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

    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)str);
    width = (s16)(GSmsgGetMessageSize(msgID) >> 16);
    GSmsgPrint2(window->mLayerID, (s16)(sprite->mWidth / 2 - width / 2), y,
                window->mRgba.mColorA | 0xFFFFFF00, msgID);
}

/* Print str at x/y; the message msgID picks the frame drawn around it. */
void _drawStrings(tagWINDOW_WORK *window, u16 *str, long x, long y,
                  unsigned long msgID)
{
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)str);
    GSmsgPrint2(window->mLayerID, x, y, window->mRgba.mColorA | 0xFFFFFF00, msgID);
}

/* -------------------------------------------------------------------------
 * Sound
 * ---------------------------------------------------------------------- */

/* Stop the sound effect seID. */
void menuCB_SE_Stop(unsigned long seID)
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
        msgctrlSetValue(MSGCTRL_TEXT,
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

        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)name);
    }

    msgID = menuCBRegionMsgID(MSG_SEX_MARK);
    x = sprite->mWidth / 2 - (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2;
    GSmsgPrint2(window->mLayerID, x, 0,
                (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA,
                menuCBRegionMsgID(MSG_SEX_MARK));
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
    u16 itemID;

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

                msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(msgID));
            }

            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        menuCBRegionMsgID(MSG_SEX_MARK));
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

    msgctrlSetValue(MSGCTRL_NUM_PANEL, num);
    msgctrlSetValue(MSGCTRL_NUM_INPUT, num);
    msgID = menuCBRegionMsgID(menuCBMsgLargePanelNumber);
    x = (s16)(sprite->mWidth / 2 - (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2);
    GSmsgPrint2(window->mLayerID, x, 0, window->mRgba.mColorA | 0xFFFFFF00,
                menuCBRegionMsgID(menuCBMsgLargePanelNumber));
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

/* The condition checks of the entry module: the party-wide trainer regulation,
   the conditions a held item gives one slot, and the rebuild of every slot of
   one entry. */
void menuCBEntrySlotConditionsSetTrainerRegulation(Hero *hero);
void menuCBEntrySlotConditionsSetItemRegulation(Hero *hero, Pokemon *pokemon, s32 slot);
void menuCBEntrySlotConditionsBuildAll(Hero *hero, s32 entry);

/* Held item accessors and the bag of the work party. */
u16 *pokemon_GetNickname(Pokemon *pokemon);

/* Item pocket, submenus and the entry screens' sources. */
FightRuleData *menuCBBios_CurrentRule(void);
u8 menuCBPokemonGetDataId(s32 port, s32 slot);
s32 menuCBBattleStartGetStatus(void);
void menuPokemonStatusOpenFromPokemonChange(Pokemon *pokemon, s32 a, s32 b);
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
    hero = &menuCBTaisenWork.mHero;

    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)name);

    itemID = pokemonGetSoubiItemDataId(pokemon);

    if ((u16)itemID != 0) {
        if (heroItemCheckAddItemDataId(hero, itemID) <= 0) {
            _menuCB_errorMsgDisp2(menuCBMsgItemTransferFailed);
        } else {
            u16 soubi;

            heroItemAddItemDataId(hero, itemID, 1, -1);
            soubi = pokemonDoItemSoubi(pokemon, 0, 0);
            msgctrlSetValue(MSGCTRL_NUM_ITEM, (u16)soubi);
            _menuCB_errorMsgDisp2(menuCBMsgHeldItemTaken);
        }
    } else {
        _menuCB_errorMsgDisp2(menuCBMsgNoHeldItem);
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
    hero = &menuCBTaisenWork.mHero;

    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)name);

    if ((s32)menuCBBios_CurrentRule()->mHeldItemMode == HELD_ITEMS_FORBIDDEN) {
        _menuCB_errorMsgDisp(menuCBMsgHeldItemNotAllowed);
        return;
    }

    menuOpen(0xE6, 1);
    select = menuPocket2Call(POCKET_MODE_GIVE_FIXED_ORDER, 0, hero);
    pocketSlot = menuPocket2GetLastSlotID();
    menuCloseCustom(0xE6, 0, 1);

    if (select == 0u)
        return;

    slot = (u16)menuCBTaisenWork.dstIndex - menuCBEntrySlotFirst;

    menuCBEntrySlotFlagsClear(0, slot);

    oldItem = pokemonDoItemSoubi(pokemon, 0, 0);
    pokemonDoItemSoubi(pokemon, select, 1);
    menuCBEntrySlotConditionsSetItemRegulation(hero, pokemon, slot);
    pokemonDoItemSoubi(pokemon, 0, 0);
    pokemonDoItemSoubi(pokemon, oldItem, 1);

    if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_PROHIBITED_ITEM) != 0) {
        _menuCB_errorMsgDisp(menuCBMsgProhibitedHeldItem);
        return;
    }

    if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_DUPLICATE_HELD_ITEM) != 0) {
        _menuCB_errorMsgDisp(menuCBMsgDuplicateHeldItem);
        return;
    }

    if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_ILLEGAL_HELD_ITEM) != 0) {
        _menuCB_errorMsgDisp(menuCBMsgIllegalHeldItem);
        return;
    }

    itemID = pokemonGetSoubiItemDataId(pokemon);

    if ((u16)itemID != 0) {
        msgctrlSetValue(MSGCTRL_NUM_ITEM, (u16)itemID);
        winMsgOpen(2, menuCBMsgConfirmHeldItemReplacement, 1, 0);

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
            _menuCB_errorMsgDisp2(menuCBMsgItemTransferFailed);
            return;
        }

        if (heroItemCheckAddItemDataId(hero, itemID) <= 0) {
            heroItemAddItemDataId(hero, select, 1, -1);
            _menuCB_errorMsgDisp2(menuCBMsgItemTransferFailed);
        } else {
            heroItemAddItemDataId(hero, itemID, 1, -1);

            oldItem = pokemonDoItemSoubi(pokemon, 0, 0);
            pokemonDoItemSoubi(pokemon, select, 1);
            msgctrlSetValue(MSGCTRL_NUM_ITEM, (u16)oldItem);
            msgctrlSetValue(MSGCTRL_NUM_SWAP_ITEM, (u16)select);
            _menuCB_errorMsgDisp2(menuCBMsgHeldItemReplaced);
        }
    } else {
        if (heroItemDecItemDataId(hero, select, 1, pocketSlot) != 0)
            return;

        pokemonDoItemSoubi(pokemon, select, 1);
        msgctrlSetValue(MSGCTRL_NUM_ITEM, (u16)select);
        _menuCB_errorMsgDisp2(menuCBMsgHeldItemGiven);
    }
}

void menuCBRuleWindowDisp(tagWINDOW_WORK *window, s32 trainer)
{
    u16 name[0x1F];
    s32 i;

    if (trainer >= 3) {
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)&menuCBTaisenWork.mRulesetData[trainer].m1E);
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

        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)name);
    }

    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x4275);

    if (menuCBTaisenWork.mRuleCursor == 5)
        msgctrlSetValue(MSGCTRL_NUM_PANEL, 2);
    else
        msgctrlSetValue(MSGCTRL_NUM_PANEL, menuCBTaisenWork.mRuleCursor + 1);

    msgctrlSetValue(MSGCTRL_NUM_INPUT, menuCBTaisenWork.mSlotNo);

    GSmsgPrint2(window->mLayerID, 0x1CE, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
    GSmsgPrint2(window->mLayerID, 0x1E4, 2, window->mRgba.mColorA | 0xFFFFFF00, 0x4417);

    if (menuCBTaisenWork.mEntrySelect != -1)
        i = 5;
    else
        i = 4;

    msgctrlSetValue(MSGCTRL_NUM_PANEL, i);
    msgctrlSetValue(MSGCTRL_NUM_INPUT, i);

    GSmsgPrint2(window->mLayerID, 0x1FA, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
}

void menuCBPortTrainerNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer)
{
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)&menuCBTaisenWork.mRulesetData[trainer].m1E);

    GSmsgPrint2(window->mLayerID, 0, 0,
                (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA, MSG_POKEMON_NAME);
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

    hero = &menuCBTaisenWork.mHero;

    menuCBEntrySlotConditionsBuildAll(hero, 0);

    for (slot = 0; slot < 6; slot++) {
        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_HELD_ITEM_NOT_ALLOWED) != 0) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgHeldItemNotAllowed;
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_PROHIBITED_ITEM) != 0) {
            menuCBTaisenWork.mSlotNo = slot + 1;
            menuCBTaisenWork.pokeMsgID = 0x4410;
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_DUPLICATE_HELD_ITEM) != 0) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgDuplicateHeldItem;
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_ILLEGAL_HELD_ITEM) != 0) {
            menuCBTaisenWork.pokeMsgID = menuCBMsgIllegalHeldItem;
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            return;
        }
    }

    for (u32 i = 0; i < 6; i++)
        menuCBEntrySlotFlagsClear(0, i);

    menuCBEntrySlotConditionsSetTrainerRegulation(hero);

    if (menuCBEntrySlotConditionGet(0, MENU_CB_ENTRY_CONDITION_ANY) != 0) {
        menuCBTaisenWork.dispState = menuCBDisplayEntryError;

        if (menuCBTaisenWork.entryFlags[menuCBTaisenWork.entryNo][0].insufficientPokemon)
            menuCBTaisenWork.pokeMsgID = menuCBMsgIncorrectEntryCount;
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
            msgctrlSetValue(MSGCTRL_NUM_INPUT, port);
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
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst;
        break;
    case 2:
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst + 1;
        break;
    case 3:
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst + 2;
        break;
    case 4:
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst + 3;
        break;
    case 5:
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotFirst + 4;
        break;
    case 6:
        menuCBTaisenWork.pokeSelect = menuCBEntrySlotLast;
        break;
    }

    _menuCBGCDelPokemon();
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
        _menuCBGCDelPokemon();
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
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            menuCBTaisenWork.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
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
            menuCBTaisenWork.dispState = menuCBDisplayEntryError;
            menuCBTaisenWork.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
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
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)pcboxGetPokemonBoxName(PCBOX::getPointer(), (s8)box));

    width = (s16)(GSmsgGetMessageSize(MSG_POKEMON_NAME) >> 16);
    x2 = (s16)(sprite->mWidth / 2 - width / 2);
    x2 += x;
    GSmsgPrint2(window->mLayerID, x2, 0, window->mRgba.mColorA | 0xFFFFFF00, MSG_POKEMON_NAME);

    spriteClearScissor(window->mLayerID);
}

void menuCBReliveTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID,
                               s32 x)
{
    s16 width;
    s16 x2;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(msgID));

    width = (s16)(GSmsgGetMessageSize(MSG_POKEMON_NAME) >> 16);
    x2 = (s16)(sprite->mWidth / 2 - width / 2);
    x2 += x;
    GSmsgPrint2(window->mLayerID, x2, 0, window->mRgba.mColorA | 0xFFFFFF00, MSG_POKEMON_NAME);

    spriteClearScissor(window->mLayerID);
}

void menuCBPokemonEntryTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x)
{
    s16 width;
    s16 x2;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)heroBiosGetNamePtr(
                              &menuCBTaisenWork.mHero));

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


extern "C" {
void menuCBPokemonItemName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank, index);
    u32 message = 1;
    if (pokemon && pokemonCheckValid(pokemon)) {
        u16 item = pokemonGetSoubiItemDataId(pokemon);
        if (item) message = itemDataBiosGetNameMsgID(itemDataBiosGetPtr(item));
    }
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(message));
    switch ((s32)mode) {
    case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgCompactSexMark)); break;
    case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(MSG_SEX_MARK)); break;
    }
}
void menuCBPokemonWazaNAME(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long moveSlot)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        u16 move = pokemonGetStatus(pokemon,0,POKEMON_QUERY_MOVE_ID,(u16)moveSlot);
        if (!move) { GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgEmptyMoveSlot); return; }
        u32 name = wazaGetStatus(0,move,WAZA_STATUS_NAME_MESSAGE_ID,0);
        if (name) {
            msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)GSmsgGetStringPtr(name));
            GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactPokemonName);
            switch (gameGetRegion()) {
            case 0: {
                s32 column = 10;
                u32 pp = pokemonGetStatus(pokemon,0,POKEMON_QUERY_MOVE_PP,(u16)moveSlot);
                if (pp < 10) column = 11;
                msgctrlSetValue(MSGCTRL_NUM_PANEL,pp);
                GSmsgPrint2(window->mLayerID,column*13,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber);
                break;
            }
            case 1: case 2:
                msgctrlSetValue(MSGCTRL_NUM_PANEL,pokemonGetStatus(pokemon,0,POKEMON_QUERY_MOVE_PP,(u16)moveSlot));
                GSmsgPrintRight(window->mLayerID,130,0,26,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber));
                break;
            }
        }
    }
}
void menuCBPokemonWazaTYPE(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long moveSlot)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        u16 move = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_MOVE_ID,(u16)moveSlot);
        u32 texture;
        switch ((s32)_menuCB_NotDarkWaza(move)) {
        case 0: texture = 0; break;
        case 0x176: texture = 0x193; break;
        default: texture = (u16)zokuseiBiosGetSmallSpriteID((u16)wazaGetStatus(0,move,WAZA_STATUS_TYPE_ID,0)); break;
        }
        if (texture) windowDrawSprite(0,0,window,(u16)texture,0,(GSgfxLayerID)window->mLayerID);
    }
}
void menuCBPokemonTYPE2(tagWINDOW_WORK *window, unsigned long bank, unsigned long index)
{
    PokemonSpeciesStats *data = pokemonDataBiosGetPtr((u16)pokemonGetStatus(menuCBPokemonGetPtr(bank,index),0,POKEMON_QUERY_SPECIES,0));
    u8 firstType = (u8)pokemonDataBiosGetZokuseiDataId(data,0), secondType = (u8)pokemonDataBiosGetZokuseiDataId(data,1);
    if (firstType != secondType) {
        u32 texture = (u16)zokuseiGetPokemonSpriteID(data,1);
        if (texture) windowDrawSprite(0,0,window,(u16)texture,0,(GSgfxLayerID)window->mLayerID);
    }
}
void menuCBPokemonTYPE1(tagWINDOW_WORK *window, unsigned long bank, unsigned long index)
{
    PokemonSpeciesStats *data = pokemonDataBiosGetPtr((u16)pokemonGetStatus(menuCBPokemonGetPtr(bank,index),0,POKEMON_QUERY_SPECIES,0));
    u32 texture = (u16)zokuseiGetPokemonSpriteID(data,0);
    if (texture) windowDrawSprite(0,0,window,(u16)texture,0,(GSgfxLayerID)window->mLayerID);
}
void menuCBPokemonStatusDisp(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode, unsigned long stat)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        s32 value;
        switch ((s32)stat) {
        case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgAttackLabel); value = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_ATTACK,0); break;
        case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgDefenseLabel); value = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_DEFENSE,0); break;
        case 2: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgSpecialAttackLabel); value = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_SPECIAL_ATTACK,0); break;
        case 3: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgSpecialDefenseLabel); value = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_SPECIAL_DEFENSE,0); break;
        case 4: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgSpeedLabel); value = (u16)pokemonGetStatus(pokemon,0,POKEMON_QUERY_SPEED,0); break;
        }
        msgctrlSetValue(MSGCTRL_NUM_PANEL,value); msgctrlSetValue(MSGCTRL_NUM_INPUT,value);
        switch (gameGetRegion()) {
        case 0: {
            s32 column = 5;
            if (value < 10) column = 6;
            if (value < 100) column++;
            switch ((s32)mode) {
            case 0: GSmsgPrint2(window->mLayerID,column*13,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber); break;
            case 1: GSmsgPrint2(window->mLayerID,column*18,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargePanelNumber); break;
            }
            break;
        }
        case 1: case 2:
            switch ((s32)mode) {
            case 0: GSmsgPrintRight(window->mLayerID,58,0,39,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber)); break;
            case 1: GSmsgPrintRight(window->mLayerID,81,0,54,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgLargePanelNumber)); break;
            }
            break;
        }
    }
}
void menuCBPokemonTOKUSEI(tagWINDOW_WORK *window, unsigned long bank, unsigned long index)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u32 name = 1;
    if (pokemon && pokemonCheckValid(pokemon)) name = pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(pokemonGetTokuseiDataId(pokemon)));
    msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)GSmsgGetStringPtr(name));
    switch (gameGetRegion()) {
    case 0:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgAbilityLabel);
        GSmsgPrint2(window->mLayerID,90,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_SEX_MARK);
        break;
    case 1: case 2:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgRegionalAbilityLabel);
        GSmsgPrint2(window->mLayerID,(s16)(GSmsgGetMessageSize(menuCBMsgRegionalAbilityLabel)>>16),0,0xFFFFFF00 | window->mRgba.mColorA,MSG_POKEMON_NAME);
        break;
    }
}
void menuCBPokemonHP(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        u16 hp = battlePokemonStatusGetCurrentHp(pokemon);
        msgctrlSetValue(MSGCTRL_NUM_PANEL,hp); msgctrlSetValue(MSGCTRL_NUM_INPUT,hp);
        switch ((s32)mode) {
        case 0:
            GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgHPLabel);
            switch (gameGetRegion()) { case 1: case 2: GSmsgPrint2(window->mLayerID,27,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber); break; }
            break;
        case 3: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber)); break;
        case 1:
            GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargeHPLabel);
            switch (gameGetRegion()) { case 1: case 2: GSmsgPrint2(window->mLayerID,38,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargePanelNumber); break; }
            break;
        case 2: break;

        }
    }
}
void menuCBPokemonLevel(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (!(pokemon && pokemonCheckValid(pokemon) && !pokemon_GetTamagoFlag(pokemon)))
        return;
    u8 level = _menuCBPokemonEntryGetlevel(pokemon);
    msgctrlSetValue(MSGCTRL_NUM_PANEL,level); msgctrlSetValue(MSGCTRL_NUM_INPUT,level);
    switch ((s32)mode) {
    case 0:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLevelLabel);
        switch (gameGetRegion()) { case 1: case 2: GSmsgPrint2(window->mLayerID,27,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber); break; }
        break;
    case 3: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber)); break;
    case 1:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargeLevelLabel);
        switch (gameGetRegion()) { case 1: case 2: GSmsgPrint2(window->mLayerID,38,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargePanelNumber); break; }
        break;
    case 2: break;

    }
}
void menuCBPokemonNameOrg(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u32 name = 1;
    if (pokemon && pokemonCheckValid(pokemon)) name = pokemonDB_GetName(pokemon_GetPokemonDataId(pokemon));
    msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)GSmsgGetStringPtr(name));
    switch ((s32)mode) {
    case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactPokemonName); break;
    case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_POKEMON_NAME); break;
    }
}
void menuCBPokemonSexOnly(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        u32 message;
        switch (menuSubGetPokemonSexForFightDisp(pokemon)) {
        case 0: message = MSG_SEX_MALE; break;
        case 1: message = MSG_SEX_FEMALE; break;
        case 2: default: message = 1; break;
        }
        if (message != 1) {
            msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)GSmsgGetStringPtr(message));
            switch ((s32)mode) {
            case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactSexMark); break;
            case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_SEX_MARK); break;
            }
        }
    }
}
void menuCBPokemonNameOnly(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        u16 *name = pokemon_GetNickname(pokemon);
        if (!name) name = GSmsgGetStringPtr(1);
        msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)name);
        switch ((s32)mode) {
        case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactPokemonName); break;
        case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_POKEMON_NAME); break;
        }
    }
}
}
void _menuCBPokemonName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode, bool showNidoranSex)
{
    u16 *name;
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u8 invalid = 0;
    if (pokemon) {
    if (!pokemonCheckValid(pokemon)) {
        if (!pokemonGetStatus(pokemon,0,POKEMON_QUERY_SPECIES,0)) return;
        name = GSmsgGetStringPtr(menuCBMsgInvalidPokemonName);
        invalid = 1;
    } else {
        name = pokemon_GetNickname(pokemon);
        if (!name) name = GSmsgGetStringPtr(1);
        if (pokemon_GetTamagoFlag(pokemon) == 1) name = GSmsgGetStringPtr(menuCBMsgEggName);
    }
    msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)name);
    switch ((s32)mode) {
    case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactPokemonName); break;
    case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_POKEMON_NAME); break;
    }
    if (pokemon_GetTamagoFlag(pokemon) == 1 || invalid) return;
    u32 sex;
    if (showNidoranSex) sex = (u8)menuSubGetPokemonSexForDisp(pokemon);
    else sex = (u8)menuSubGetPokemonSexForFightDisp(pokemon);
    u32 message;
    switch (sex) {
    case 0: message = MSG_SEX_MALE; break;
    case 1: message = MSG_SEX_FEMALE; break;
    case 2: default: message = 0; break;
    }
    if (message) {
        msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)GSmsgGetStringPtr(message));
        switch (gameGetRegion()) {
        case 0:
            switch ((s32)mode) {
            case 0: GSmsgPrint2(window->mLayerID,70,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactSexMark); break;
            case 1: GSmsgPrint2(window->mLayerID,90,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_SEX_MARK); break;
            }
            break;
        case 1: case 2:
            switch ((s32)mode) {
            case 0: GSmsgPrint2(window->mLayerID,90,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactSexMark); break;
            case 1: GSmsgPrint2(window->mLayerID,110,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_SEX_MARK); break;
            }
            break;
        }
    }
}
}
extern "C" {
void menuCBPokemonName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode) { _menuCBPokemonName(window,bank,index,mode,false); }
void menuCBPokemonName_nido(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode) { _menuCBPokemonName(window,bank,index,mode,true); }
}

#include <menuCBBattleStart.h>
void GSprimSetup2DRender(GSgfxLayer *layer);
extern "C" {
void *memset(void *dst, s32 value, u32 size);
void spriteSetOffset(s32 layer, s32 x, s32 y);
void spriteSetEnv(s32 layer);
u8 menuCB_BattleStartSoundCheck();
u8 FlashSystemIsLastFrame(u32 id);
Pokemon *toolentryTaisenGetEntryPokemonPtr(s32 trainer, s32 slot);

void winSeqSetMenu(s32 menu, s32 sequence);
u16 toolentryTaisenGetPokemonItemDataId(s32 trainer);
void menuCB2DPokemonItemDisp(tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    winSpriteSetDisp(sprite,0);
    if (pokemon && pokemonCheckValid(pokemon)) {
        if (pokemonGetSoubiItemDataId(pokemon)) winSpriteSetDisp(sprite,1);
    }
}
void menuCB2DPokemonClearSHIFT(unsigned long direction)
{
    if (!direction) {
        memcpy(gEntryAntRowThird,gEntryAntRowFirst,30*sizeof(CAnt));
        memcpy(gEntryAntRowFirst,gEntryAntRowSecond,30*sizeof(CAnt));
        memset(gEntryAntRowSecond,0,30*sizeof(CAnt));
    } else {
        memcpy(gEntryAntRowSecond,gEntryAntRowFirst,30*sizeof(CAnt));
        memcpy(gEntryAntRowFirst,gEntryAntRowThird,30*sizeof(CAnt));
        memset(gEntryAntRowThird,0,30*sizeof(CAnt));
    }
    CAnt::systemUsecountReset();
    for (u32 i=0;i<36;i++) gEntryAntRowFirst[i].use();
    for (u32 i=0;i<30;i++) { gEntryAntRowSecond[i].use(); gEntryAntRowThird[i].use(); }
    CAnt::systemUnUseResourceRelease();
    menuCBTaisenWork.mPageScrollTime = 0;
    if (!direction) {
        menuCBTaisenWork.mPageScrollStep = -16;
        menuCBTaisenWork.mPageScrollOffset = 384;
    } else {
        menuCBTaisenWork.mPageScrollStep = 16;
        menuCBTaisenWork.mPageScrollOffset = -384;
    }
    menuCBTaisenWork.mPreviousBattleMode2D = menuCBTaisenWork.battleMode2D;
    menuCBTaisenWork.mPreviousPage2D = menuCBTaisenWork.page2D;
    menuCBTaisenWork.mPreviousPage2DSub = menuCBTaisenWork.page2DSub;
}
void menuCB2DPokemonALLClear()
{
    CAnt::systemUsecountReset();
    CAnt::systemUnUseResourceRelease();
    memset(gEntryAntRowFirst,0,36*sizeof(CAnt));
    memset(gEntryAntRowThird,0,30*sizeof(CAnt));
    memset(gEntryAntRowSecond,0,30*sizeof(CAnt));
    menuCBTaisenWork.battleMode2D = 0;
    menuCB2DPokemonPageScroll(2);
    menuCB2DPokemonPageScroll(0);
    menuCB2DPokemonPageScroll(1);
    for (u32 i=0;i<36;i++) gEntryAntRowFirst[i].use();
    for (u32 i=0;i<30;i++) { gEntryAntRowSecond[i].use(); gEntryAntRowThird[i].use(); }
}
void menuCB2DPokemonDisp2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u32 modelIndex = menuCBPokemonGetIndex(bank,index);
    if (pokemon && pokemonCheckValid(pokemon)) {
        spriteSetOffset(window->mLayerID,0,0);
        spriteClearScissor(window->mLayerID);
        spriteSetEnv(window->mLayerID);
        if (modelIndex < 30) spriteSetScissor(window->mLayerID,18,81,384,320);
        PocketVec<s16> position;
        position.x = 32 + sprite->mX + window->mScrollX;
        position.y = sprite->mY + 32;
        if (menuCBTaisenWork.mPageScrollStep < 0) {
            CAnt *ant = &gEntryAntRowSecond[modelIndex];
            battlePokemonGetPokeBody(ant,pokemon);
            gEntryAntRowSecond[modelIndex].setPos(position);
            ant->setAnimation(0);
            GSprimSetup2DRender(0);
            if (ant->mBodyResourceID) {
                gEntryAntRowSecond[modelIndex].mAlpha = window->mRgba.mColorA;
                ant->drawAndUpdate();
            }
        } else {
            CAnt *ant = &gEntryAntRowThird[modelIndex];
            battlePokemonGetPokeBody(ant,pokemon);
            gEntryAntRowThird[modelIndex].setPos(position);
            ant->setAnimation(0);
            GSprimSetup2DRender(0);
            if (ant->mBodyResourceID) {
                gEntryAntRowThird[modelIndex].mAlpha = window->mRgba.mColorA;
                ant->drawAndUpdate();
            }
        }
    }
}
void menuCB2DPokemonDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index)
{
    CAnt *ant;
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u32 modelIndex = menuCBPokemonGetIndex(bank,index);
    if (modelIndex == 30) { menuCB2DPokemonPageScroll(0); menuCB2DPokemonPageScroll(1); }
    if (pokemon && pokemonCheckValid(pokemon)) {
        spriteSetOffset(window->mLayerID,0,0);
        spriteClearScissor(window->mLayerID);
        spriteSetEnv(window->mLayerID);
        if (modelIndex < 30) spriteSetScissor(window->mLayerID,18,81,384,320);
        PocketVec<s16> position;
        position.x = 32 + sprite->mX + window->mScrollX;
        position.y = sprite->mY + 32;
        ant = &gEntryAntRowFirst[modelIndex];
        battlePokemonGetPokeBody(ant,pokemon);
        gEntryAntRowFirst[modelIndex].setPos(position);
        ant->setAnimation(0);
        GSprimSetup2DRender(0);
        if (ant->mBodyResourceID) {
            gEntryAntRowFirst[modelIndex].mAlpha = window->mRgba.mColorA;
            ant->drawAndUpdate();
        }
    }
}
u8 menuCBBattleBANextMenu(tagWINDOW_WORK *window)
{
    if (pauseCheck()) return 0;
    if (menuCBBattleStartGetStatus() == 0) {
        if (menuCBTaisenWork.mBattleEndWait == 0) {
            if (menuCB_BattleStartSoundCheck()) window->mAccept = 1;
            if (FlashSystemIsLastFrame(0x21533000)) return 1;
            return 0;
        }
        menuCBTaisenWork.mBattleEndWait--;
        return 0;
    } else {
        if (menuCBFadeSoundStopCount == 0) { window->mAccept = 1; return 0; }
        menuCBFadeSoundStopCount--;
        if (menuCBFadeSoundStopCount == 55) {
            if (menuCBTaisenWork.mBattleEndHPSound != 0) menuCB_SE_Stop(0x5D0);
        }
        return 0;
    }
}
void menuCBBattleBABatuMark(tagSPRITE_WORK *sprite, s32 port, s32 slot)
{
    if (menuCBBattleStartGetStatus() == 0 || menuCBPokemonGetDataId(port, slot) == 0) {
        winSpriteSetDisp(sprite, 0);
    } else {
        if (menuCBTaisenWork.mBattleEndDisplayHP[port][slot] == 0.0f &&
            menuCBTaisenWork.mBattleEndTargetHP[port][slot] == 0.0f) {
            winSpriteSetDisp(sprite, 1);
            menuCBTaisenWork.mFaintedPokemonScale[port][slot] +=
                menuCBTaisenWork.mFaintedPokemonScaleStep[port][slot];
            if (menuCBTaisenWork.mFaintedPokemonScale[port][slot] >= 1.0f)
                menuCBTaisenWork.mFaintedPokemonScale[port][slot] = 1.0f;
            sprite->mScaleX = sprite->mScaleY =
                menuCBTaisenWork.mFaintedPokemonScale[port][slot];
        } else {
            winSpriteSetDisp(sprite, 0);
        }
    }
}

void menuCBBattleEndSetPokemonHP()
{
    Pokemon *pokemon;
    f32 totalStep = 0.0f;
    for (s32 trainer=0;trainer<4;trainer++) {
        for (s32 slot=0;slot<6;slot++) {
            pokemon = toolentryTaisenGetEntryPokemonPtr(trainer,slot);
            if (pokemon) {
                menuCBTaisenWork.mBattleEndTargetHP[trainer][slot] = (u16)battlePokemonStatusGetCurrentHp(pokemon);
                menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot] = (u16)battlePokemonStatusGetMaxHp(pokemon);
                menuCBTaisenWork.mBattleEndMaxHP[trainer][slot] = (u16)battlePokemonStatusGetMaxHp(pokemon);
                menuCBTaisenWork.mBattleEndHPStep[trainer][slot] = (menuCBTaisenWork.mBattleEndMaxHP[trainer][slot] - menuCBTaisenWork.mBattleEndTargetHP[trainer][slot])/60.0f;
                totalStep += menuCBTaisenWork.mBattleEndHPStep[trainer][slot];
                menuCBTaisenWork.mFaintedPokemonScale[trainer][slot] = 0.0f;
                menuCBTaisenWork.mFaintedPokemonScaleStep[trainer][slot] = 1.0f/15.0f;
            } else {
                menuCBTaisenWork.mBattleEndMaxHP[trainer][slot] = 100.0f;
                menuCBTaisenWork.mFaintedPokemonScaleStep[trainer][slot] = 0.0f;
                menuCBTaisenWork.mFaintedPokemonScale[trainer][slot] = 0.0f;
                menuCBTaisenWork.mBattleEndHPStep[trainer][slot] = 0.0f;
                menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot] = 0.0f;
                menuCBTaisenWork.mBattleEndTargetHP[trainer][slot] = 0.0f;
            }
        }
    }
    if (totalStep != 0.0f) menuCBTaisenWork.mBattleEndHPSound = 0x5D0;
    else menuCBTaisenWork.mBattleEndHPSound = 0;
    menuCBFadeSoundCount = 30;
    menuCBFadeSoundStopCount = 135;
}
void menuCB_BattleBA_HPBarDispSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot)
{
    if (menuCBBattleStartGetStatus() != 0 && menuCBPokemonGetDataId(trainer,slot)) {
        winSpriteSetDisp(sprite,1);
        if (menuCBFadeSoundCount == 0) {
            menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot] -= menuCBTaisenWork.mBattleEndHPStep[trainer][slot];
            if (menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot] < menuCBTaisenWork.mBattleEndTargetHP[trainer][slot])
                menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot] = menuCBTaisenWork.mBattleEndTargetHP[trainer][slot];
        } else {
            if (!pauseCheck()) menuCBFadeSoundCount--;
            if (menuCBFadeSoundCount == 0 && menuCBTaisenWork.mBattleEndHPSound != 0) menuCBSEPlay(0x5D0);
        }
        f32 hpMax = menuCBTaisenWork.mBattleEndMaxHP[trainer][slot];
        f32 hp = menuCBTaisenWork.mBattleEndDisplayHP[trainer][slot];
        s16 width = 48.0f * ((hpMax - (hpMax-hp))/hpMax);
        if ((20.0f * hpMax)/100.0f >= hp) menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[2]));
        else if ((50.0f * hpMax)/100.0f >= hp) menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[1]));
        else menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[0]));
        sprite->mWidth = width;
    } else winSpriteSetDisp(sprite,0);
}
void menuCB_BattleBA_FacePanelSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot)
{
    if (menuCBPokemonGetDataId(trainer,slot)) {
        winSpriteSetDisp(sprite,1);
        if (menuCBBattleStartGetStatus() == 0) menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleFacePanelTextures[1]));
        else menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleFacePanelTextures[0]));
    } else winSpriteSetDisp(sprite,0);
}
void menuCB_DispEffect(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_SEQUENCE_OPEN:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID,0x189);
            window->mFlag = 1;
        }
        break;
    case WINDOW_SEQUENCE_CLOSE:
        if (window->mFlag == 0) {
            if (menuCBTaisenWork.mSkipBattleExitEffect == 0) {
                winSeqSetMenu(window->mMenuID,0x18D);
                window->mFlag = 1;
                spriteClearScissor(window->mLayerID);
            } else {
                window->mFlag = 1;
                menuSetDisp(window->mMenuID,0);
            }
        }
        break;
    }
}
u8 menuCB_BattleBA_PokemonEntryNum()
{
    s32 count[2];
    for (s32 trainer=0;trainer<2;trainer++) {
        count[trainer] = 0;
        for (s32 slot=0;slot<6;slot++) if (menuCBPokemonGetDataId(trainer,slot)) count[trainer]++;
    }
    if (count[0] >= 4 || count[1] >= 4) return 1;
    return 0;
}
void menuCB_PrintXP(tagSPRITE_WORK *sprite, s32 trainer)
{
    u16 item = toolentryTaisenGetPokemonItemDataId(trainer);
    menuCBSpriteSetImage(sprite,menuSpriteBiosGetPtr(battleXPTextures[item]));
}
}

#include <menuCBBattleStart.h>

extern "C" {
GSgfxVF *GSgfxVFBuildTemp(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void GSgfxRenderDrawEnd(GSgfxLayer *layer);

void menuCBBattleStartDispTrainerFaceSub(tagWINDOW_WORK *window,
                                      tagSPRITE_WORK *sprite, s32 trainer)
{
    GStexture *texture;

    u8 alpha = window->mRgba.mColorA;
    GSgfxLayer &layer = *GSgfxLayerFindByID((GSgfxLayerID)window->mLayerID);
    MenuCBBattleFace &face = menuCBBattleStartWork.face[trainer];
    texture = (GStexture *)face.resource;
    if (texture != 0) {
        layer.mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
        layer.setRenderStateAndSelect(GSgfxVFBuildTemp(0, 3, 0, 0, 1, 5, 1, 1, 4));
        layer.texSelect(0, texture);
        layer.beginSurface(4);
        f32 zero = 0.0f, one = 1.0f;
        s16 x, y;
        GXWGFifo.u16 = 0;
        GXWGFifo.u16 = 0;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = alpha;
        GXWGFifo.f32 = zero;
        GXWGFifo.f32 = zero;
        x = sprite->mWidth;
        GXWGFifo.s16 = x;
        GXWGFifo.u16 = 0;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = alpha;
        GXWGFifo.f32 = one;
        GXWGFifo.f32 = zero;
        y = sprite->mHeight;
        GXWGFifo.u16 = 0;
        GXWGFifo.s16 = y;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = alpha;
        GXWGFifo.f32 = zero;
        GXWGFifo.f32 = one;
        y = sprite->mHeight;
        x = sprite->mWidth;
        GXWGFifo.s16 = x;
        GXWGFifo.s16 = y;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = 0xFF;
        GXWGFifo.u8 = alpha;
        GXWGFifo.f32 = one;
        GXWGFifo.f32 = one;
        GSgfxRenderDrawEnd(&layer);
    }
}
}
