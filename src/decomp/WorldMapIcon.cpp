#include <WorldMap.h>

/* WorldMap::GetAngle -- convert a selected work's map direction into the
 * angle the world-map icon faces (pi/8 per rotation step). Part of the
 * WorldMap icon work group (GetStartIconAnimIndex, CheckIconAnime,
 * CheckCursorAnime); compiled with GC/2.0p1 like that group.
 *
 * Staged unlinked (completed = false): the folded pi/8 constant lives in
 * the enclosing WorldMap translation unit's shared sdata2 pool, which this
 * function cannot own standalone. */
f32 WorldMap::GetAngle(long work)
{
    return work * (3.1415927f / 8);    /* pi/8, folded by the compiler */
}
