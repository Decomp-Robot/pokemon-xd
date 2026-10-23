#ifndef TOOLENTRY_H
#define TOOLENTRY_H

#include <hero.h>
#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
Hero* toolentryTaisenGetHeroPtr(s32 port);
Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index);
Pokemon *toolentryTaisenGetEntryPokemonPtr(s32 port, s32 slot);
u16 toolentryTaisenGetPokemonNum(s32 port);
s32 toolentryTaisengetEtnryPokemonOrderNum(s32 port);
u16 toolentryTaisenGetHomePlace(s32 port);
u16 toolentryTaisenGetBattlePlayerID(s32 port);
void toolentryTaisenInitPokemonOrder(s32 port);
s32 toolentryTaisenSetEtnryPokemonOrder(s32 port, s32 index);
s32 toolentryTaisenDeleteEtnryPokemonOrder(s32 port);
void toolentryTaisenSetEtnryPokemonOrderGBA(s32 port, s32 num, u32 *pokemon);
u16 toolentryTaisenGetEntryPokemonNum(s32 port);
s32 toolentryTaisenGetEntryPlayerNum(void);
s32 toolentryTaisenGetBattleType(void);
s32 toolentryTaisenGetBattleVersus(void);
s32 toolentryTaisenGetControlerType(s32 port);
void toolentryTaisenEntryPokemon(s32 port);

#ifdef __cplusplus
}
#endif

#endif
