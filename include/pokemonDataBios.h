#ifndef POKEMONDATABIOS_H
#define POKEMONDATABIOS_H
#include <types.h>
#include <PokemonData.h>

#ifdef __cplusplus
extern "C" {
#endif
u32 pokemonDataBiosGetName(PokemonData *data);
u32 pokemonDataBiosGetDoc(PokemonData *data);
u32 pokemonDataBiosGetTypeName(PokemonData *data);
u32 pokemonDataBiosGetTukamaetaFlag(PokemonData *data);
u32 pokemonDataBiosGetMitaFlag(PokemonData *data);
s32 pokemonDataBiosGetVoice(PokemonData *data);
u16 pokemonDataBiosGetWeight(PokemonData *data);
u16 pokemonDataBiosGetHeight(PokemonData *data);
u8 pokemonDataBiosGetColor(PokemonData *data, u16 index);
u32 pokemonDataBiosGetStatusFaceMenuSpriteId(PokemonData *data, u16 index);
u32 pokemonDataBiosGetPokebodyId(PokemonData *data, u16 index);
PokemonBody *pokemonDataBiosGetCgDataPtr(PokemonData *data, u16 index);
u8 pokemonDataBiosGetWazaMcn(PokemonData *data, u16 index);
u16 pokemonDataBiosGetGetWazaDataId(PokemonData *data, u16 index);
u8 pokemonDataBiosGetGetWazaLevel(PokemonData *data, u16 index);
PokemonLevelUpMove *pokemonDataBiosGetGetWazaPtr(PokemonData *data, u16 index);
u16 pokemonDataBiosGetSinkaPokemonDataId(PokemonData *data, u16 index);
u16 pokemonDataBiosGetSinkaBuff(PokemonData *data, u16 index);
u8 pokemonDataBiosGetSinkaKind(PokemonData *data, u16 index);
PokemonEvolution *pokemonDataBiosGetSinkaPtr(PokemonData *data, u16 index);
u8 pokemonDataBiosGetTokuseiDataId(PokemonData *data, u16 index);
u32 pokemonDataBiosGetZokuseiDataId(PokemonData *data, u16 index);
u16 pokemonDataBiosGetItemDataId(PokemonData *data, u16 index);
u16 pokemonDataBiosGetInitFriend(PokemonData *data);
u8 pokemonDataBiosGetSexRatio(PokemonData *data);
u8 pokemonDataBiosGetGet(PokemonData *data);
u8 pokemonDataBiosGetGrowDataId(PokemonData *data);
u16 pokemonDataBiosGetGiveExp(PokemonData *data);
u16 pokemonDataBiosGetGiveNimblenessEffort(PokemonData *data);
u16 pokemonDataBiosGetGiveSpeDefEffort(PokemonData *data);
u16 pokemonDataBiosGetGiveSpeAtkEffort(PokemonData *data);
u16 pokemonDataBiosGetGivePhyDefEffort(PokemonData *data);
u16 pokemonDataBiosGetGivePhyAtkEffort(PokemonData *data);
u16 pokemonDataBiosGetGiveMaxHpEffort(PokemonData *data);
u16 pokemonDataBiosGetBasisNimbleness(PokemonData *data);
u16 pokemonDataBiosGetBasisSpeDef(PokemonData *data);
u16 pokemonDataBiosGetBasisSpeAtk(PokemonData *data);
u16 pokemonDataBiosGetBasisPhyDef(PokemonData *data);
u16 pokemonDataBiosGetBasisPhyAtk(PokemonData *data);
u16 pokemonDataBiosGetBasisMaxHp(PokemonData *data);
PokemonBaseStats *pokemonDataBiosGetBasisStatusPtr(PokemonData *data, u16 index);
PokemonData *pokemonDataBiosGetPtr(u16 species);
#ifdef __cplusplus
}
#endif
#endif
