#include <WorldMap.h>

/* WorldMap::WorldMap - clear the reserved word and both work pointers.
 * The assignment order matches the original codegen. */
WorldMap::WorldMap()
{
    mOffscreenTexture = 0;
    mModelWork = 0;
    mDemoEffect = 0;
}
