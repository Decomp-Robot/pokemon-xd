#ifndef HERO_BIOS_H
#define HERO_BIOS_H

#include <heroStatus.h>
#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif

void heroBiosSetMeetDarkPokemonCount(Hero *hero, u8 count);
u8 heroBiosGetMeetDarkPokemonCount(Hero *hero);
void heroBiosSetHomePlace(Hero *hero, u8 homePlace);
u8 heroBiosGetHomePlace(Hero *hero);
u8 heroBiosGetHizukiFlag(Hero *hero);
void heroBiosSetHizukiFlag(Hero *hero, u8 flag);
s32 heroBiosGetPokecouponAll(Hero *hero);
void heroBiosSetPokecouponAll(Hero *hero, s32 pokecouponAll);
s32 heroBiosGetPokecoupon(Hero *hero);
void heroBiosSetPokecoupon(Hero *hero, s32 pokecoupon);
s32 heroBiosGetPokedoru(Hero *hero);
void heroBiosSetPokedoru(Hero *hero, s32 pokedoru);
void heroBiosSetHizukiNamePtr(Hero *hero, u16 *name);
u16 *heroBiosGetHizukiNamePtr(Hero *hero);
u8 heroBiosGetBadge08Flag(Hero *hero);
u8 heroBiosGetBadge07Flag(Hero *hero);
u8 heroBiosGetBadge06Flag(Hero *hero);
u8 heroBiosGetBadge05Flag(Hero *hero);
u8 heroBiosGetBadge04Flag(Hero *hero);
u8 heroBiosGetBadge03Flag(Hero *hero);
u8 heroBiosGetBadge02Flag(Hero *hero);
u8 heroBiosGetBadge01Flag(Hero *hero);
void heroBiosSetBadge08Flag(Hero *hero, u8 flag);
void heroBiosSetBadge07Flag(Hero *hero, u8 flag);
void heroBiosSetBadge06Flag(Hero *hero, u8 flag);
void heroBiosSetBadge05Flag(Hero *hero, u8 flag);
void heroBiosSetBadge04Flag(Hero *hero, u8 flag);
void heroBiosSetBadge03Flag(Hero *hero, u8 flag);
void heroBiosSetBadge02Flag(Hero *hero, u8 flag);
void heroBiosSetBadge01Flag(Hero *hero, u8 flag);
u8 heroBiosGetSexDataId(Hero *hero);
void heroBiosSetSexDataId(Hero *hero, u8 sexDataId);
void heroBiosSetRnd(Hero *hero, u32 rnd);
void heroBiosSetNamePtr(Hero *hero, u16 *name);
ItemSlot *heroBiosGetHizukiItemPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetItemKoronPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetItemSeedPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetItemSkillPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetItemBallPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetExtraItemPtr(Hero *hero, u16 index);
ItemSlot *heroBiosGetItemNormalPtr(Hero *hero, u16 index);
Pokemon *heroBiosGetPokemonPtr(Hero *hero, u16 index);
u32 heroBiosGetRnd(Hero *hero);
u16 *heroBiosGetNamePtr(Hero *hero);
void heroBiosCopy(Hero *destination, Hero *source);

#ifdef __cplusplus
}
#endif

#endif
