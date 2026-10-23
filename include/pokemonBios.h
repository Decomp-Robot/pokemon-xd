#ifndef POKEMON_BIOS_H
#define POKEMON_BIOS_H

#include <Pokemon.h>
#include <PokemonData.h>
#include <types.h>
typedef struct PokemonTokuseiData PokemonTokuseiData;

#ifdef __cplusplus
extern "C" {
#endif
void pokemonBiosSetFriend(Pokemon* pokemon, unsigned short level);
void pokemonBiosCopy(Pokemon* dst, Pokemon* src);

u16 pokemonBiosGetInitDp(Pokemon *pokemon);
u8 pokemon_GetFuseiFlag(Pokemon *pokemon);
u16 pokemonBiosGetFightTrainerPokemonDataId(Pokemon *pokemon);
u8 pokemonBiosGetPokerus(Pokemon *pokemon);
u8 pokemonBiosGetMailId(Pokemon *pokemon);
u8 pokemonBiosGetTokuseiFlag(Pokemon *pokemon);
#ifdef __cplusplus
void pokemonBiosSetTokuseiFlag(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetTokuseiFlag(Pokemon *pokemon, u8 flag);
#endif
s32 pokemonBiosGetAttest(Pokemon *pokemon);
u32 pokemonBiosGetRnd(Pokemon *pokemon);
u16 pokemonGetTokuseiDataId(Pokemon *pokemon);
PokemonTokuseiData *pokemonTokuseiDataBiosGetPtr(u16 dataID);
u32 pokemonTokuseiDataBiosGetName(PokemonTokuseiData *data);
u16 pokemonBiosGetHp(Pokemon *pokemon);
u16 pokemonBiosGetMaxHp(Pokemon *pokemon);
void pokemonBiosSetPhyAtk(Pokemon *pokemon, u16 value);
void pokemonBiosSetPhyDef(Pokemon *pokemon, u16 value);
void pokemonBiosSetSpeAtk(Pokemon *pokemon, u16 value);
void pokemonBiosSetSpeDef(Pokemon *pokemon, u16 value);
void pokemonBiosSetNimbleness(Pokemon *pokemon, u16 value);
u16 pokemon_GetPokemonDataId(Pokemon *pokemon);
u32 pokemonDB_GetName(u16 dataID);
u8 pokemonBiosGetTamagoFlag(Pokemon *pokemon);
void pokemonBiosSetNicknamePtr(Pokemon *pokemon, const u16 *name);

u8 pokemonBiosGetLevel(Pokemon *pokemon);
u16 pokemonGetSoubiItemDataId(Pokemon *pokemon);
u16 pokemonDoItemSoubi(Pokemon *pokemon, u16 itemID, u8 equip);
u16 pokemonBiosGetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u8 pokemonBiosGetPokemonWazaPp(Pokemon *pokemon, u16 slot);
u32 pokemonTokuseiDataBiosGetDoc(PokemonTokuseiData *data);
u8 pokemonBiosGetCatchBallId(Pokemon *pokemon);
unsigned long pokemonBiosGetCatchTrainerRnd(Pokemon *pokemon);
#ifdef __cplusplus
bool pokemonBiosGetDarkFlag(Pokemon *pokemon);
#else
u8 pokemonBiosGetDarkFlag(Pokemon *pokemon);
#endif
int pokemonBiosGetDp(Pokemon *pokemon);
u16 pokemonBiosGetFriend(Pokemon *pokemon);
#ifdef __cplusplus
}
#endif

#endif
