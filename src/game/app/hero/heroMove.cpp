#include <hero.h>
#include <heroMove.h>

bool getResID(u32* group, u32* resource, HEROMOVE_MEMBER member) {
    u32 resources[] = {
        HERO_RESOURCE_HERO,
        HERO_RESOURCE_MANA,
        HERO_RESOURCE_GIRL,
        HERO_RESOURCE_KLEIN
    };
    if (member < 0 || member >= HERO_MOVE_MEMBER_COUNT) return false;
    *group = 0;
    *resource = resources[member];
    return true;
}

HeroMoveState HeroMove;
