#ifndef HERO_H
#define HERO_H

#include <types.h>

// The hero (player character): the items, money and pokemon the pocket menus
// act on, and the follower and style the map menu reads. Partial declaration:
// only the members the recovered menus call are named here. The bios block
// itself is moved around as a whole -- heroBiosCopy transfers 0x978 bytes per
// hero slot, and the trainer records keep two snapshots of it.
class Hero {
public:
    u8 getFollowerID();
    u8 getHeroStyle() const;
    void itemSort(u8 kind); // sorts the item list of one pocket.

private:
    u8 mBios[0x978];
};

#endif
