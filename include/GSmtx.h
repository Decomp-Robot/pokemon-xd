#ifndef GSMTX_H
#define GSMTX_H

#include <dolphin/mtx.h>

/* GS projection-matrix helpers. Both forward their arguments unchanged to the
 * matching C_MTX builder; they exist so GS code does not have to include the
 * SDK matrix header directly. */

extern "C" {
void GSmtxMakePerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f);
void GSmtxMakeOrtho(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 n, f32 f);
}

#endif
