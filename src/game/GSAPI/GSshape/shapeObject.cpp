#include <GSshapeInternal.h>

void GsShapeObject::MakeShapeGfxDL()
{
    u16 strip, first, next, column;
    u16 texCoordCount = mEntry->mTextureCount;
    u16 columns = mEntry->mPolygonCount + 1;
    GSgfxDLPrimitiveType(GS_GFX_PRIM_TRIANGLE_STRIP);

    for (strip = 0; strip < mEntry->mSubPolygonCount; strip++) {
        first = strip * columns;
        next = (strip + 1) * columns;
        GSgfxDLBeginSurface(columns * 2);
        for (column = 0; column < columns; column++) {
            u16 index0 = column + first;
            u16 index1 = column + next;
            u16 coord;
            GSgfxFifo.u16 = index0;
            GSgfxFifo.u16 = index0;
            for (coord = 0; coord < texCoordCount; coord++) {
                GSgfxFifo.u16 = index0;
            }
            GSgfxFifo.u16 = index1;
            GSgfxFifo.u16 = index1;
            for (coord = 0; coord < texCoordCount; coord++) {
                GSgfxFifo.u16 = index1;
            }
        }
        GSgfxDLEndSurface();
    }
}
