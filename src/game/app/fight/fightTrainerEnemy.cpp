#include "app/fight/fightTrainerEnemy.h"

// Drops the record of the given fight entry. The slot keeps its position and
// simply reads back as free afterwards.
bool fightTrainerEnemyPokemonEraseAry(FightTrainerEnemyPokemon *ary, u16 num, s16 fightEntryeId)
{
    FightTrainerEnemyPokemon *pokemon;

    if (ary == 0) {
        return 0;
    }
    if (fightEntryeId < 0) {
        return 0;
    }
    pokemon = fightTrainerEnemyPokemonSearchAry(ary, num, fightEntryeId);
    if (pokemon == 0) {
        return 0;
    }
    fightTrainerEnemyPokemonInit(pokemon);
    return 1;
}

// Registers one more enemy Pokemon in the array; false when the entry already
// has a record or the array is full.
bool fightTrainerEnemyPokemonRegistAry(FightTrainerEnemyPokemon *ary, u16 num, s16 fightEntryeId)
{
    FightTrainerEnemyPokemon *pokemon;

    if (ary == 0) {
        return 0;
    }
    if (fightEntryeId < 0) {
        return 0;
    }
    if (fightTrainerEnemyPokemonSearchAry(ary, num, fightEntryeId) != 0) {
        return 0;
    }
    pokemon = fightTrainerEnemyPokemonSearchAry(ary, num, -1);
    if (pokemon == 0) {
        return 0;
    }
    fightTrainerEnemyPokemonCreate(pokemon, fightEntryeId);
    return 1;
}

// Finds the record of the given fight entry; a negative id asks for the first
// free record instead.
FightTrainerEnemyPokemon *fightTrainerEnemyPokemonSearchAry(FightTrainerEnemyPokemon *ary, u16 num,
                                                           s16 fightEntryeId)
{
    FightTrainerEnemyPokemon *pokemon;
    u16 i;

    if (ary == 0) {
        return 0;
    }
    for (i = 0; i < num; i++) {
        pokemon = &ary[i];
        if (fightEntryeId < 0) {
            if (!fightTrainerEnemyPokemonCheckValid(pokemon)) {
                return pokemon;
            }
        } else {
            if (fightTrainerEnemyPokemonCheckValid(pokemon) &&
                fightTrainerEnemyPokemonBiosGetFightEntryeId(pokemon) == fightEntryeId) {
                return pokemon;
            }
        }
    }
    return 0;
}

// A record is in use as soon as it carries a fight entry id.
bool fightTrainerEnemyPokemonCheckValid(FightTrainerEnemyPokemon *pokemon)
{
    if (pokemon == 0) {
        return 0;
    }
    return fightTrainerEnemyPokemonBiosGetFightEntryeId(pokemon) >= 0;
}

void fightTrainerEnemyPokemonCreate(FightTrainerEnemyPokemon *pokemon, s16 fightEntryeId)
{
    if (pokemon == 0) {
        return;
    }
    if (fightEntryeId < 0) {
        return;
    }
    fightTrainerEnemyPokemonInit(pokemon);
    fightTrainerEnemyPokemonBiosSetFightEntryeId(pokemon, fightEntryeId);
}

void fightTrainerEnemyPokemonInitAry(FightTrainerEnemyPokemon *ary, u16 num)
{
    u16 i;

    if (ary == 0) {
        return;
    }
    for (i = 0; i < num; i++) {
        fightTrainerEnemyPokemonInit(&ary[i]);
    }
}

// Resets the record to the state of an unused enemy Pokemon: no fight entry,
// no moves, no ability and no fight-out ranking flags.
void fightTrainerEnemyPokemonInit(FightTrainerEnemyPokemon *pokemon)
{
    u8 i;

    fightTrainerEnemyPokemonBiosSetFightEntryeId(pokemon, -1);
    fightTrainerEnemyPokemonInitFightStatus(pokemon);
    fightTrainerEnemyPokemonInitFightOutStatus(pokemon);
    fightTrainerEnemyPokemonBiosSetStoreWazaNum(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetExpectWazaNum(pokemon, 0);
    for (i = 0; i < 4; i++) {
        fightTrainerEnemyPokemonBiosSetStoreWazaDataId(pokemon, i, 0);
    }
    for (i = 0; i < 8; i++) {
        fightTrainerEnemyPokemonBiosSetExpectWazaDataId(pokemon, i, 0);
    }
    fightTrainerEnemyPokemonBiosSetTokuseiFlag(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetStoreTokuseiData(pokemon, 0);
}

void fightTrainerEnemyPokemonInitFightOutStatus(FightTrainerEnemyPokemon *pokemon)
{
    fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(pokemon, 0);
    fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(pokemon, 0);
}

void fightTrainerEnemyPokemonInitFightStatus(FightTrainerEnemyPokemon *pokemon)
{
    u8 i;

    for (i = 0; i < 4; i++) {
        pokemon->storeWazaDataId[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        pokemon->expectWazaDataId[i] = 0;
    }
    pokemon->storeWazaNum = 0;
    pokemon->expectWazaNum = 0;
    pokemon->tokuseiFlag = 0;
    pokemon->storeTokuseiData = 0;
}
