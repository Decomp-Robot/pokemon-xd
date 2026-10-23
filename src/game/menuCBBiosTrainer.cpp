#include <heroBios.h>
#include <FightRuleData.h>
#include <LooseBallAnims.h>
#include <Pokemon.h>
#include <fightTrainer.h>
#include <hero.h>
#include <menuCBBios.h>
#include <types.h>

/*
 * menuCBBiosTrainer.cpp
 *
 * The trainer records of the link (GBA) battle bios: one FIGHT_TRAINER per
 * controller port, the "fight immediately" opponents of the connect-battle
 * setup, and the accessors the battle menus share. The rule data of the same
 * work area lives in game/menuCBBiosRule.cpp. The entry points the menus call
 * are C functions.
 */

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
void *memcpy(void *dst, const void *src, unsigned long count);
void *memset(void *dst, int val, unsigned long count);

void heroBiosCopy(Hero *dst, Hero *src);
void heroBiosSetHizukiFlag(Hero *hero, s32 flag);
u8 heroBiosGetHomePlace(Hero *hero);
u8 heroBiosGetSexDataId(Hero *hero);
u8 menuCBRule_IsBlankPokemon(Pokemon *pokemon);
void pokemonAllKaihuku(Pokemon *pokemon);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

extern "C" void menuCBBios_SetEncountFlag(u32 flag);
extern "C" s32 menuCBBios_GetNameEntryMode(void);
extern "C" u16 *menuCBBios_GetRuleEntryList(s32 ruleId);
extern "C" void menuCBBios_SetTrainerHomePlace(FIGHT_TRAINER *trainer,
                                               u32 homePlace);
extern "C" u32 menuCBBios_GetTrainerHomePlace(FIGHT_TRAINER *trainer);
extern "C" void menuCBBios_CopyTrainer(FIGHT_TRAINER *dst,
                                       const FIGHT_TRAINER *src);
extern "C" void menuCBBios_InitVersusTrainer(FIGHT_TRAINER *trainer,
                                             Hero *hero);
extern "C" void menuCBBios_InitTrainerFromHero(FIGHT_TRAINER *trainer,
                                               Hero *hero, u16 dataId);
extern "C" void menuCBBios_InitTrainerFromDataID(FIGHT_TRAINER *trainer,
                                                 u16 dataId, u16 encountNo);
extern "C" void menuCBBios_ClearTrainer(FIGHT_TRAINER *trainer);
extern "C" void menuCBBios_InitTrainer(FIGHT_TRAINER *trainer, u16 dataId);
extern "C" s32 menuCBBios_FightTrainerDataIDToHomePlace(u16 dataId);
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerFromVersusBattleID(s32 battleId);
extern "C" s32 menuCBBios_GetBattlePortID(void);
extern "C" Hero *menuCBBios_GetBattleHeroPtr(void);
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainer(s32 controllerId);
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerImmediately(s32 index);
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerByPort(s32 index);
extern "C" Hero *menuCBBios_GetHeroImmediately(s32 index);
void _menuCBBios_RecoverAllPokemons(Hero *hero);
extern "C" s32 menuCBBios_ControlerIDtoPortID(s32 controllerId);

/* -------------------------------------------------------------------------
 * Encounter flag and work-area accessors
 * ---------------------------------------------------------------------- */

extern "C" void menuCBBios_SetEncountFlag(u32 flag)
{
    staEncountFlag = flag;
}

extern "C" s32 menuCBBios_GetEncountFlag(void)
{
    return staEncountFlag;
}

extern "C" s32 menuCBBios_GetNameEntryMode(void)
{
    return menuCBTaisenWork.nameEntryMode;
}

// Rule entry list of rule set `ruleId`; the colosseum screens walk it as u16
// entries.
extern "C" u16 *menuCBBios_GetRuleEntryList(s32 ruleId)
{
    return menuCBTaisenWork.mRulesetData[ruleId].m1E;
}

/* -------------------------------------------------------------------------
 * Trainer player ID
 * ---------------------------------------------------------------------- */

extern "C" u16 menuCBTrainerBios_GetPlayerID(FIGHT_TRAINER *trainer)
{
    return trainer->mResetDataId;
}

/* -------------------------------------------------------------------------
 * Trainer records
 * ---------------------------------------------------------------------- */

extern "C" void menuCBBios_SetTrainerHomePlace(FIGHT_TRAINER *trainer,
                                               u32 homePlace)
{
    trainer->mHomePlace = homePlace;
}

extern "C" u32 menuCBBios_GetTrainerHomePlace(FIGHT_TRAINER *trainer)
{
    return trainer->mHomePlace;
}

extern "C" void menuCBBios_CopyTrainer(FIGHT_TRAINER *dst,
                                       const FIGHT_TRAINER *src)
{
    memcpy(dst, src, 0x1320);
}

extern "C" s32 menuCBTrainerBios_GetControlerID(FIGHT_TRAINER *trainer)
{
    return trainer->mControllerId;
}

extern "C" void menuCBTrainerBios_SetControlerID(FIGHT_TRAINER *trainer, s32 controllerId)
{
    trainer->mControllerId = controllerId;
}

/*
 * Set up a versus trainer record from the hero of controller port `hero`:
 * the trainer data ID follows the hero's home place (and sex inside the
 * home), so every hero keeps its own trainer face.
 */
extern "C" void menuCBBios_InitVersusTrainer(FIGHT_TRAINER *trainer,
                                             Hero *hero)
{
    u16 dataId = 0x1388;

    switch (heroBiosGetHomePlace(hero)) {
    case 0:
        dataId = 0x1388;
        break;
    case 1:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x1389;
            break;
        case 1:
            dataId = 0x138A;
            break;
        case 2:
            break;
        }
        break;
    case 2:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x138B;
            break;
        case 1:
            dataId = 0x138C;
            break;
        case 2:
            break;
        }
        break;
    case 3:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x138D;
            break;
        case 1:
            dataId = 0x138E;
            break;
        case 2:
            break;
        }
        break;
    }

    menuCBBios_InitTrainerFromHero(trainer, hero, dataId);
}

/*
 * Fill a trainer record from `hero`: reset it, then store the hero in both
 * the entry and the battle bios slots and book the trainer data ID and its
 * home place.
 */
extern "C" void menuCBBios_InitTrainerFromHero(FIGHT_TRAINER *trainer,
                                               Hero *hero, u16 dataId)
{
    Hero work;

    heroBiosCopy(&work, hero);
    heroBiosSetHizukiFlag(&work, 0);
    _menuCBBios_RecoverAllPokemons(&work);
    menuCBBios_ClearTrainer(trainer);
    heroBiosCopy(&trainer->mHero, &work);
    heroBiosCopy(&trainer->mBattleHero, &work);
    trainer->mDataId = dataId;
    trainer->mHomePlace = menuCBBios_FightTrainerDataIDToHomePlace(dataId);
}

/*
 * Fill a trainer record with a fresh hero generated for trainer data ID
 * `dataId`, fought with the record's battle hero as the base.
 */
extern "C" void menuCBBios_InitTrainerFromDataID(FIGHT_TRAINER *trainer,
                                                 u16 dataId, u16 encountNo)
{
    Hero work;

    heroBiosCopy(&work, &trainer->mBattleHero);
    fightTrainerCreateFightTrainerDataIdToHero(
        dataId, LooseBallAnims::GetDesperationInfo(0), &work, encountNo);
    heroBiosSetHizukiFlag(&work, 0);
    _menuCBBios_RecoverAllPokemons(&work);
    menuCBBios_ClearTrainer(trainer);
    heroBiosCopy(&trainer->mHero, &work);
    heroBiosCopy(&trainer->mBattleHero, &work);
    trainer->mDataId = dataId;
    trainer->mHomePlace = menuCBBios_FightTrainerDataIDToHomePlace(dataId);
}

// Reset a trainer record to its default trainer data ID.
extern "C" void menuCBBios_ClearTrainer(FIGHT_TRAINER *trainer)
{
    u16 defaultDataId;

    defaultDataId = menuCBTrainerBios_GetPlayerID(trainer);
    menuCBBios_InitTrainer(trainer, defaultDataId);
}

extern "C" void menuCBBios_InitTrainer(FIGHT_TRAINER *trainer, u16 dataId)
{
    memset(trainer, 0, 0x1320);
    trainer->mResetDataId = dataId;
}

// Home place a fight trainer data ID plays: the hero trainer (0x1388) has
// none, data IDs 1-5 and the versus pairs 0x1389/0x138A come from home 1,
// 0x138B/0x138C from home 2 and 0x138D/0x138E from home 3.
extern "C" s32 menuCBBios_FightTrainerDataIDToHomePlace(u16 dataId)
{
    switch (dataId) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x1389:
    case 0x138A:
        return 1;

    case 0x138B:
    case 0x138C:
        return 2;

    case 0x138D:
    case 0x138E:
        return 3;

    case 0x1388:
        return 0;

    default:
        return 0;
    }
}

/*
 * Trainer record a versus battle ID belongs to: the single-battle modes keep
 * it in the port slot named by the ID, the versus modes scan the records'
 * booked versus battle IDs.
 */
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerFromVersusBattleID(s32 battleId)
{
    s32 i;

    switch (menuCBTaisenWork.battleMode) {
    case 0:
    case 1:
        if (battleId >= 0 && battleId <= 1) {
            return &menuCBTaisenWork.mTrainer[battleId];
        }
        break;

    case 2:
    default:
        for (i = 0; i < 4; i++) {
            if (battleId == menuCBTaisenWork.mTrainer[i].mVersusBattleId) {
                return &menuCBTaisenWork.mTrainer[i];
            }
        }
        break;
    }

    return 0;
}

// Battle participant accessors of the connect-battle setup, answered by the
// battle menus' own state (currently 0 in this build's flow): GetBattlePortID
// is matched by heroMoveGetKenObjID against the per-participant object-ID
// table to pick the participant slot, and GetBattleHeroPtr is the Hero* the
// pokemon menus use on the connect-battle branch (alongside
// savedataGetStatus(0, 2) and
// fightFloorGetGcHeroFightTrainerPtr) before reading its gamedata record.
extern "C" s32 menuCBBios_GetBattlePortID(void)
{
    return 0;
}

extern "C" Hero *menuCBBios_GetBattleHeroPtr(void)
{
    return 0;
}

// Trainer record of the port `controllerId` is plugged into.
extern "C" FIGHT_TRAINER *menuCBBios_GetTrainer(s32 controllerId)
{
    s32 port = menuCBBios_ControlerIDtoPortID(controllerId);

    if (port < 0) {
        return 0;
    }
    return &menuCBTaisenWork.mTrainer[port];
}

extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerImmediately(s32 index)
{
    if (index < 0 || index >= 4) {
        return 0;
    }
    return &menuCBTaisenWork.mTrainerImmediately[index];
}

extern "C" FIGHT_TRAINER *menuCBBios_GetTrainerByPort(s32 index)
{
    if (index < 0 || index >= 4) {
        return 0;
    }
    return &menuCBTaisenWork.mTrainer[index];
}

extern "C" Hero *menuCBBios_GetHeroImmediately(s32 index)
{
    if (index < 0 || (u32)index >= 4) {
        return 0;
    }
    return &menuCBTaisenWork.mTrainer[index].mHero;
}

// Refresh every pokemon of `hero` that is not an empty slot.
void _menuCBBios_RecoverAllPokemons(Hero *hero)
{
    u16 slot;
    Pokemon *pokemon;

    for (slot = 0; slot < 6; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, slot);
        if (menuCBRule_IsBlankPokemon(pokemon) == 0) {
            pokemonAllKaihuku(pokemon);
        }
    }
}

/* -------------------------------------------------------------------------
 * Controller mapping
 * ---------------------------------------------------------------------- */

// Port a controller ID is plugged into, or -1 when it is unknown.
extern "C" s32 menuCBBios_ControlerIDtoPortID(s32 controllerId)
{
    s32 port;

    for (port = 0; port < 4; port++) {
        if (controllerId == staControlerMap[port]) {
            return port;
        }
    }
    return -1;
}
