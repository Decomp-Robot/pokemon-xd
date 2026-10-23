#include <WorldMap.h>

/* WorldMap::GetAngle -- convert a selected work's map direction into the
 * angle the world-map icon faces. The selected work (SearchDataFloorID)
 * stores its direction as a rotation count; Init stores this result at
 * +0x38 of the model work as the selected-map icon angle. Groups with the
 * icon/animation helpers that share the WorldMap icon work
 * (GetStartIconAnimIndex, CheckIconAnime, CheckCursorAnime).
 *
 * Built with GC/2.0p1: this unit's .text matches the original only under
 * that version -- GC/2.6 hoists both sdata2 loads above the stack stores,
 * while the original (and the sibling conversions in _cameraPlayAnime at
 * 0x80011384 and GetPadIndex3 at 0x80011D2C, which share this TU) emits
 * the stores before the loads. The compiler question is settled by that
 * sibling evidence; what still blocks linking is the int-to-double bias
 * literal lbl_804EBF08, one shared sdata2 pool entry of the enclosing
 * WorldMap translation unit (the same entry the sibling conversions
 * load). A standalone object carries its own copy of the entry, and
 * pooling entries cannot be named from source, so linking this function
 * in isolation would shift every later sdata2 reference. Staged unlinked
 * (completed = false) until the enclosing module is decompiled. */
f32 WorldMap::GetAngle(long work)
{
    return work * (3.1415927f / 8);
}
