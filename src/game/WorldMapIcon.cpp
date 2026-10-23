#include <WorldMap.h>

/* WorldMap::GetStartIconAnimIndex -- map the icon start flag to the
 * animation index the icon is created with (see InitModel, which stores
 * this result into the icon work). Groups with the other icon-animation
 * helpers sharing the icon work (CheckIconAnime, CheckCursorAnime,
 * SetIconMotion), which will join this module as they are recovered. */
s32 WorldMap::GetStartIconAnimIndex(unsigned long index)
{
    switch (index) {
    case 0:
        return 0;
    case 1:
        return 6;
    case 2:
        return 1;
    default:
        return 0;
    }
}
