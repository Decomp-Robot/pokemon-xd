#include <heroBios.h>
#include <GSmsg.h>
#include <string.h>

namespace {

enum {
    HERO_BIOS_NAME_CAPACITY = 11,
    HERO_BIOS_CURRENCY_MAX = 9999999
};

} // namespace

void heroBiosSetMeetDarkPokemonCount(Hero *hero, u8 count)
{
    if (hero == 0) return;
    hero->mMeetDarkPokemonCount = count;
}

u8 heroBiosGetMeetDarkPokemonCount(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mMeetDarkPokemonCount;
}

void heroBiosSetHomePlace(Hero *hero, u8 homePlace)
{
    if (hero == 0) return;
    hero->mHomePlace = homePlace;
}

u8 heroBiosGetHomePlace(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mHomePlace;
}

u8 heroBiosGetHizukiFlag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mHizukiFlag;
}

void heroBiosSetHizukiFlag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mHizukiFlag = flag;
}

s32 heroBiosGetPokecouponAll(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mCouponTotal;
}

void heroBiosSetPokecouponAll(Hero *hero, s32 pokecouponAll)
{
    if (hero == 0) return;
    if (pokecouponAll < 0) pokecouponAll = 0;
    if (pokecouponAll > HERO_BIOS_CURRENCY_MAX) pokecouponAll = HERO_BIOS_CURRENCY_MAX;
    hero->mCouponTotal = pokecouponAll;
}

s32 heroBiosGetPokecoupon(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mCoupon;
}

void heroBiosSetPokecoupon(Hero *hero, s32 pokecoupon)
{
    if (hero == 0) return;
    if (pokecoupon < 0) pokecoupon = 0;
    if (pokecoupon > HERO_BIOS_CURRENCY_MAX) pokecoupon = HERO_BIOS_CURRENCY_MAX;
    hero->mCoupon = pokecoupon;
}

s32 heroBiosGetPokedoru(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mMoney;
}

void heroBiosSetPokedoru(Hero *hero, s32 pokedoru)
{
    if (hero == 0) return;
    if (pokedoru < 0) pokedoru = 0;
    if (pokedoru > HERO_BIOS_CURRENCY_MAX) pokedoru = HERO_BIOS_CURRENCY_MAX;
    hero->mMoney = pokedoru;
}

void heroBiosSetHizukiNamePtr(Hero *hero, u16 *name)
{
    if (hero == 0) return;
    if (name == 0) return;
    GScharLenCpy(hero->mHizukiName, name, HERO_BIOS_NAME_CAPACITY);
}

u16 *heroBiosGetHizukiNamePtr(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mHizukiName;
}

u8 heroBiosGetBadge08Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[7];
}

u8 heroBiosGetBadge07Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[6];
}

u8 heroBiosGetBadge06Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[5];
}

u8 heroBiosGetBadge05Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[4];
}

u8 heroBiosGetBadge04Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[3];
}

u8 heroBiosGetBadge03Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[2];
}

u8 heroBiosGetBadge02Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[1];
}

u8 heroBiosGetBadge01Flag(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mBadges[0];
}

void heroBiosSetBadge08Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[7] = flag;
}

void heroBiosSetBadge07Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[6] = flag;
}

void heroBiosSetBadge06Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[5] = flag;
}

void heroBiosSetBadge05Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[4] = flag;
}

void heroBiosSetBadge04Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[3] = flag;
}

void heroBiosSetBadge03Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[2] = flag;
}

void heroBiosSetBadge02Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[1] = flag;
}

void heroBiosSetBadge01Flag(Hero *hero, u8 flag)
{
    if (hero == 0) return;
    hero->mBadges[0] = flag;
}

u8 heroBiosGetSexDataId(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mSex;
}

void heroBiosSetSexDataId(Hero *hero, u8 sexDataId)
{
    if (hero == 0) return;
    hero->mSex = sexDataId;
}

void heroBiosSetRnd(Hero *hero, u32 rnd)
{
    if (hero == 0) return;
    hero->mRandomIdentity = rnd;
}

void heroBiosSetNamePtr(Hero *hero, u16 *name)
{
    if (hero == 0 || name == 0) return;
    GScharLenCpy(hero->mName, name, HERO_BIOS_NAME_CAPACITY);
}

ItemSlot *heroBiosGetHizukiItemPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mHizukiItems) / sizeof(hero->mHizukiItems[0])) return 0;
    return &hero->mHizukiItems[index];
}

ItemSlot *heroBiosGetItemKoronPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mKoronItems) / sizeof(hero->mKoronItems[0])) return 0;
    return &hero->mKoronItems[index];
}

ItemSlot *heroBiosGetItemSeedPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mSeedItems) / sizeof(hero->mSeedItems[0])) return 0;
    return &hero->mSeedItems[index];
}

ItemSlot *heroBiosGetItemSkillPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mSkillItems) / sizeof(hero->mSkillItems[0])) return 0;
    return &hero->mSkillItems[index];
}

ItemSlot *heroBiosGetItemBallPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mBallItems) / sizeof(hero->mBallItems[0])) return 0;
    return &hero->mBallItems[index];
}

ItemSlot *heroBiosGetExtraItemPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mExtraItems) / sizeof(hero->mExtraItems[0])) return 0;
    return &hero->mExtraItems[index];
}

ItemSlot *heroBiosGetItemNormalPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mNormalItems) / sizeof(hero->mNormalItems[0])) return 0;
    return &hero->mNormalItems[index];
}

Pokemon *heroBiosGetPokemonPtr(Hero *hero, u16 index)
{
    if (hero == 0) return 0;
    if (index >= sizeof(hero->mPokemon) / sizeof(hero->mPokemon[0])) return 0;
    return &hero->mPokemon[index];
}

u32 heroBiosGetRnd(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mRandomIdentity;
}

u16 *heroBiosGetNamePtr(Hero *hero)
{
    if (hero == 0) return 0;
    return hero->mName;
}

void heroBiosCopy(Hero *destination, Hero *source)
{
    if (destination == 0) return;
    if (source == 0) return;
    memcpy(destination, source, sizeof(Hero));
}

