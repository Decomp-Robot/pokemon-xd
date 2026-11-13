#ifndef POKEMON_BIOS_H
#define POKEMON_BIOS_H

#include <Pokemon.h>
#include <PokemonData.h>
#include <types.h>
typedef struct PokemonTokuseiData PokemonTokuseiData;

// Ribbon word of a Pokemon record (0x7C). Each of the twelve contest ribbons
// takes one flag in the top twelve bits of the word; the amari ribbon count
// follows at bit 12 and spans eight bits, so it shares the second and third
// byte of the word.
typedef struct PokemonRibbon {
    u8 champ : 1;
    u8 winning : 1;
    u8 victory : 1;
    u8 bromide : 1;
    u8 ganba : 1;
    u8 marine : 1;
    u8 land : 1;
    u8 sky : 1;
    u8 country : 1;
    u8 national : 1;
    u8 earth : 1;
    u8 world : 1;
    u32 amari : 8;
} PokemonRibbon;

// Contest record of a Pokemon's expand status: the five contest categories in
// the order the status field ids select them. A Pokemon holds two of these
// records, one with the contest values themselves and one with the medals it
// won in each category.
typedef struct PokemonExpandStatus {
    u8 style; // 0x0
    u8 beautiful; // 0x1
    u8 cute; // 0x2
    u8 clever; // 0x3
    u8 strong; // 0x4
} PokemonExpandStatus;

#ifdef __cplusplus
extern "C" {
#endif
void pokemonBiosSetFriend(Pokemon* pokemon, unsigned short level);
void pokemonBiosCopy(Pokemon* dst, const Pokemon* src);

u16 pokemonBiosGetInitDp(Pokemon *pokemon);
u8 pokemonBiosGetFuseiFlag(Pokemon *pokemon);
u16 pokemonBiosGetFightTrainerPokemonDataId(Pokemon *pokemon);
u8 pokemonBiosGetPokerus(Pokemon *pokemon);
u8 pokemonBiosGetMailId(Pokemon *pokemon);
u8 pokemonBiosGetTokuseiFlag(Pokemon *pokemon);
#ifdef __cplusplus
void pokemonBiosSetTokuseiFlag(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetTokuseiFlag(Pokemon *pokemon, u8 flag);
#endif
GamedataAttest *pokemonBiosGetAttest(Pokemon *pokemon);
u32 pokemonBiosGetRnd(Pokemon *pokemon);
u32 pokemonBiosGetCatchTrainerRnd(Pokemon *pokemon);
u16 pokemonGetTokuseiDataId(Pokemon *pokemon);
PokemonTokuseiData *pokemonTokuseiDataBiosGetPtr(u16 dataID);
u32 pokemonTokuseiDataBiosGetName(PokemonTokuseiData *data);
u16 pokemonBiosGetHp(Pokemon *pokemon);
u16 pokemonBiosGetMaxHp(Pokemon *pokemon);
u16 pokemonBiosGetPhyAtk(Pokemon *pokemon);
u16 pokemonBiosGetPhyDef(Pokemon *pokemon);
u16 pokemonBiosGetSpeAtk(Pokemon *pokemon);
u16 pokemonBiosGetSpeDef(Pokemon *pokemon);
u16 pokemonBiosGetNimbleness(Pokemon *pokemon);
u16 pokemonBiosGetMaxHpEffort(Pokemon *pokemon);
u16 pokemonBiosGetPhyAtkEffort(Pokemon *pokemon);
u16 pokemonBiosGetPhyDefEffort(Pokemon *pokemon);
u16 pokemonBiosGetSpeAtkEffort(Pokemon *pokemon);
u16 pokemonBiosGetSpeDefEffort(Pokemon *pokemon);
u16 pokemonBiosGetNimblenessEffort(Pokemon *pokemon);void pokemonBiosSetPhyAtk(Pokemon *pokemon, u16 value);
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
u16 pokemonBiosGetDarkpokemonDataId(Pokemon *pokemon);
u16 pokemonBiosGetFriend(Pokemon *pokemon);
u8 pokemonBiosGetFlagAmari(Pokemon *pokemon);
u8 pokemonBiosGetPcboxMark(Pokemon *pokemon);
u16 pokemonBiosGetAmari(Pokemon *pokemon);
u16 pokemonBiosGetPokemonDataId(Pokemon *pokemon);
u16 pokemonBiosGetItemDataId(Pokemon *pokemon);
u32 pokemonBiosGetExp(Pokemon *pokemon);
u16 pokemonBiosGetPara1Amari(Pokemon *pokemon);
u8 pokemonBiosGetPokemonWazaPpCount(Pokemon *pokemon, u16 slot);
u16 pokemonBiosGetCatchFloorId(Pokemon *pokemon);
u8 pokemonBiosGetCatchLevel(Pokemon *pokemon);
u8 pokemonBiosGetCatchTrainerSex(Pokemon *pokemon);
u16 pokemonBiosGetMaxHpRnd(Pokemon *pokemon);
u16 pokemonBiosGetPhyAtkRnd(Pokemon *pokemon);
u16 pokemonBiosGetPhyDefRnd(Pokemon *pokemon);
u16 pokemonBiosGetNimblenessRnd(Pokemon *pokemon);
u16 pokemonBiosGetSpeAtkRnd(Pokemon *pokemon);
u16 pokemonBiosGetSpeDefRnd(Pokemon *pokemon);
u8 pokemonBiosGetStyle(Pokemon *pokemon);
u8 pokemonBiosGetBeautiful(Pokemon *pokemon);
u8 pokemonBiosGetCute(Pokemon *pokemon);
u8 pokemonBiosGetClever(Pokemon *pokemon);
u8 pokemonBiosGetStrong(Pokemon *pokemon);
u8 pokemonBiosGetFur(Pokemon *pokemon);
u8 pokemonBiosGetStyleMedal(Pokemon *pokemon);
u8 pokemonBiosGetBeautifulMedal(Pokemon *pokemon);
u8 pokemonBiosGetCuteMedal(Pokemon *pokemon);
u8 pokemonBiosGetCleverMedal(Pokemon *pokemon);
u8 pokemonBiosGetStrongMedal(Pokemon *pokemon);
u8 pokemonBiosGetAmariRibbon(Pokemon *pokemon);
u32 pokemonBiosGetConditionAmari(Pokemon *pokemon);
u16 *pokemonBiosGetNicknameOrgPtr(Pokemon *pokemon);
u16 *pokemonBiosGetCatchTrainerNamePtr(Pokemon *pokemon);

u8 pokemonBiosGetChampRibbon(Pokemon *pokemon);
u8 pokemonBiosGetWinningRibbon(Pokemon *pokemon);
u8 pokemonBiosGetVictoryRibbon(Pokemon *pokemon);
u8 pokemonBiosGetBromideRibbon(Pokemon *pokemon);
u8 pokemonBiosGetGanbaRibbon(Pokemon *pokemon);
u8 pokemonBiosGetMarineRibbon(Pokemon *pokemon);
u8 pokemonBiosGetLandRibbon(Pokemon *pokemon);
u8 pokemonBiosGetSkyRibbon(Pokemon *pokemon);
u8 pokemonBiosGetCountryRibbon(Pokemon *pokemon);
u8 pokemonBiosGetNationalRibbon(Pokemon *pokemon);
u8 pokemonBiosGetEarthRibbon(Pokemon *pokemon);
u8 pokemonBiosGetWorldRibbon(Pokemon *pokemon);

// Ribbon word and contest records of a Pokemon record; null for a Pokemon
// without one. The expand status index selects the contest values (0) or the
// contest medals (1).
PokemonRibbon *pokemonBiosGetRibbonPtr(Pokemon *pokemon);
PokemonExpandStatus *pokemonBiosGetExpandStatusPtr(Pokemon *pokemon, u16 index);

#ifdef __cplusplus
bool pokemonBiosGetEventGetFlag(Pokemon *pokemon);
#else
u8 pokemonBiosGetEventGetFlag(Pokemon *pokemon);
#endif

void pokemonBiosSetAmari(Pokemon *pokemon, u16 value);
void pokemonBiosSetPara1Amari(Pokemon *pokemon, u16 value);
void pokemonBiosSetPokemonDataId(Pokemon *pokemon, u16 value);
void pokemonBiosSetItemDataId(Pokemon *pokemon, u16 value);
void pokemonBiosSetMaxHp(Pokemon *pokemon, u16 value);
void pokemonBiosSetHp(Pokemon *pokemon, u16 value);
void pokemonBiosSetMaxHpEffort(Pokemon *pokemon, u16 value);
void pokemonBiosSetPhyAtkEffort(Pokemon *pokemon, u16 value);
void pokemonBiosSetPhyDefEffort(Pokemon *pokemon, u16 value);
void pokemonBiosSetNimblenessEffort(Pokemon *pokemon, u16 value);
void pokemonBiosSetSpeAtkEffort(Pokemon *pokemon, u16 value);
void pokemonBiosSetSpeDefEffort(Pokemon *pokemon, u16 value);

void pokemonBiosSetRnd(Pokemon *pokemon, u32 value);
void pokemonBiosSetDarkpokemonDataId(Pokemon *pokemon, u16 darkID);
void pokemonBiosSetCatchTrainerRnd(Pokemon *pokemon, u32 value);
void pokemonBiosSetExp(Pokemon *pokemon, u32 value);
void pokemonBiosSetConditionAmari(Pokemon *pokemon, u32 value);

void pokemonBiosSetPcboxMark(Pokemon *pokemon, u8 value);
void pokemonBiosSetFlagAmari(Pokemon *pokemon, u8 value);
void pokemonBiosSetPokerus(Pokemon *pokemon, u8 value);
void pokemonBiosSetCatchFloorId(Pokemon *pokemon, u8 value);
void pokemonBiosSetCatchLevel(Pokemon *pokemon, u8 value);
void pokemonBiosSetCatchBallId(Pokemon *pokemon, u8 value);
void pokemonBiosSetCatchTrainerSex(Pokemon *pokemon, u8 value);
void pokemonBiosSetMaxHpRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetPhyAtkRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetPhyDefRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetNimblenessRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetSpeAtkRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetSpeDefRnd(Pokemon *pokemon, u8 value);
void pokemonBiosSetStyle(Pokemon *pokemon, u8 value);
void pokemonBiosSetBeautiful(Pokemon *pokemon, u8 value);
void pokemonBiosSetCute(Pokemon *pokemon, u8 value);
void pokemonBiosSetClever(Pokemon *pokemon, u8 value);
void pokemonBiosSetStrong(Pokemon *pokemon, u8 value);
void pokemonBiosSetFur(Pokemon *pokemon, u8 value);
void pokemonBiosSetStyleMedal(Pokemon *pokemon, u8 value);
void pokemonBiosSetBeautifulMedal(Pokemon *pokemon, u8 value);
void pokemonBiosSetCuteMedal(Pokemon *pokemon, u8 value);
void pokemonBiosSetCleverMedal(Pokemon *pokemon, u8 value);
void pokemonBiosSetStrongMedal(Pokemon *pokemon, u8 value);
void pokemonBiosSetAmariRibbon(Pokemon *pokemon, u8 value);
void pokemonBiosSetLevel(Pokemon *pokemon, u8 value);
void pokemonBiosSetMailId(Pokemon *pokemon, u8 value);

void pokemonBiosSetPokemonWazaDataId(Pokemon *pokemon, u16 slot, u16 value);
void pokemonBiosSetPokemonWazaPpCount(Pokemon *pokemon, u16 slot, u8 value);
void pokemonBiosSetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 value);

#ifdef __cplusplus
void pokemonBiosSetFuseiFlag(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetFuseiFlag(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetTamagoFlag(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetTamagoFlag(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetChampRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetChampRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetWinningRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetWinningRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetVictoryRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetVictoryRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetBromideRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetBromideRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetGanbaRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetGanbaRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetMarineRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetMarineRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetLandRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetLandRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetSkyRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetSkyRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetCountryRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetCountryRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetNationalRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetNationalRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetEarthRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetEarthRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetWorldRibbon(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetWorldRibbon(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
void pokemonBiosSetEventGetFlag(Pokemon *pokemon, bool flag);
#else
void pokemonBiosSetEventGetFlag(Pokemon *pokemon, u8 flag);
#endif
#ifdef __cplusplus
}
#endif

#endif
