#ifndef HERO_H
#define HERO_H

#include <types.h>

enum HERO_STYLE {
    HERO_STYLE_NORMAL = 0,
    HERO_STYLE_VISOR = 1,
    HERO_STYLE_SNATCH = 2
};

// Player items, money, Pokemon, follower and appearance.
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

    u8 getFollowerID();
    u8 getHeroStyle() const;
    void itemSort(u8 kind); // sorts the item list of one pocket.

private:
    u8 mBios[0x978];
};
#else
// The C menus only move hero records around, so they see the same layout
// without the member functions.
typedef struct Hero {
    u8 mBios[0x978];
} Hero;
#endif

// Selects the connect-battle hero instead of the active save hero.
enum { HERO_PTR_FLAG = 0x30D };
// Field-selecting query codes of heroGetStatus; each dispatches to one bios
// getter of the hero/trainer record. Query 0 and anything past these return 0.
enum HERO_STATUS_QUERY {
    HERO_STATUS_NAME = 1,             // heroBiosGetNamePtr
    HERO_STATUS_TRAINER_ID = 2,       // trainerGetTID: the word at trainer+0x2C.
    HERO_STATUS_POKEMON = 3,          // heroBiosGetPokemonPtr (a Pokemon address).
    HERO_STATUS_ITEM_NORMAL = 4,      // heroBiosGetItemNormalPtr
    HERO_STATUS_ITEM_EXTRA = 5,       // heroBiosGetExtraItemPtr
    HERO_STATUS_ITEM_BALL = 6,        // heroBiosGetItemBallPtr
    HERO_STATUS_ITEM_SKILL = 7,       // heroBiosGetItemSkillPtr
    HERO_STATUS_ITEM_SEED = 8,        // heroBiosGetItemSeedPtr
    HERO_STATUS_ITEM_KORON = 9,       // heroBiosGetItemKoronPtr
    HERO_STATUS_HIZUKI_ITEM = 10,     // heroBiosGetHizukiItemPtr
    HERO_STATUS_SEX = 11,             // heroBiosGetSexDataId
    HERO_STATUS_POKEDORU = 12,        // heroBiosGetPokedoru
    HERO_STATUS_POKECOUPON = 13,      // heroBiosGetPokecoupon
    HERO_STATUS_POKECOUPON_ALL = 14,  // heroBiosGetPokecouponAll
    HERO_STATUS_BADGE_01 = 15,        // heroBiosGetBadge01Flag
    HERO_STATUS_BADGE_02 = 16,        // heroBiosGetBadge02Flag
    HERO_STATUS_BADGE_03 = 17,        // heroBiosGetBadge03Flag
    HERO_STATUS_BADGE_04 = 18,        // heroBiosGetBadge04Flag
    HERO_STATUS_BADGE_05 = 19,        // heroBiosGetBadge05Flag
    HERO_STATUS_BADGE_06 = 20,        // heroBiosGetBadge06Flag
    HERO_STATUS_BADGE_07 = 21,        // heroBiosGetBadge07Flag
    HERO_STATUS_BADGE_08 = 22,        // heroBiosGetBadge08Flag
    HERO_STATUS_HIZUKI_NAME = 23,     // heroBiosGetHizukiNamePtr
    HERO_STATUS_HIZUKI_FLAG = 24,     // heroBiosGetHizukiFlag
    HERO_STATUS_HOME_PLACE = 25,      // heroBiosGetHomePlace
};

#ifdef __cplusplus
extern "C" {
#endif

// One field of the hero/trainer bios record. `hero` may be null, which the
// callee resolves to the active hero; `query` selects the field and `slot`
// indexes the record the pointer queries hand back. The result is the scalar
// the getter returns -- a count, id or Pokemon address, so cast accordingly.
unsigned long heroGetStatus(Hero *hero, u16 query, int slot);

#ifdef __cplusplus
}
#endif

#endif
