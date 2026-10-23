#include <WorldMap.h>

/* WorldMap::WorldMap - clear the reserved word and both work pointers.
 * The assignment order matches the original codegen. */
WorldMap::WorldMap()
{
    _00 = 0;
    mModelWork = 0;
    mDemoEffect = 0;
}
