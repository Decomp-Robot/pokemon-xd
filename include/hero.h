#ifndef HERO_H
#define HERO_H

#include <types.h>

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

#endif
