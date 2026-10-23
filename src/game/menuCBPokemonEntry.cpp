/*
 * menuCBPokemonEntry.cpp
 *
 * The Pokemon entry screen of the link (GBA) battles: the face texture work
 * that walks the entered parties, the per-port entry flags and the layout
 * the versus battle asks the screen for.
 */

#include <menuBBS.h>
#include <menuCBBattleStart.h>
#include <menuCBBios.h>
#include <menuCBRule.h>
#include <Pokemon.h>
#include <types.h>
#include <winSprite.h>

/* -------------------------------------------------------------------------
 * External data and functions
 * ---------------------------------------------------------------------- */

extern MenuCBPokemonEntryWork menuCBPokemonEntryWork;

/* Face ID the face loader keeps for the slot it loaded last. */
extern u16 menuCBPokemonEntryFaceID;

extern "C" {
extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetBattleVersus(void);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern u16 toolentryTaisenGetEntryPokemonNum(long port);
extern Pokemon *toolentryTaisenGetPokemonPtr(long port, long slot);
extern Pokemon *toolentryTaisenGetEntryPokemonPtr(long port, long slot);
extern s32 menuCBBattleStartGetStatus(void);
/* Face ID of a Pokemon: the face table entry is 16 bit wide, which is what
   menuFaceGetIDSub hands back. */
extern u16 menuFaceGetID2(Pokemon *pokemon, s32 flag);
/* Draws the face of a Pokemon by face ID. */
extern void menuFaceDisplay(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 faceID);
extern void menuFaceLoad_Tool(Pokemon *pokemon, Pokemon *(*callback)(void *), void *context);
extern FightRuleData *menuCBBios_CurrentRule(void);
extern u16 battlePokemonStatusGetHeldItem(Pokemon *pokemon);

}

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */

u8 _menuCBPokemonEntryGetEntryFlag(long port);
MenuCBPokemonEntryFaceWork *_menuCBPokemonEntryGetFaceBase(long player, long slot);

/* -------------------------------------------------------------------------
 * Face texture work
 * ---------------------------------------------------------------------- */

static Pokemon *_menuCBPokemonEntryLoadCallBack(void *arg);

s32 _menuCB_pokemonGetDeoxysSel(u16 port, bool flag);

/* The face texture work walks four parties of six slots each. */
#define FACE_WORK_SLOT_NUM 24

extern "C" void menuCBPokemonEntryTexWorkInit(void)
{
    MenuCBPokemonEntryFaceWork *faceRow;
    s32 player;
    s32 slot;

    menuCBPokemonEntryWork.mCursor = 0;
    menuCBPokemonEntryWork.mReadFlag = 0;

    for (player = 0; player < 4; player++) {
        faceRow = &menuCBPokemonEntryWork.mFaceWork[player][0];

        for (slot = 0; slot < 6; slot++) {
            MenuCBPokemonEntryFaceWork *face = &faceRow[slot];

            face->mLoaded = 0;
            face->mX = 0;
        }
    }

}

extern "C" u32 menuCBPokemonEntryGetReadFlag(void)
{
    return menuCBPokemonEntryWork.mReadFlag;
}

void _menuCBPokemonEntrySetEntryFlag(long port, bool flag)
{
    menuCBPokemonEntryWork.mEntryFlag[port] = flag;
}

u8 _menuCBPokemonEntryGetEntryFlag(long port)
{
    return menuCBPokemonEntryWork.mEntryFlag[port];
}

/* The 2D entry screen draws its party rows lower down than the others. */
void _menuCBPokemonEntryDispPosition(void)
{
    s32 battleType = toolentryTaisenGetBattleType();

    switch (battleType) {
    case BATTLE_TYPE_DOUBLE:
    case BATTLE_TYPE_SINGLE:
        menuCBPokemonEntryWork.mScroll = 0x136;
        break;

    case BATTLE_TYPE_MULTI:
        menuCBPokemonEntryWork.mScroll = 0;
        break;

    default:
        menuCBPokemonEntryWork.mScroll = 0;
        break;
    }
}

/* Whether every player of the battle has handed its party over. */
u8 _menuCBPokemonEntryAllPlayer(void)
{
    s32 playerNum = 1;
    s32 versus = toolentryTaisenGetBattleVersus();
    s32 battleType = toolentryTaisenGetBattleType();
    s32 port;

    switch (battleType) {
    case BATTLE_TYPE_DOUBLE:
    case BATTLE_TYPE_SINGLE:
        if (versus == 4) {
            playerNum = 2;
            menuCBPokemonEntryWork.mPlayerNum = 4;
        } else {
            playerNum = 2;
            menuCBPokemonEntryWork.mPlayerNum = versus;
        }

        break;

    case BATTLE_TYPE_MULTI:
        menuCBPokemonEntryWork.mPlayerNum = 4;
        playerNum = 4;
        break;

    default:
        break;
    }

    for (port = 0; port < playerNum; port++) {
        if (!_menuCBPokemonEntryGetEntryFlag(port)) {
            return 0;
        }
    }

    return 1;
}

extern "C" void menuCBPokemonEntryLoadTex(void)
{
    menuFaceLoad_Tool(0, _menuCBPokemonEntryLoadCallBack, 0);
}

/* Returns the Pokemon whose face it loaded last, 0 once the walk is over. */
Pokemon *_menuCBPokemonEntryLoadCallBack(void *arg)
{
    Pokemon *pokemon;
    s32 slot;
    s32 player;
    s32 keepLoading = 1;

    toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();

    pokemon = (Pokemon *)arg;

    if (arg != 0) {
        *(u8 *)arg = 1;
    }

    do {
        MenuCBPokemonEntryFaceWork *face;
        u16 faceID;

        if (menuCBPokemonEntryWork.mCursor == FACE_WORK_SLOT_NUM) {
            menuCBPokemonEntryWork.mReadFlag = 1;
            keepLoading = 0;
            pokemon = 0;
        } else {
            player = menuCBPokemonEntryWork.mCursor / 6;
            slot = menuCBPokemonEntryWork.mCursor % 6;

            if (menuCBBattleStartGetStatus() == 0) {
                pokemon = toolentryTaisenGetPokemonPtr(player, slot);
            } else if (toolentryTaisenGetEntryPokemonNum(player) <= slot) {
                pokemon = 0;
            } else {
                pokemon = toolentryTaisenGetEntryPokemonPtr(player, slot);
            }

            face = _menuCBPokemonEntryGetFaceBase(player, slot);
            faceID = menuFaceGetID2(pokemon, _menuCB_pokemonGetDeoxysSel(player, 1));

            if (faceID != 0 && pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0) != 0) {
                face->mLoaded = true;
                face->mFaceID = faceID;
                menuCBPokemonEntryFaceID = faceID;
                keepLoading = 0;
                menuCBPokemonEntryWork.mCursor = menuCBPokemonEntryWork.mCursor + 1;
            } else {
                face->mLoaded = false;
                menuCBPokemonEntryWork.mCursor = menuCBPokemonEntryWork.mCursor + 1;
            }
        }
    } while (keepLoading);

    return pokemon;
}

MenuCBPokemonEntryFaceWork *_menuCBPokemonEntryGetFaceBase(long player, long slot)
{
    return &menuCBPokemonEntryWork.mFaceWork[player][slot];
}

extern "C" void menuCBPokemonEntryTexWorkReInit(void)
{
    menuCBPokemonEntryTexWorkInit();
}

extern "C" u8 menuCBPokemonEntryDispPokemonFace(tagWINDOW_WORK *window,
                                                 tagSPRITE_WORK *sprite,
                                                 s32 player, s32 slot)
{
    MenuCBPokemonEntryFaceWork *face = _menuCBPokemonEntryGetFaceBase(player, slot);

    if (face->mLoaded) {
        menuFaceDisplay(window, sprite, face->mFaceID);
        return 1;
    }

    return 0;
}

extern "C" void menuCBEntrySlotConditionsSetTrainerRegulation(Hero *hero)
{
    FightRuleData *rule = menuCBBios_CurrentRule();
    u32 i;

    for (i = 0; i < 6; i++) {
        menuCBTaisenWork.entryFlags[0][i].insufficientPokemon =
            !menuCBRule_CheckTrainerRegulation(hero, rule, REGULATION_TRAINER_POKEMON_NUM);
    }
}

extern "C" void menuCBEntrySlotConditionsSetItemRegulation(Hero *hero,
                                                           Pokemon *pokemon, s32 slot)
{
    FightRuleData *rule = menuCBBios_CurrentRule();

    battlePokemonStatusGetHeldItem(pokemon);

    if (!menuCBRule_CheckPokemonRegulation(pokemon, rule, REGULATION_POKEMON_ITEM)) {
        switch (rule->mHeldItemMode) {
        case HELD_ITEMS_FORBIDDEN:
            menuCBTaisenWork.entryFlags[0][slot].heldItemForbidden = 1;
            break;

        case HELD_ITEMS_RESTRICTED:
            menuCBTaisenWork.entryFlags[0][slot].heldItemRestricted = 1;
            break;

        case HELD_ITEMS_ALLOWED:
            break;

        default:
            break;
        }
    }

    menuCBTaisenWork.entryFlags[0][slot].invalidHeldItem = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_HELD_ITEM);
    menuCBTaisenWork.entryFlags[0][slot].duplicateHeldItem =
        !menuCBRule_CheckTrainerRegulationAt(hero, pokemon, rule, REGULATION_TRAINER_SAME_ITEM);
}

extern "C" void menuCBEntrySlotConditionsBuild(Hero *hero, Pokemon *src, s32 index)
{
    FightRuleData *rule = menuCBBios_CurrentRule();

    menuCBTaisenWork.entryFlags[0][index].levelBelowMinimum =
        !menuCBRule_CheckPokemonRegulation(src, rule, REGULATION_POKEMON_LEVEL_MIN);
    menuCBTaisenWork.entryFlags[0][index].levelAboveMaximum =
        !menuCBRule_CheckPokemonRegulation(src, rule, REGULATION_POKEMON_LEVEL_MAX);
    menuCBTaisenWork.entryFlags[0][index].illegalPokemon = !menuCBRule_CheckPokemonKind(src, POKEMON_CONDITION_ILLEGAL);
    menuCBTaisenWork.entryFlags[0][index].shadowPokemon = !menuCBRule_CheckPokemonKind(src, POKEMON_CONDITION_SHADOW);
    menuCBTaisenWork.entryFlags[0][index].eggOrInvalidPokemon = !menuCBRule_CheckPokemonKind(src, POKEMON_CONDITION_TAMAGO);
    menuCBTaisenWork.entryFlags[0][index].invalidPokemon = !menuCBRule_CheckPokemonKind(src, POKEMON_CONDITION_VALID);
    menuCBTaisenWork.entryFlags[0][index].invalidMove = !menuCBRule_CheckPokemonKind(src, POKEMON_CONDITION_ILLEGAL_MOVE);
    menuCBTaisenWork.entryFlags[0][index].partyLevelSumExceeded =
        !menuCBRule_CheckTrainerRegulation(hero, rule, REGULATION_TRAINER_LEVEL_SUM);
    menuCBTaisenWork.entryFlags[0][index].legendPokemon = !menuCBRule_CheckLegendPokemon(src, rule);
    menuCBTaisenWork.entryFlags[0][index].duplicateSpecies =
        !menuCBRule_CheckTrainerRegulationAt(hero, src, rule, REGULATION_TRAINER_SAME_SPECIES);
}
