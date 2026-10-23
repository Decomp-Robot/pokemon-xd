#ifndef HERO_H
#define HERO_H

#include <types.h>

// The hero (player character): the items, money and pokemon the pocket menus
// act on, and the follower and style the map menu reads. Partial declaration:
// only the members the recovered menus call are named here.
class Hero {
public:
    u8 getFollowerID();
    u8 getHeroStyle() const;
    void itemSort(u8 kind); // sorts the item list of one pocket.
};

#endif
