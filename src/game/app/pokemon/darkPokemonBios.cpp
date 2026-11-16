#include <darkPokemon.h>
#include <deck.h>
#include <fightFloor.h>
#include <hero.h>
#include <savedata.h>

// Floor ID the script capture command stamps when it registers a Shadow
// Pokemon.  That path already assigns the meet index, so a record carrying
// this floor must not be handed a second one here.
const u16 DARK_POKEMON_CAPTURE_FLOOR_ID = 181;

extern "C" {

u16 darkPokemonListBiosGetPokemonID(u16 darkID)
{
    u16 *pokemonID = darkPokemonListBiosGetDPList(darkID);
    if (pokemonID != 0) {
        return *pokemonID;
    }
    return 0;
}

u16 *darkPokemonListBiosGetDPList(u16 darkID)
{
    if (darkID >= darkPokemonList_number) {
        return 0;
    }
    return &darkPokemonList[darkID];
}

u32 darkpokemonBiosGetScenarioNum(void)
{
    u16 i;
    u32 num = 0;

    for (i = 0; i < deckGetDarkPokemonSize(); i++) {
        DeckDarkPokemon *darkPokemon = deckGetDeckDarkPokemon(i);
        if (darkPokemon != 0 && darkPokemon->isScenarioPokemon()) {
            num++;
        }
    }
    return num;
}

u16 darkPokemonBiosGetSteps(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return 0;
    }
    return darkPokemon->getWalkCount();
}

void darkPokemonBiosSetSteps(u16 darkID, u16 steps)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setWalkCount(steps);
}

void darkPokemonBiosSetInfomation(u16 darkID, u16 information)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setSnatchInfoLevel(information);
}

u32 darkPokemonBiosGetRnd(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getRnd();
    }
    return 0;
}

void darkPokemonBiosSetRnd(u16 darkID, u32 rnd)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setRnd(rnd);
}

s32 darkPokemonBiosGetStatusRnd(u16 darkID, u16 status)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getStatusRnd(status);
    }
    return 0;
}

void darkPokemonBiosSetStatusRnd(u16 darkID, u16 status, u16 rnd)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setStatusRnd(status, rnd);
}

u8 darkPokemonBiosGetFightFlag(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0 && darkPokemon->getStatus() >= DARK_POKEMON_STATUS_FIGHT) {
        return 1;
    }
    return 0;
}

void darkPokemonBiosSetFightFlag(u16 darkID)
{
    u32 floorID;
    u32 catchFloorID;
    DarkPokemon *darkPokemon;
    Hero *hero;
    u8 index;

    darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }

    floorID = fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_ID, 0);
    catchFloorID = darkPokemon->getCatchFloorId();

    if (darkPokemon->getStatus() == DARK_POKEMON_STATUS_NONE &&
        catchFloorID != DARK_POKEMON_CAPTURE_FLOOR_ID) {
        hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
        index = hero->getMeetDarkPokemonCount();
        hero->setMeetDarkPokemonCount(index + 1);
        darkPokemon->setSnatchIndex(index);
        darkPokemon->addStatus(DARK_POKEMON_STATUS_FIGHT);
        darkPokemon->setCatchFloorId(floorID);
        darkPokemon->setUpdate(true);
    }
    if (catchFloorID == DARK_POKEMON_CAPTURE_FLOOR_ID) {
        darkPokemon->addStatus(DARK_POKEMON_STATUS_FIGHT);
        darkPokemon->setCatchFloorId(floorID);
        darkPokemon->setUpdate(true);
    }
}

u8 darkPokemonBiosGetSnatchSuccessFlag(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0 && darkPokemon->getStatus() >= DARK_POKEMON_STATUS_SNATCHED) {
        return 1;
    }
    return 0;
}

u8 darkPokemonBiosGetReliveFlag(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0 && darkPokemon->getStatus() == DARK_POKEMON_STATUS_RELIVED) {
        return 1;
    }
    return 0;
}

void darkPokemonBiosSetReliveFlag(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setStatus(DARK_POKEMON_STATUS_RELIVED);
}

u8 darkPokemonBiosGetInformationFlag(u16 darkID)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->isInformation();
    }
    return 0;
}

void darkPokemonBiosSetInformationFlag(u16 darkID, bool information)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(darkID);
    if (darkPokemon == 0) {
        return;
    }
    darkPokemon->setInformation(information);
}

u16 darkPokemonBiosGetTemotiDarkWaza(u16 darkID, u16 slot)
{
    DeckDarkPokemon *darkPokemon = deckGetDeckDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getDarkWaza(slot);
    }
    return 0;
}

u8 darkPokemonBiosGetLevel(u16 darkID)
{
    DeckDarkPokemon *darkPokemon = deckGetDeckDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getPlayerLevel();
    }
    return 0;
}

u8 darkPokemonBiosGetHokakuritu(u16 darkID)
{
    DeckDarkPokemon *darkPokemon = deckGetDeckDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getSnatchRatio();
    }
    return 0;
}

u16 darkPokemonBiosGetDarkPoint(u16 darkID)
{
    return darkPokemonBiosGetInitDarkPoint(darkID);
}

u16 darkPokemonBiosGetInitDarkPoint(u16 darkID)
{
    DeckDarkPokemon *darkPokemon = deckGetDeckDarkPokemon(darkID);
    if (darkPokemon != 0) {
        return darkPokemon->getInitDarkPoint();
    }
    return 0;
}

}
