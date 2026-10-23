#ifndef HERO_H
#define HERO_H

#include <types.h>

// The hero (player character): the items, money and pokemon the pocket menus
// act on, and the follower and style the map menu reads. Partial declaration:
// only the members the recovered menus call are named here. The bios block
// itself is moved around as a whole -- heroBiosCopy transfers 0x978 bytes per
// hero slot, and the trainer records keep two snapshots of it.
#ifdef __cplusplus
struct GSvec;

enum HERO_RESOURCE_ID {
    HERO_RESOURCE_MANA = 0x65,
    HERO_RESOURCE_FOLLOWER_SLOT_2 = 0x68,
    HERO_RESOURCE_FOLLOWER_SLOT_3 = 0x69
};

extern "C" void heroMoveGetLeaderPosition(GSvec* out);
/* Member code of the selected follower, or 0 when the hero walks alone. */
extern "C" u32 heroGetFollower(void);

class Hero {
public:
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
