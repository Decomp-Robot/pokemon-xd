#ifndef HERO_H
#define HERO_H

#include <types.h>

enum { HERO_STATUS_TRAINER_ID = 2,
HERO_STATUS_ITEM_NORMAL = 4,
HERO_STATUS_ITEM_EXTRA = 5,
HERO_STATUS_ITEM_BALL = 6,
HERO_STATUS_ITEM_SKILL = 7,
HERO_STATUS_ITEM_SEED = 8,
HERO_STATUS_ITEM_KORON = 9,
HERO_STATUS_HIZUKI_ITEM = 10,
HERO_STATUS_SEX = 11,
HERO_STATUS_POKEDORU = 12,
HERO_STATUS_POKECOUPON = 13,
HERO_STATUS_POKECOUPON_ALL = 14,
HERO_STATUS_BADGE_01 = 15,
HERO_STATUS_BADGE_02 = 16,
HERO_STATUS_BADGE_03 = 17,
HERO_STATUS_BADGE_04 = 18,
HERO_STATUS_BADGE_05 = 19,
HERO_STATUS_BADGE_06 = 20,
HERO_STATUS_BADGE_07 = 21,
HERO_STATUS_BADGE_08 = 22,
HERO_STATUS_HIZUKI_FLAG = 24,
HERO_STATUS_HOME_PLACE = 25 };

enum { HERO_PTR_FLAG = 0x30D };

enum { HERO_STYLE_NORMAL = 0,
HERO_STYLE_VISOR = 1,
HERO_STYLE_SNATCH = 2 };
#include <Pokemon.h>

// Player inventory, money and party.
#ifdef __cplusplus
struct GSvec;

enum HERO_RESOURCE_ID {
    HERO_RESOURCE_MANA = 0x65,
    HERO_RESOURCE_FOLLOWER_SLOT_2 = 0x68,
    HERO_RESOURCE_FOLLOWER_SLOT_3 = 0x69
};

extern "C" void heroMoveGetHeroPos(GSvec* out);
/* Member code of the selected follower, or 0 when the hero walks alone. */
extern "C" u32 heroGetFollower(void);

class Hero {
public:
    Hero() {}
    static Hero* getHeroPtr();
    u8 getFollowerID();
    u8 getHeroStyle() const;
    Pokemon* getPokemon(long slot);
    const Pokemon* getPokemon(long slot) const;
    void setMeetDarkPokemonCount(u8 count);
    u8 getMeetDarkPokemonCount() const;
    void itemSort(u8 kind); // sorts the item list of one pocket.

private:
    u8 _00[0x30];
    Pokemon mPokemon[6];
    u8 _4C8[0x978 - 0x4C8];
};
#else
typedef struct Hero {
    u8 _00[0x30];
    Pokemon mPokemon[6];
    u8 _4C8[0x978 - 0x4C8];
} Hero;
#endif

#ifdef __cplusplus
extern "C" {
#endif
enum HeroStatusID {
    HERO_STATUS_NAME = 1,
    HERO_STATUS_TRAINER_RND = 2,
    HERO_STATUS_POKEMON = 3,
    HERO_STATUS_MONEY = 12,
    HERO_STATUS_HIZUKI_NAME = 23
};

unsigned long heroGetPokeomCount(Hero* hero);
u32 heroGetStatus(Hero* hero, u16 statusID, u16 slot);
void heroSetStatus(Hero* hero, u16 statusID, u32 value);
u16* heroBiosGetNamePtr(Hero* hero);
Pokemon* heroBiosGetPokemonPtr(Hero* hero, u16 slot);
s32 heroItemCheckAddItemDataId(Hero* hero, u16 itemID);
s32 heroItemAddItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
s32 heroItemDecItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
#ifdef __cplusplus
}
#endif

#endif
