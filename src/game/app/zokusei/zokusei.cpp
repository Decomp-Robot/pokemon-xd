#include <relglobal.h>
#include <fightSeq.h>
#include <waza.h>
#include <zokusei.h>

// Direct wrapper around the BIOS type-matchup table.
u16 zokuseiGetWazaJoutai(u16 zokuseiDataId, u16 defenseZokuseiDataId)
{
    return zokuseiBiosGetWazaJoutai(zokuseiDataId, defenseZokuseiDataId);
}

// Overall result of an attacking type against both types of a species.
static inline u16 zokuseiGetWazaJoutaiAry(u16 zokuseiDataId, u16 *defenseZokuseiAry, u16 count)
{
    u16 j;
    u16 superCount = 0;
    u16 notVeryCount = 0;

    for (j = 0; j < count; j++) {
        u16 defenseZokuseiDataId = defenseZokuseiAry[j];
        u16 joutai;

        if (defenseZokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
        joutai = zokuseiGetWazaJoutai(zokuseiDataId, defenseZokuseiAry[j]);
        if (joutai == JOUTAI_MOVE_RESULT_NO_EFFECT) return JOUTAI_MOVE_RESULT_NO_EFFECT;
        if (joutai == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
            superCount++;
        } else if (joutai == JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE) {
            notVeryCount++;
        }
    }

    if (superCount == 0 && notVeryCount == 0) return JOUTAI_MOVE_RESULT_NORMAL;
    if (superCount == notVeryCount) return JOUTAI_MOVE_RESULT_NORMAL;
    if (superCount > notVeryCount) return JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE;
    return JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE;
}

u16 zokuseiGetWazaJoutai_PokemonId(u16 zokuseiDataId, u16 pokemonId)
{
    u16 defenseZokuseiAry[POKEMON_DATA_TYPE_COUNT];
    u16 count;
    u8 i;

    if (pokemonId > *pokemon_data_number) return 0;
    count = 0;
    for (i = 0; i < POKEMON_DATA_TYPE_COUNT; i++) {
        u16 defenseZokuseiDataId = pokemonDB_GetZokuseiDataId(pokemonId, i);

        if (defenseZokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
        defenseZokuseiAry[count] = defenseZokuseiDataId;
        count++;
    }

    return zokuseiGetWazaJoutaiAry(zokuseiDataId, defenseZokuseiAry, count);
}

// Overall result of an attacking type against an array of defending types.
u16 zokuseiGetWazaJoutaiAsDefenseZokuseiAry(u16 zokuseiDataId, u16 *defenseZokuseiAry, u16 count)
{
    u16 i;
    u16 defenseZokuseiDataId;
    u16 biosJoutai;
    u16 joutai;
    u16 superCount = 0;
    u16 notVeryCount = 0;

    for (i = 0; i < count; i++) {
        defenseZokuseiDataId = defenseZokuseiAry[i];
        if (defenseZokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
        biosJoutai = zokuseiBiosGetWazaJoutai(zokuseiDataId, defenseZokuseiAry[i]);
        joutai = biosJoutai;
        if (joutai == JOUTAI_MOVE_RESULT_NO_EFFECT) return JOUTAI_MOVE_RESULT_NO_EFFECT;
        if (joutai == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
            superCount++;
        } else if (joutai == JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE) {
            notVeryCount++;
        }
    }

    if (superCount == 0 && notVeryCount == 0) return JOUTAI_MOVE_RESULT_NORMAL;
    if (superCount == notVeryCount) return JOUTAI_MOVE_RESULT_NORMAL;
    if (superCount > notVeryCount) return JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE;
    return JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE;
}

// Sum of the type advantages of one type over the whole type chart.
// direction 1 scores the specified type as the attacker; otherwise as the defender.
// filter 1 keeps only positive contributions; filter 2 keeps only negative ones.
s32 zokuseiGetAisyouValue(u16 zokuseiDataId, u8 direction, u8 filter)
{
    const u8 asAttacker = 1;
    const u8 positiveOnly = 1;
    const u8 negativeOnly = 2;
    s32 value = 0;
    u16 i;

    for (i = 0; i < *zokusei_data_number; i++) {
        u16 joutai;

        if (i == ZOKUSEI_DATA_ID_MYSTERY) continue;
        if (direction == asAttacker) {
            joutai = zokuseiGetWazaJoutai(zokuseiDataId, i);
            if (joutai == JOUTAI_MOVE_RESULT_NO_EFFECT) {
                if (filter != positiveOnly) value -= 10;
            } else if (joutai == JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE) {
                if (filter != positiveOnly) value -= 10;
            } else if (joutai == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
                if (filter != negativeOnly) value += 10;
            }
        } else {
            joutai = zokuseiGetWazaJoutai(i, zokuseiDataId);
            if (joutai == JOUTAI_MOVE_RESULT_NO_EFFECT) {
                if (filter != negativeOnly) value += 10;
            } else if (joutai == JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE) {
                if (filter != negativeOnly) value += 10;
            } else if (joutai == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
                if (filter != positiveOnly) value -= 10;
            }
        }
    }
    return value;
}

// Sprite of one of a species' two types, or 0 when the slot is unused.
u16 zokuseiGetPokemonSpriteID(POKEMON_DATA_dd_ *data, u16 index)
{
    if (index >= POKEMON_DATA_TYPE_COUNT) return 0;
    return zokuseiBiosGetSpriteID((u8)pokemonDataBiosGetZokuseiDataId(data, index));
}
