#ifndef VEC_GRAPH_H
#define VEC_GRAPH_H

#include <types.h>
#include <flashcalc.h>
#include <flashmenu.h>
#include <GSgfx.h>

extern "C" {
void vectorGraph(u32 characterId, _FILLSTYLEARRAY* fills, _LINESTYLEARRAY* lines,
                 _SHAPERECORD* records, FLASHMATRIXF* objectMatrix,
                 FLASHMATRIXF* parentMatrix, FLASHCXFORM* colorTransform, u8 flags);
void vectroGraphSetBlendMode(GSgfxBlendFactor source, GSgfxBlendFactor destination);
}

#endif
