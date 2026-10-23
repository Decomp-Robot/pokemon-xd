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
#include <menuCBCommon.h>
#include <menuTop.h>
#include <pokecolo.h>
#include <pause.h>
#include <toolentry.h>
#include <pcbox.h>

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

s32 pokecoloGetCountryCode(void);
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
u32 menuSubAnimAlpha(void);
// The full-screen base window (menu 0x119) the entry screens open on entry
// and close on exit: a single item, a 640x480 sprite, which the flash start
// drops to window level 0 so it stays under the screen's other windows.
void menuTopJumpVSMode(void);
// Start the connect battle screen flash, at its end frame unless
// playFromStart; waits until the flash system reports its last frame.
void menuCB_FlashStart(s32 playFromStart);
void menuCB_FlashEnd(s32 close);
// Source bank/index the 2D Pokemon screen takes its selected Pokemon from.
void menuCB2DPokemonSetSource(void);
// Return the Pokemon the 2D screen has selected to its party.
void menuCBGCDelPokemon(void);
// One condition bit of one entry slot; the index selects which condition.
u8 menuCBEntrySlotConditionGet(s32 slot, s32 condition);
// Drop every condition of one entry slot before its Pokemon is re-checked.
void menuCBEntrySlotFlagsClear(s32 entry, s32 slot);
// Run the current rule set's checks over `src` against the party based at
// `hero` and record them as the conditions of entry slot `index`.
void menuCBRule_ErrorFlagSetPokemonGetTiming(Hero *hero, Pokemon *src, s32 index);
Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index);
u16 menuCBRule_RegulationItemID(s32 index);
u8 itemDataCheckValid(u16 itemID);
void pokemonAllKaihuku(Pokemon *pokemon);

}


/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/* Bank/index pairs the entry screens park Pokemon in while they are moved
   around; _menuCBGCDelPokemon compacts them. */
extern u32 backup_type[6];
extern u32 backup_index[6];

/* 2D Pokemon model states of the entry screens: the three rows of CAnt models
   the screens animate. */
CAnt ant[36];
CAnt antR[30];
CAnt antL[30];

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
    menuCBPokemonWorkParty  = 7, // the party copy in _CB.
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
void menuCB_SE_Play(unsigned long seID);
void menuCB_Cursol_Disp_AlphaSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag);
void menuCB_AlphaGet(tagWINDOW_WORK *window);
void menuCB_GCEntryCursolDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned char flag);
void menuCBTrainerNameDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCB_RuleDispColorChenge_flag(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag);
void menuCB_RuleDispColorChenge(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 value);
void menuCB_RuleItemFlagDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCB_RuleItemNameEntry(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCBRuleNumberDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 kind);
void menuCBMessageCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID);
void menuCBMsgDrawOnSprite(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned long msgID);
void menuCBMessageYCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID);
void menuCB_GCPokemonHPandWAZA_MAX(void);
}

void _menuCB2DPokemonNewSet(unsigned char key);
void _menuCB_PokemonStatusDispAfterJob(unsigned long load);
void _menuCB_PokemonStatusDispBeforeJob(unsigned long unload);
u8 menuCBGCEntry_ErrorDisp(unsigned long bank, unsigned long index);
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
u16 battleEndHPStartFlag;
u16 battleEndHPEndFlag;

/* Phase of the access flash the entry screens show while loading. */
f32 _f_menuCB_circleAnimationTime = 0.0f;

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
    switch (pokecoloGetCountryCode()) {
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
        if (_CB.entryFlags[_CB.entryNo][i].eggOrInvalidPokemon) {
            _CB.entryFlags[_CB.entryNo][i].levelBelowMinimum = 0;
            _CB.entryFlags[_CB.entryNo][i].levelAboveMaximum = 0;
            _CB.entryFlags[_CB.entryNo][i].partyLevelSumExceeded = 0;
        }

        if (_CB.entryFlags[_CB.entryNo][i].invalidPokemon) {
            _CB.entryFlags[_CB.entryNo][i].levelBelowMinimum = 0;
            _CB.entryFlags[_CB.entryNo][i].levelAboveMaximum = 0;
            _CB.entryFlags[_CB.entryNo][i].partyLevelSumExceeded = 0;
            _CB.entryFlags[_CB.entryNo][i].eggOrInvalidPokemon = 1;
        }

        if (_CB.entryFlags[_CB.entryNo][i].invalidHeldItem &&
            _CB.entryFlags[_CB.entryNo][i].heldItemRestricted)
            _CB.entryFlags[_CB.entryNo][i].heldItemRestricted = 0;
    }
}

void _menuCB2DPokemonNewSet2(unsigned long bank, unsigned long page,
                             unsigned long count, unsigned char mode)
{
    /* Row the screens show: 0 is the +36 row, 1 the +66 row, 2 the +0 row. */
    CAnt *row36 = antR;
    CAnt *row66 = antL;
    CAnt *row00 = ant;

    unsigned long row = mode;
    Pokemon *pokemon;
    unsigned long i;

    for (i = 0; i < count; i++) {
        pokemon = menuCBPokemonGetPtr(bank, page | i);

        if (pokemon != 0 && (u8)pokemonCheckValid(pokemon) != 0) {
            switch (row) {
            case 0:
                row36->initByPokemonAndSetColorAnimationInAny(pokemon);
                row36->setAnimation(0);
                break;
            case 1:
                row66->initByPokemonAndSetColorAnimationInAny(pokemon);
                row66->setAnimation(0);
                break;
            case 2:
                row00->initByPokemonAndSetColorAnimationInAny(pokemon);
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
void _menuCB2DPokemonNewSet(unsigned char key)
{
    if (key == 2) {
        _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
        return;
    }

    long lastPage2D = 7;

    switch (_CB.battleMode2D) {
    case 0:
        switch (key) {
        case 0:
            _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0, 0x1E, key);
            break;
        case 1:
            if (_CB.pageWrap == 1)
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall, 0x80000, 5, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0x70000, 0x1E, key);
            break;
        }
        break;

    case 1:
        switch (key) {
        case 0:
            if (_CB.page2D == lastPage2D) {
                if (_CB.pageWrap == 1)
                    _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall, 0, 5, key);
                else
                    _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            } else {
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox,
                                       (_CB.page2D + 1) << 16,
                                       0x1E, key);
            }
            break;
        case 1:
            if (_CB.page2D == 0)
                _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox,
                                       (_CB.page2D - 1) << 16,
                                       0x1E, key);
            break;
        }
        break;

    case 2:
        switch (key) {
        case 0:
            if (_CB.page2DSub == 8)
                _menuCB2DPokemonNewSet2(menuCBPokemonHeroParty, 0, 6, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall,
                                       (_CB.page2DSub + 1) << 16,
                                       5, key);
            break;
        case 1:
            if (_CB.page2DSub == 0)
                _menuCB2DPokemonNewSet2(menuCBPokemonPcBox, 0x70000, 0x1E, key);
            else
                _menuCB2DPokemonNewSet2(menuCBPokemonReliveHall,
                                       (_CB.page2DSub - 1) << 16,
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

void _menuCB_PokemonStatusDispAfterJob(unsigned long load)
{
    if (load == 0) {
        GSfsysReadWait(0x88C);
        menuTopJumpVSMode();
        menuCB_FlashStart(0);
        fadeSet(2, 0.5f);
    }

    menuCB_SE_Play(0x14);
    menuOpen(0xBB, 0);
    menuOpen(0xBD, 0);
    menuOpen(0xBE, 0);
    menuOpen(0xBF, 0);
    menuOpen(0x10D, 0);
    menuOpen(0x10F, 0);
    menuOpen(0x10E, 0);
    menuOpen(0xC2, 0);
}

void _menuCB_PokemonStatusDispBeforeJob(unsigned long unload)
{
    if (unload == 0)
        menuCB_FlashEnd(1);

    menuCB_SE_Play(0x15);
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

    switch (_CB.pokeSelect) {
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
            u32 bank = backup_type[sel];

            index = backup_index[sel];

            backup_type[sel] = 0;
            backup_index[sel] = 0;
            menuCB2DPokemonSetSource();

            dst = menuCBPokemonGetPtr(_CB.dstBank, _CB.dstIndex);
            s32 region = pokecoloGetCountryCode();

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

            backup_type[sel] = backup_type[sel + 1];
            backup_index[sel] = backup_index[sel + 1];
            backup_type[sel + 1] = 0;
            backup_index[sel + 1] = 0;

            next = menuCBPokemonGetPtr(menuCBPokemonWorkParty, sel + 1);
            memcpy(slot, next, sizeof(Pokemon));
            pokemonInit(next);

        }
    }

    _CB.selectCount--;
}

u8 menuCBGCEntry_ErrorDisp(unsigned long bank, unsigned long index)
{
    s32 sel = _CB.selectCount - 1;
    Pokemon *pokemon = menuCBPokemonGetPtr(bank, index);

    menuCBEntrySlotFlagsClear(0, sel);
    menuCBRule_ErrorFlagSetPokemonGetTiming(&_CB.mHero, pokemon, sel);

    _CB.entryNo = 0;
    if (menuCBEntrySlotConditionGet(sel, MENU_CB_ENTRY_CONDITION_ANY) != 0) {
        menuCBGCDelPokemon();
        _CB.dispState = menuCBDisplayEntryError;

        if (_CB.entryFlags[_CB.entryNo][sel].legendPokemon) {
            _CB.pokeMsgID = menuCBMsgProhibitedSpecies;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].duplicateSpecies) {
            _CB.pokeMsgID = menuCBMsgDuplicateSpecies;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].levelBelowMinimum || _CB.entryFlags[_CB.entryNo][sel].levelAboveMaximum) {
            _CB.pokeMsgID = menuCBMsgLevelOutsideAllowedRange;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].shadowPokemon) {
            _CB.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].eggOrInvalidPokemon) {
            _CB.pokeMsgID = menuCBMsgEggNotAllowed;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].invalidMove) {
            _CB.pokeMsgID = menuCBMsgUnknownMove;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].illegalPokemon || _CB.entryFlags[_CB.entryNo][sel].invalidPokemon) {
            _CB.pokeMsgID = menuCBMsgInvalidPokemonData;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].heldItemForbidden) {
            _CB.pokeMsgID = menuCBMsgHeldItemNotAllowed;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].heldItemRestricted) {
            _CB.pokeMsgID = menuCBMsgProhibitedHeldItem;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].duplicateHeldItem) {
            _CB.pokeMsgID = menuCBMsgDuplicateHeldItem;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].invalidHeldItem) {
            _CB.pokeMsgID = menuCBMsgIllegalHeldItem;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].partyLevelSumExceeded) {
            _CB.pokeMsgID = menuCBMsgCombinedLevelTooHigh;
            return 0;
        }
        if (_CB.entryFlags[_CB.entryNo][sel].insufficientPokemon) {
            _CB.pokeMsgID = menuCBMsgIncorrectEntryCount;
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
            s32 region = pokecoloGetCountryCode();

            switch (region) {
            case 0:
                pokemon = stage->getDancerPointer(boxSlot);
                break;
            case 1:
            case 2:
                pokemon = _CB.reliveDancers[box][boxSlot];
                break;
            }
        }
        break;
    }

    case menuCBPokemonWorkParty:
        pokemon = heroBiosGetPokemonPtr(&_CB.mHero, (u16)index);
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
void menuCB_SE_Play(unsigned long seID)
{
    GSsndPlay(seID, 0, 0);
}

/* Alpha the sprite is drawn with, from the saved window colour. */
void menuCB_Cursol_Disp_AlphaSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag)
{
    if (flag != 0) {
        if (window->mRgba.mColorA == 0xFF)
            sprite->mRgba.mAlpha = _CB.savedColor;
        else
            sprite->mRgba.mAlpha = window->mRgba.mColorA;
    } else {
        if ((u32)window->mRgba.mColorA >= 0x7F)
            sprite->mRgba.mAlpha = 0x7F;
        else
            sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

void menuCB_AlphaGet(tagWINDOW_WORK *window)
{
    if (window->mRgba.mColorA == 0xFF)
        _CB.savedColor = menuSubAnimAlpha() & 0xFF;
    else
        _CB.savedColor = window->mRgba.mColorA;
}

/* -------------------------------------------------------------------------
 * Access flash
 * ---------------------------------------------------------------------- */

/* Blink the access marks around the sprite while the disc is being read. */
void menuCB_GCEntryCursolDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           unsigned char flag)
{
    if (flag != 0) {
        f32 phase = _f_menuCB_circleAnimationTime;
        f32 tri;
        s32 size;

        phase /= menuCBAccessFlashCyclePeriod;

        if (phase > menuCBAccessFlashCyclePeriod)
            phase = menuCBAccessFlashUnity - phase;
        tri = menuCBAccessFlashGain * phase;

        _f_menuCB_circleAnimationTime = _f_menuCB_circleAnimationTime + timeGetLastFrameSec();
        if (_f_menuCB_circleAnimationTime >= menuCBAccessFlashCyclePeriod)
            _f_menuCB_circleAnimationTime =
                fmod(_f_menuCB_circleAnimationTime, menuCBAccessFlashWrapPeriod);

        u32 color = window->mRgba.mColorA | 0xFFFFFF00;
        MenuSpriteValue *image = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(0x216));
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
                        (unsigned long)(_CB.mRulesetData[trainer].mTitle));
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

void menuCB_RuleDispColorChenge_flag(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                          unsigned char flag)
{
    if (flag != 0) {
        sprite->mRgba.mBlue = 0xFF;
        sprite->mRgba.mGreen = 0xFF;
        sprite->mRgba.mRed = 0xFF;
        sprite->mRgba.mAlpha = _CB.savedColor;
    } else {
        sprite->mRgba.mBlue = 0x7F;
        sprite->mRgba.mGreen = 0x7F;
        sprite->mRgba.mRed = 0x7F;
        sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

/* menuCB_RuleDispColorChenge_flag, but white only while value is the selected entry. */
void menuCB_RuleDispColorChenge(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 value)
{
    if (value == _CB.selectedValue) {
        sprite->mRgba.mBlue = 0xFF;
        sprite->mRgba.mGreen = 0xFF;
        sprite->mRgba.mRed = 0xFF;
        sprite->mRgba.mAlpha = _CB.savedColor;
    } else {
        sprite->mRgba.mBlue = 0x7F;
        sprite->mRgba.mGreen = 0x7F;
        sprite->mRgba.mRed = 0x7F;
        sprite->mRgba.mAlpha = window->mRgba.mColorA;
    }
}

void menuCB_RuleItemFlagDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 layer = winSpriteGetLayerID(window, sprite);
    tagMENU_ITEM_DATA *itemTop = menuItemBiosGetPtr(window->mMenuID, 0x3AB);
    tagMENU_ITEM_DATA *itemRow = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 row = _CB.listRow;
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
        } else if (_CB.mEntryRule->m52[i] != 0) {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFF3F0000,
                        0x43E4);
        } else {
            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        0x43E3);
        }
    }

    spriteClearScissor(layer);
}

void menuCB_RuleItemNameEntry(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 layer = winSpriteGetLayerID(window, sprite);
    tagMENU_ITEM_DATA *itemTop = menuItemBiosGetPtr(window->mMenuID, 0x3AC);
    tagMENU_ITEM_DATA *itemRow = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 row = _CB.listRow;
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
                u32 msgID = itemDataBiosGetName(itemDataBiosGetPtr(itemID));

                msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(msgID));
            }

            GSmsgPrint2(window->mLayerID, 0, y, window->mRgba.mColorA | 0xFFFFFF00,
                        menuCBRegionMsgID(MSG_SEX_MARK));
        }
    }

    spriteClearScissor(layer);
}

void menuCBRuleNumberDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 kind)
{
    s16 x;
    s32 num;
    u32 msgID;

    switch (kind) {
    case 0:
        num = _CB.mEntryRule->mPokemonLevelMin / 100;
        break;
    case 1:
        num = (_CB.mEntryRule->mPokemonLevelMin % 100) / 10;
        break;
    case 2:
        num = _CB.mEntryRule->mPokemonLevelMin % 10;
        break;
    case 3:
        num = _CB.mEntryRule->mPokemonLevelMax / 100;
        break;
    case 4:
        num = (_CB.mEntryRule->mPokemonLevelMax % 100) / 10;
        break;
    case 5:
        num = _CB.mEntryRule->mPokemonLevelMax % 10;
        break;
    case 6:
        num = _CB.mEntryRule->mPokemonLevelSumMax / 100;
        break;
    case 7:
        num = (_CB.mEntryRule->mPokemonLevelSumMax % 100) / 10;
        break;
    case 8:
        num = _CB.mEntryRule->mPokemonLevelSumMax % 10;
        break;

    case 9: {
        FightRuleData *rule = _CB.mEntryRule;

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
        if (_CB.mEntryRule->m14 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = _CB.mEntryRule->m14 % 10;
        break;
    case 11:
        if (_CB.mEntryRule->m16 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = _CB.mEntryRule->m16 / 10;
        break;
    case 12:
        if (_CB.mEntryRule->m16 & 0x8000) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = _CB.mEntryRule->m16 % 10;
        break;

    case 13:
        if (_CB.mEntryRule->mPokemonCountMode == 1) {
            GSmsgPrint2(window->mLayerID,
                        (s16)(sprite->mWidth / 2 -
                              (s16)(GSmsgGetMessageSize(0x4416) >> 16) / 2),
                        0, window->mRgba.mColorA | 0xFFFFFF00, 0x4416);
            return;
        }
        num = _CB.mEntryRule->mPokemonCount % 10;
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

void menuCBMessageCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                              unsigned long msgID)
{
    s32 region = pokecoloGetCountryCode();

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
 * menu set menuCB_RuleNameChengeAfterJob/menuCB_RuleNameChengeBeforeJob open and close, and drive their windows
 * through the checks below. The action submenu (menuCB_GCEntry_SubWindow_Disp) and the item
 * submenu (menuCB_GCEntry_ItemGiveJob) are the two ways a Pokemon is worked on; the entry
 * flow in the entry module runs their result.
 * ---------------------------------------------------------------------- */

extern "C" {
/* Entry-screen menu set and the windows' row checks. */
void menuCB_RuleNameChengeAfterJob(u8 load);
void menuCB_RuleNameChengeBeforeJob(u8 unload);
void menuCB_MenuDispCheckONorOFF_RULE(tagWINDOW_WORK *window, s32 index);
void menuCB_MenuDispCheckONorOFF_TUNAIDESUB(tagWINDOW_WORK *window, s32 index);
void menuCB_SpriteDispCheckONorOFF(tagSPRITE_WORK *sprite, bool on);

/* Submenus and the Pokemon work of the entry screens. */
void menuCB_GCEntry_ItemKeepJob(void);
void menuCB_GCEntry_ItemGiveJob(void);
void menuCBRULE_EditNameDispLarge(tagWINDOW_WORK *window, s32 trainer);
void menuCBRULE_EditNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCB_GCEntry_PokemonStatusDisp(void);
s32 menuCB_GCEntry_SubWindow_Disp(void);
void menuCB_TUNAIDE01_Rule_SELECT(void);
void menuCB_GCEntry_OkPushErrorCheck(void);
void menuCBPokemonEntryDispPokemonFace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                       s32 arg, s32 slot);
u8 menuCBPokemonEntryDispPokemonFace_AFTER(tagWINDOW_WORK *window,
                                           tagSPRITE_WORK *sprite, s32 arg, s32 slot);
u8 menuCBPokemonEntryDispPokemonFace_BEFORE(tagWINDOW_WORK *window,
                                            tagSPRITE_WORK *sprite, s32 arg, s32 slot);

/* Entry-slot faces and the entry screens' checks. */
void menuCBPokemonEntryBallAnaDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot);
void menuCBPokemonEntryBallDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot);
void menuCB_BattleEntryPokemonSelectDisp(tagSPRITE_WORK *sprite, s32 bank, s32 index);
s32 menuCB_GCStatusDispMode(tagSPRITE_WORK *sprite, s32 mode);
u8 menuCBPokemonEntryDisp(tagSPRITE_WORK *sprite);
void menuCBPokemonEntryGCControlDisp(tagSPRITE_WORK *sprite, s32 slot);
void menuCB2DPokemonSetSource(void);
void menuCBGCDelPokemon(void);
u8 menuCBPokemonCheck(unsigned long bank, unsigned long index);
void menuCB_WaitGCControler1(void);
void menuCB_GBADisconnectedErrorJob(void);
void menuCBGCEntryPokemon(void);
void menuCBReliveHallCursolInit(void);
void menuCBPCBOXTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 box,
                              s32 x);
void menuCBReliveTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID,
                               s32 x);
void menuCBGCEntryTitleDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x);

/* The fight module's archive members: fightReadResource frees the member the
   group held before, reads the requested one in when it is not in memory yet
   and returns 1 when it had to, 0 when it was already there; fightFreeResource
   releases one member. */
u8 fightReadResource(u32 group, u32 addr);
void fightFreeResource(u32 group, u32 addr);

/* The condition checks of the entry module: the party-wide trainer regulation,
   the conditions a held item gives one slot, and the rebuild of every slot of
   one entry. */
void menuCBRule_GetTaikaiFieldFirstDraw(Hero *hero);
void menuCBRule_ErrorFlagSetItemGetTiming(Hero *hero, Pokemon *pokemon, s32 slot);
void menuCBEntrySlotConditionsBuildAll(Hero *hero, s32 entry);

/* Held item accessors and the bag of the work party. */
u16 *pokemon_GetNickname(Pokemon *pokemon);

/* Item pocket, submenus and the entry screens' sources. */
FightRuleData *menuCBBios_CurrentRule(void);
u8 menuCBPokemonGetDataId(s32 port, s32 slot);
s32 menuCBBattleStartGetStatus(void);
void menuPokemonStatusOpenFromPokemonChange(Pokemon *pokemon, s32 a, s32 b);
u32 menuCBBios_ParticipationPokemonMax(void);
u16 toolentryTaisenGetPokemonNum(s32 port);
s32 toolentryTaisengetEtnryPokemonOrderNum(s32 port);
u16 toolentryTaisenGetHomePlace(s32 port);
u8 pauseIsPause(void);
s32 menuCB_GetDisconnectedGBA(void);
u8 gbaCommandIsConnected(s32 port);
u8 GSinputIsConnected(s32 pad);

/* The two-entry message table the entry select falls back on once all six
   slots are filled: the second and third choice of submenu_msg1, the first
   one (0x4398) being unavailable while no slot is left. */
extern u32 submenu_msgFull[2];
}

void menuCB_RuleNameChengeAfterJob(u8 load)
{
    if (load != 0) {
        GSfsysReadWait(0x88C);
        menuTopJumpVSMode();
        menuCB_FlashStart(0);
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
    }

    menuCB_SE_Play(0x14);
    menuOpen(0xA9, 0);
    menuOpen(0xCE, 0);
    menuOpen(0xCB, 0);
    menuOpen(0xCC, 0);
    menuOpen(0xCD, 0);
    menuOpen(0xAA, 0);
    menuOpen(0xF7, 0);
}

void menuCB_RuleNameChengeBeforeJob(u8 unload)
{
    if (unload != 0)
        menuCB_FlashEnd(1);

    menuCB_SE_Play(0x15);
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

void menuCB_MenuDispCheckONorOFF_RULE(tagWINDOW_WORK *window, s32 index)
{
    if (_CB.mRuleCursorPrev != -1) {
        menuSetDisp(window->mMenuID, 0);

        if (_CB.mRuleCursorPrev == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)_CB.mRuleCursorPrevColor;
        }

        if (_CB.mRuleCursor == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)_CB.mRuleCursorColor;
        }
    } else if (index == 1) {
        menuCB_MenuDispCheckONorOFF(window, _CB.mRuleCursor == index ||
                                               _CB.mRuleCursor == 5);
    } else {
        menuCB_MenuDispCheckONorOFF(window, _CB.mRuleCursor == index);
    }
}

void menuCB_MenuDispCheckONorOFF_TUNAIDESUB(tagWINDOW_WORK *window, s32 index)
{
    if (_CB.mEntryCursorPrev != -1) {
        menuSetDisp(window->mMenuID, 0);

        if (_CB.mEntryCursorPrev == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)_CB.mEntryCursorPrevColor;
        }

        if (_CB.mEntryCursor == index) {
            menuSetDisp(window->mMenuID, 1);
            window->mRgba.mColorA = (u8)_CB.mEntryCursorColor;
        }
    } else {
        menuCB_MenuDispCheckONorOFF(window, _CB.mEntryCursor == index);
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

void menuCB_GCEntry_ItemKeepJob(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(_CB.dstBank,
                                           _CB.dstIndex);
    u16 *name;
    Hero *hero;
    u16 itemID;

    name = pokemon_GetNickname(pokemon);
    hero = &_CB.mHero;

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

void menuCB_GCEntry_ItemGiveJob(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(_CB.dstBank,
                                           _CB.dstIndex);
    Hero *hero;
    u16 select;
    u8 pocketSlot;
    u16 itemID;
    u16 oldItem;
    s32 answer;
    u16 *name;
    s32 slot;

    name = pokemon_GetNickname(pokemon);
    hero = &_CB.mHero;

    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)name);

    if ((s32)menuCBBios_CurrentRule()->mHeldItemMode == MENU_CB_HELD_ITEMS_FORBIDDEN) {
        _menuCB_errorMsgDisp(menuCBMsgHeldItemNotAllowed);
        return;
    }

    menuOpen(0xE6, 1);
    select = menuPocket2Call(POCKET_MODE_GIVE_FIXED_ORDER, 0, hero);
    pocketSlot = menuPocket2GetLastSlotID();
    menuCloseCustom(0xE6, 0, 1);

    if (select == 0u)
        return;

    slot = (u16)_CB.dstIndex - menuCBEntrySlotFirst;

    menuCBEntrySlotFlagsClear(0, slot);

    oldItem = pokemonDoItemSoubi(pokemon, 0, 0);
    pokemonDoItemSoubi(pokemon, select, 1);
    menuCBRule_ErrorFlagSetItemGetTiming(hero, pokemon, slot);
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

        switch (pokecoloGetCountryCode()) {
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

void menuCBRULE_EditNameDispLarge(tagWINDOW_WORK *window, s32 trainer)
{
    u16 name[0x1F];
    s32 i;

    if (trainer >= 3) {
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)&_CB.mRulesetData[trainer].mTitle);
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

    if (_CB.mRuleCursor == 5)
        msgctrlSetValue(MSGCTRL_NUM_PANEL, 2);
    else
        msgctrlSetValue(MSGCTRL_NUM_PANEL, _CB.mRuleCursor + 1);

    msgctrlSetValue(MSGCTRL_NUM_INPUT, _CB.mSlotNo);

    GSmsgPrint2(window->mLayerID, 0x1CE, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
    GSmsgPrint2(window->mLayerID, 0x1E4, 2, window->mRgba.mColorA | 0xFFFFFF00, 0x4417);

    if (_CB.mEntrySelect != -1)
        i = 5;
    else
        i = 4;

    msgctrlSetValue(MSGCTRL_NUM_PANEL, i);
    msgctrlSetValue(MSGCTRL_NUM_INPUT, i);

    GSmsgPrint2(window->mLayerID, 0x1FA, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x427A);
}

void menuCBRULE_EditNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer)
{
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)&_CB.mRulesetData[trainer].mTitle);

    GSmsgPrint2(window->mLayerID, 0, 0,
                (sprite->mColor & 0xFFFFFF00) | window->mRgba.mColorA, MSG_POKEMON_NAME);
}

void menuCB_GCEntry_PokemonStatusDisp(void)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(_CB.dstBank,
                                           _CB.dstIndex);

    _menuCB_PokemonStatusDispBeforeJob(0);
    menuPokemonStatusOpenFromPokemonChange(pokemon, 0, 0);
    _menuCB_PokemonStatusDispAfterJob(0);
}

s32 menuCB_GCEntry_SubWindow_Disp(void)
{
    u16 index;

    if (_CB.mSubmenuConfirm == 1) {
        winMsgOpen(2, 0x43A1, 1, 0);

        switch (pokecoloGetCountryCode()) {
        case 0:
            _CB.selectResult = menuSubOpenYesNo(0, 0x3C, 0xD2, 0);
            break;
        case 1:
        case 2:
            _CB.selectResult = menuSubOpenYesNo(0, 0x3C, 0xC8, 0);
            break;
        }

        if (_CB.selectResult == -1)
            _CB.selectResult = 1;

        winMsgClose(0);
        return _CB.selectResult;
    }

    menuCB2DPokemonSetSource();

    index = (u16)_CB.dstIndex;

    if (index >= menuCBEntrySlotFirst && index <= menuCBEntrySlotLast) {
        _CB.selectResult =
            menuSubOpenSelect2(0, submenu_msg2, 4, 0x28, 0x64, 0, 4, 1);

        if (_CB.selectResult == -1)
            _CB.selectResult = 3;

        switch (_CB.selectResult) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            winMsgOpen(2, 0x43A0, 0, 0);
            _CB.selectResult =
                menuSubOpenSelect2(0, submenu_msg3, 3, 0x28, 0x64, 0, 3, 1);

            if (_CB.selectResult == -1)
                _CB.selectResult = 2;

            switch (_CB.selectResult) {
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

        switch (_CB.battleMode2D) {
        case 1:
            x = submenu1_x[index];
            y = submenu1_y[index];
            break;
        case 0:
        case 2:
        case 3:
            break;
        }

        if (_CB.selectCount == 6) {
            _CB.selectResult =
                menuSubOpenSelect2(0, submenu_msgFull, 2, x, y, 0, 2, 1);

            if (_CB.selectResult == -1)
                _CB.selectResult = 1;
        } else {
            _CB.selectResult =
                menuSubOpenSelect2(0, submenu_msg1, 3, x, y, 0, 3, 1);

            if (_CB.selectResult == -1)
                _CB.selectResult = 2;
        }

        if (_CB.selectCount == 6) {
            switch (_CB.selectResult) {
            case 0:
                return 2;
            case 1:
                return 6;
            }
        } else {
            switch (_CB.selectResult) {
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

void menuCB_TUNAIDE01_Rule_SELECT(void)
{
    switch (_CB.mRuleNo) {
    case 0:
    case 1:
    case 2:
        _CB.selectResult =
            menuSubOpenSelect2(0, submenurule_msg1, 3, 0x28, 0xF4,
                               _CB.selectResult, 3, 1);

        if (_CB.selectResult == -1)
            _CB.selectResult = 2;
        break;

    case 3:
    case 4:
    case 5:
        _CB.selectResult =
            menuSubOpenSelect2(0, submenurule_msg2, 4, 0x28, 0xD4,
                               _CB.selectResult, 4, 1);

        if (_CB.selectResult == -1)
            _CB.selectResult = 3;

        _CB.selectResult += 0x10;
        break;
    }

    switch (_CB.selectResult) {
    case 0:
    case 0x10:
        _CB.mRuleSelect = 0;
        break;
    case 2:
    case 0x13:
        _CB.mRuleSelect = 1;
        break;
    }
}

void menuCB_GCEntry_OkPushErrorCheck(void)
{
    Hero *hero;
    u32 slot;

    for (u32 i = 0; i < 6; i++)
        menuCBEntrySlotFlagsClear(0, i);

    hero = &_CB.mHero;

    menuCBEntrySlotConditionsBuildAll(hero, 0);

    for (slot = 0; slot < 6; slot++) {
        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_HELD_ITEM_NOT_ALLOWED) != 0) {
            _CB.pokeMsgID = menuCBMsgHeldItemNotAllowed;
            _CB.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_PROHIBITED_ITEM) != 0) {
            _CB.mSlotNo = slot + 1;
            _CB.pokeMsgID = 0x4410;
            _CB.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_DUPLICATE_HELD_ITEM) != 0) {
            _CB.pokeMsgID = menuCBMsgDuplicateHeldItem;
            _CB.dispState = menuCBDisplayEntryError;
            return;
        }

        if (menuCBEntrySlotConditionGet(slot, MENU_CB_ENTRY_ILLEGAL_HELD_ITEM) != 0) {
            _CB.pokeMsgID = menuCBMsgIllegalHeldItem;
            _CB.dispState = menuCBDisplayEntryError;
            return;
        }
    }

    for (u32 i = 0; i < 6; i++)
        menuCBEntrySlotFlagsClear(0, i);

    menuCBRule_GetTaikaiFieldFirstDraw(hero);

    if (menuCBEntrySlotConditionGet(0, MENU_CB_ENTRY_CONDITION_ANY) != 0) {
        _CB.dispState = menuCBDisplayEntryError;

        if (_CB.entryFlags[_CB.entryNo][0].insufficientPokemon)
            _CB.pokeMsgID = menuCBMsgIncorrectEntryCount;
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

void menuCBPokemonEntryBallAnaDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot)
{
    u16 limit = (u16)menuCBBios_ParticipationPokemonMax();
    u16 num = toolentryTaisenGetPokemonNum(port);
    u16 bound;

    _CB.mPartySlotX[port][slot - 1] = sprite->mX;

    if (_CB.mPartySlotFlag[port][slot - 1] == 0)
        _CB.mPartySlotValue[port][slot - 1] = 0x32;

    if (num < limit)
        bound = num;
    else
        bound = limit;

    if (slot > (u16)bound)
        winSpriteSetDisp(sprite, 0);
    else
        winSpriteSetDisp(sprite, 1);
}

void menuCBPokemonEntryBallDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot)
{
    u16 limit = (u16)menuCBBios_ParticipationPokemonMax();
    u16 num = toolentryTaisenGetPokemonNum(port);
    s32 order = (u16)toolentryTaisengetEtnryPokemonOrderNum(port);
    u16 bound;
    s16 x;

    if (num < limit)
        bound = num;
    else
        bound = limit;

    if (slot > bound || slot > order) {
        if (pauseIsPause()) {
            if (_CB.mPartySlotFlag[port][slot - 1] == 0)
                winSpriteSetDisp(sprite, 0);
            else
                winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
            _CB.mPartySlotFlag[port][slot - 1] = 0;
        }
        return;
    }

    if (pauseIsPause()) {
        if (_CB.mPartySlotFlag[port][slot - 1] != 1) {
            winSpriteSetDisp(sprite, 0);
            _CB.mPartySlotFlag[port][slot - 1] = 0;
            return;
        }
    }

    _CB.mPartySlotFlag[port][slot - 1] = 1;

    x = _CB.mPartySlotX[port][slot - 1] +
        _CB.mPartySlotValue[port][slot - 1];
    sprite->mX = x;

    if (_CB.mPartySlotValue[port][slot - 1] != 0)
        _CB.mPartySlotValue[port][slot - 1] -= 5;

    winSpriteSetDisp(sprite, 1);

    if (_CB.mTrainer[port].mEntryList[slot - 1] == 0xF)
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
    if (_CB.battleMode == 2) {
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
    s32 home = toolentryTaisenGetHomePlace(0);

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
    switch (_CB.battleMode2D) {
    case 0:
        _CB.dstBank = menuCBPokemonHeroParty;
        _CB.dstIndex = _CB.pokeSelect;
        return;

    case 1:
        _CB.dstBank = menuCBPokemonPcBox;
        _CB.dstIndex = (u16)_CB.pokeSelect |
                                    ((_CB.page2D << 16) & 0xFFFF0000);
        return;

    case 2:
        _CB.dstBank = menuCBPokemonReliveHall;
        _CB.dstIndex = (u16)_CB.pokeSelect |
                                    ((_CB.page2DSub << 16) & 0xFFFF0000);
        return;

    case 3:
        _CB.dstBank = menuCBPokemonWorkParty;
        _CB.dstIndex = _CB.pokeSelect;
        return;
    }
}

void menuCBGCDelPokemon(void)
{
    s32 select = _CB.pokeSelect;

    switch (_CB.selectCount) {
    case 1:
        _CB.pokeSelect = menuCBEntrySlotFirst;
        break;
    case 2:
        _CB.pokeSelect = menuCBEntrySlotFirst + 1;
        break;
    case 3:
        _CB.pokeSelect = menuCBEntrySlotFirst + 2;
        break;
    case 4:
        _CB.pokeSelect = menuCBEntrySlotFirst + 3;
        break;
    case 5:
        _CB.pokeSelect = menuCBEntrySlotFirst + 4;
        break;
    case 6:
        _CB.pokeSelect = menuCBEntrySlotLast;
        break;
    }

    _menuCBGCDelPokemon();
    _CB.pokeSelect = select;
}

void menuCBGCEntryPokemon(void)
{
    u32 bank;
    u32 index;
    Pokemon *src;
    Pokemon *pokemon;

    menuCB2DPokemonSetSource();

    if (menuCBPokemonCheck(_CB.dstBank, _CB.dstIndex) == 0)
        return;

    if (_CB.pokeSelect >= menuCBEntrySlotFirst) {
        _menuCBGCDelPokemon();
        return;
    }

    if (_CB.selectCount == 6)
        return;

    menuCB2DPokemonSetSource();

    bank = _CB.dstBank;
    index = _CB.dstIndex;

    backup_type[_CB.selectCount] = bank;
    backup_index[_CB.selectCount] = index;

    pokemon = menuCBPokemonGetPtr(menuCBPokemonWorkParty, _CB.selectCount);

    switch (pokecoloGetCountryCode()) {
    case 0:
        if (bank != menuCBPokemonReliveHall) {
            src = menuCBPokemonGetPtr(bank, index);

            memcpy(pokemon, src, sizeof(Pokemon));
            pokemonInit(src);
        } else if (_CB.pokeSelect == 4) {
            backup_type[_CB.selectCount] = 0;
            backup_index[_CB.selectCount] = 0;
            _CB.dispState = menuCBDisplayEntryError;
            _CB.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
            return;
        } else {
            CReliveStage *stage = CReliveHall::getPointer()->getStage(index >> 16);

            stage->pickupDancer(*pokemon, index & 0xFFFF);
        }
        break;

    case 1:
    case 2:
        if (bank == menuCBPokemonReliveHall && _CB.pokeSelect == 4) {
            backup_type[_CB.selectCount] = 0;
            backup_index[_CB.selectCount] = 0;
            _CB.dispState = menuCBDisplayEntryError;
            _CB.pokeMsgID = menuCBMsgShadowPokemonNotAllowed;
            return;
        }

        src = menuCBPokemonGetPtr(bank, index);

        memcpy(pokemon, src, sizeof(Pokemon));
        pokemonInit(src);
        break;
    }

    _CB.selectCount++;

    if (menuCBGCEntry_ErrorDisp(bank, _CB.selectCount + 0x1D) != 0 &&
        _CB.selectCount == 6)
        _CB.pokeSelect = 0x25;
}

void menuCBReliveHallCursolInit(void)
{
    s32 base;
    s32 page;
    u32 i;

    page = _CB.page2DSub;

    if (_CB.pokeSelect > 4)
        return;

    base = (page << 16) & 0xFFFF0000;

    if (menuCBPokemonCheck(menuCBPokemonReliveHall, _CB.pokeSelect | base) != 0)
        return;

    for (i = 0; i < 5; i++) {
        if (menuCBPokemonCheck(menuCBPokemonReliveHall, i | base) != 0) {
            _CB.pokeSelect = (s8)i;
            break;
        }
    }

    if (_CB.pokeSelect != i)
        _CB.pokeSelect = menuCBEntrySlotFirst;
}

void menuCBPCBOXTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 box, s32 x)
{
    s16 x2;
    s16 width;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)PCBOX::getPointer()->getPokemonBoxName((s8)box));

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

void menuCBGCEntryTitleDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x)
{
    s16 width;
    s16 x2;

    spriteSetScissor(window->mLayerID, 0, 0, 0xD1, 0x12);
    msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)heroBiosGetNamePtr(
                              &_CB.mHero));

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
        if (item) message = itemDataBiosGetName(itemDataBiosGetPtr(item));
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
            switch (pokecoloGetCountryCode()) {
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
        switch (pokecoloGetCountryCode()) {
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
    switch (pokecoloGetCountryCode()) {
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
        u16 hp = pokemonBiosGetHp(pokemon);
        msgctrlSetValue(MSGCTRL_NUM_PANEL,hp); msgctrlSetValue(MSGCTRL_NUM_INPUT,hp);
        switch ((s32)mode) {
        case 0:
            GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgHPLabel);
            switch (pokecoloGetCountryCode()) { case 1: case 2: GSmsgPrint2(window->mLayerID,27,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber); break; }
            break;
        case 3: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber)); break;
        case 1:
            GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargeHPLabel);
            switch (pokecoloGetCountryCode()) { case 1: case 2: GSmsgPrint2(window->mLayerID,38,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargePanelNumber); break; }
            break;
        case 2: break;

        }
    }
}
void menuCBPokemonLevel(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode)
{
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    if (!(pokemon && pokemonCheckValid(pokemon) && !pokemonBiosGetTamagoFlag(pokemon)))
        return;
    u8 level = _menuCBPokemonEntryGetlevel(pokemon);
    msgctrlSetValue(MSGCTRL_NUM_PANEL,level); msgctrlSetValue(MSGCTRL_NUM_INPUT,level);
    switch ((s32)mode) {
    case 0:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLevelLabel);
        switch (pokecoloGetCountryCode()) { case 1: case 2: GSmsgPrint2(window->mLayerID,27,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgPanelNumber); break; }
        break;
    case 3: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBRegionMsgID(menuCBMsgPanelNumber)); break;
    case 1:
        GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargeLevelLabel);
        switch (pokecoloGetCountryCode()) { case 1: case 2: GSmsgPrint2(window->mLayerID,38,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgLargePanelNumber); break; }
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
        if (pokemonBiosGetTamagoFlag(pokemon) == 1) name = GSmsgGetStringPtr(menuCBMsgEggName);
    }
    msgctrlSetValue(MSGCTRL_TEXT,(unsigned long)name);
    switch ((s32)mode) {
    case 0: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,menuCBMsgCompactPokemonName); break;
    case 1: GSmsgPrint2(window->mLayerID,0,0,0xFFFFFF00 | window->mRgba.mColorA,MSG_POKEMON_NAME); break;
    }
    if (pokemonBiosGetTamagoFlag(pokemon) == 1 || invalid) return;
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
        switch (pokecoloGetCountryCode()) {
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
void spriteSetEnv(s32 layer);
u8 menuCB_BattleStartSoundCheck();
u8 FlashSystemIsLastFrame(u32 id);
Pokemon *toolentryTaisenGetEntryPokemonPtr(s32 trainer, s32 slot);

void winSeqSetMenu(s32 menu, s32 sequence);
u16 toolentryTaisenGetBattlePlayerID(s32 trainer);
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
        memcpy(antL,ant,30*sizeof(CAnt));
        memcpy(ant,antR,30*sizeof(CAnt));
        memset(antR,0,30*sizeof(CAnt));
    } else {
        memcpy(antR,ant,30*sizeof(CAnt));
        memcpy(ant,antL,30*sizeof(CAnt));
        memset(antL,0,30*sizeof(CAnt));
    }
    CAnt::systemUsecountReset();
    for (u32 i=0;i<36;i++) ant[i].use();
    for (u32 i=0;i<30;i++) { antR[i].use(); antL[i].use(); }
    CAnt::systemUnUseResourceRelease();
    _CB.outgoingPageScrollOffset = 0;
    if (!direction) {
        _CB.pageScrollVelocity = -16;
        _CB.incomingPageScrollOffset = 384;
    } else {
        _CB.pageScrollVelocity = 16;
        _CB.incomingPageScrollOffset = -384;
    }
    _CB.previousBattleMode2D = _CB.battleMode2D;
    _CB.previousPage2D = _CB.page2D;
    _CB.previousPage2DSub = _CB.page2DSub;
}
void menuCB2DPokemonALLClear()
{
    CAnt::systemUsecountReset();
    CAnt::systemUnUseResourceRelease();
    memset(ant,0,36*sizeof(CAnt));
    memset(antL,0,30*sizeof(CAnt));
    memset(antR,0,30*sizeof(CAnt));
    _CB.battleMode2D = 0;
    _menuCB2DPokemonNewSet(2);
    _menuCB2DPokemonNewSet(0);
    _menuCB2DPokemonNewSet(1);
    for (u32 i=0;i<36;i++) ant[i].use();
    for (u32 i=0;i<30;i++) { antR[i].use(); antL[i].use(); }
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
        if (_CB.pageScrollVelocity < 0) {
            CAnt *pokemonModel = &antR[modelIndex];
            pokemonModel->initByPokemonAndSetColorAnimationInAny(pokemon);
            antR[modelIndex].setPosition(position);
            pokemonModel->setAnimation(0);
            GSprimSetup2DRender(0);
            if (pokemonModel->mResource) {
                antR[modelIndex].mAlpha = window->mRgba.mColorA;
                pokemonModel->drawAndUpdate();
            }
        } else {
            CAnt *pokemonModel = &antL[modelIndex];
            pokemonModel->initByPokemonAndSetColorAnimationInAny(pokemon);
            antL[modelIndex].setPosition(position);
            pokemonModel->setAnimation(0);
            GSprimSetup2DRender(0);
            if (pokemonModel->mResource) {
                antL[modelIndex].mAlpha = window->mRgba.mColorA;
                pokemonModel->drawAndUpdate();
            }
        }
    }
}
void menuCB2DPokemonDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index)
{
    CAnt *pokemonModel;
    Pokemon *pokemon = menuCBPokemonGetPtr(bank,index);
    u32 modelIndex = menuCBPokemonGetIndex(bank,index);
    if (modelIndex == 30) { _menuCB2DPokemonNewSet(0); _menuCB2DPokemonNewSet(1); }
    if (pokemon && pokemonCheckValid(pokemon)) {
        spriteSetOffset(window->mLayerID,0,0);
        spriteClearScissor(window->mLayerID);
        spriteSetEnv(window->mLayerID);
        if (modelIndex < 30) spriteSetScissor(window->mLayerID,18,81,384,320);
        PocketVec<s16> position;
        position.x = 32 + sprite->mX + window->mScrollX;
        position.y = sprite->mY + 32;
        pokemonModel = &ant[modelIndex];
        pokemonModel->initByPokemonAndSetColorAnimationInAny(pokemon);
        ant[modelIndex].setPosition(position);
        pokemonModel->setAnimation(0);
        GSprimSetup2DRender(0);
        if (pokemonModel->mResource) {
            ant[modelIndex].mAlpha = window->mRgba.mColorA;
            pokemonModel->drawAndUpdate();
        }
    }
}
u8 menuCBBattleBANextMenu(tagWINDOW_WORK *window)
{
    if (pauseIsPause()) return 0;
    if (menuCBBattleStartGetStatus() == 0) {
        if (_CB.battleStartWait == 0) {
            if (menuCB_BattleStartSoundCheck()) window->mAccept = 1;
            if (FlashSystemIsLastFrame(0x21533000)) return 1;
            return 0;
        }
        _CB.battleStartWait--;
        return 0;
    } else {
        if (battleEndHPEndFlag == 0) { window->mAccept = 1; return 0; }
        battleEndHPEndFlag--;
        if (battleEndHPEndFlag == 55) {
            if (_CB.hpRecoverySound != 0) menuCB_SE_Stop(0x5D0);
        }
        return 0;
    }
}
void menuCBBattleBABatuMark(tagSPRITE_WORK *sprite, s32 port, s32 slot)
{
    if (menuCBBattleStartGetStatus() == 0 || menuCBPokemonGetDataId(port, slot) == 0) {
        winSpriteSetDisp(sprite, 0);
    } else {
        if (_CB.hpDisplay[port][slot] == 0.0f &&
            _CB.hpAfterBattle[port][slot] == 0.0f) {
            winSpriteSetDisp(sprite, 1);
            _CB.faintMarkScale[port][slot] +=
                _CB.faintMarkScaleStep[port][slot];
            if (_CB.faintMarkScale[port][slot] >= 1.0f)
                _CB.faintMarkScale[port][slot] = 1.0f;
            sprite->mScaleX = sprite->mScaleY =
                _CB.faintMarkScale[port][slot];
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
                _CB.hpAfterBattle[trainer][slot] = (u16)pokemonBiosGetHp(pokemon);
                _CB.hpDisplay[trainer][slot] = (u16)pokemonBiosGetMaxHp(pokemon);
                _CB.hpMaximum[trainer][slot] = (u16)pokemonBiosGetMaxHp(pokemon);
                _CB.hpRecoveryStep[trainer][slot] = (_CB.hpMaximum[trainer][slot] - _CB.hpAfterBattle[trainer][slot])/60.0f;
                totalStep += _CB.hpRecoveryStep[trainer][slot];
                _CB.faintMarkScale[trainer][slot] = 0.0f;
                _CB.faintMarkScaleStep[trainer][slot] = 1.0f/15.0f;
            } else {
                _CB.hpMaximum[trainer][slot] = 100.0f;
                _CB.faintMarkScaleStep[trainer][slot] = 0.0f;
                _CB.faintMarkScale[trainer][slot] = 0.0f;
                _CB.hpRecoveryStep[trainer][slot] = 0.0f;
                _CB.hpDisplay[trainer][slot] = 0.0f;
                _CB.hpAfterBattle[trainer][slot] = 0.0f;
            }
        }
    }
    if (totalStep != 0.0f) _CB.hpRecoverySound = 0x5D0;
    else _CB.hpRecoverySound = 0;
    battleEndHPStartFlag = 30;
    battleEndHPEndFlag = 135;
}
void menuCB_BattleBA_HPBarDispSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot)
{
    if (menuCBBattleStartGetStatus() != 0 && menuCBPokemonGetDataId(trainer,slot)) {
        winSpriteSetDisp(sprite,1);
        if (battleEndHPStartFlag == 0) {
            _CB.hpDisplay[trainer][slot] -= _CB.hpRecoveryStep[trainer][slot];
            if (_CB.hpDisplay[trainer][slot] < _CB.hpAfterBattle[trainer][slot])
                _CB.hpDisplay[trainer][slot] = _CB.hpAfterBattle[trainer][slot];
        } else {
            if (!pauseIsPause()) battleEndHPStartFlag--;
            if (battleEndHPStartFlag == 0 && _CB.hpRecoverySound != 0) menuCB_SE_Play(0x5D0);
        }
        f32 hpMax = _CB.hpMaximum[trainer][slot];
        f32 hp = _CB.hpDisplay[trainer][slot];
        s16 width = 48.0f * ((hpMax - (hpMax-hp))/hpMax);
        if ((20.0f * hpMax)/100.0f >= hp) menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[2]));
        else if ((50.0f * hpMax)/100.0f >= hp) menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[1]));
        else menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleHPBarTextures[0]));
        sprite->mWidth = width;
    } else winSpriteSetDisp(sprite,0);
}
void menuCB_BattleBA_FacePanelSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot)
{
    if (menuCBPokemonGetDataId(trainer,slot)) {
        winSpriteSetDisp(sprite,1);
        if (menuCBBattleStartGetStatus() == 0) menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleFacePanelTextures[1]));
        else menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleFacePanelTextures[0]));
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
            if (_CB.skipBattleEffect == 0) {
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
    u16 item = toolentryTaisenGetBattlePlayerID(trainer);
    menuCBSpriteSetData(sprite,menuSpriteBiosGetPtr(battleXPTextures[item]));
}
}

#include <menuCBBattleStart.h>

extern "C" {
GSgfxVF *GSgfxVFBuildTemp(s32, s32, s32, s32, s32, s32, s32, s32, s32);

void menuCBBattleStartDispTrainerFaceSub(tagWINDOW_WORK *window,
                                      tagSPRITE_WORK *sprite, s32 trainer)
{
    GStexture *texture;

    u8 alpha = window->mRgba.mColorA;
    GSgfxLayer &layer = *GSgfxLayerFindByID((GSgfxLayerID)window->mLayerID);
    MenuCBBattleFace &face = _menuCBBattleStartWork.face[trainer];
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
        layer.endSurface();
    }
}
}
