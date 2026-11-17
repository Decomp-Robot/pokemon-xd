#include <fightPokemon.h>

extern "C" {

void fightPokemonSetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u32 buff)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return;
    }
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetBuff(pokemon, dataId, buff);
    }
}

u32 fightPokemonGetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetBuff(pokemon, dataId);
}

void fightPokemonCopyJoutaiDataId(FightPokemon *destination, FightPokemon *source, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonCopyJoutaiDataId(fightPokemonGetPokemonPtr(destination),
                               fightPokemonGetPokemonPtr(source), dataId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiCopy(destination, source, dataId);
    }
}

void fightPokemonSetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 idBuff)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return;
    }
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetIdBuff(pokemon, dataId, idBuff);
    }
}

u16 fightPokemonGetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetIdBuff(pokemon, dataId);
}

u16 fightPokemonGetUserFightTargetIdJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetUserFightTargetId(pokemon, dataId);
}

void fightPokemonSetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 kaisuu)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, kaisuu);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetKaisuuJoutaiDataId(pokemon, dataId, kaisuu);
    }
}

void fightPokemonSetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 nowKaisuu)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetNowKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, nowKaisuu);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetNowKaisuuJoutaiDataId(pokemon, dataId, nowKaisuu);
    }
}

s8 fightPokemonGetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetNowKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetNowKaisuuJoutaiDataId(pokemon, dataId);
}

s8 fightPokemonGetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetKaisuuJoutaiDataId(pokemon, dataId);
}

void fightPokemonSetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId, s16 count)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetCountAsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, count);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetCountAsJoutaiDataId(pokemon, dataId, count);
    }
}

s16 fightPokemonGetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetCountAsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetCountAsJoutaiDataId(pokemon, dataId);
}

void fightPokemonWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 targetId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonWriteJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, targetId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiWriteJoutaiDataId(pokemon, dataId, targetId);
    }
}

u8 fightPokemonCheckWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonCheckWriteJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiCheckWriteJoutaiDataId(pokemon, dataId);
}

u8 fightPokemonIsJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (pokemon == 0) {
        return 0;
    }

    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonIsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiIsJoutaiDataId(pokemon, dataId);
}

void fightPokemonInitJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonInitJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiInitJoutaiDataId(pokemon, dataId);
    }
}

} // extern "C"
