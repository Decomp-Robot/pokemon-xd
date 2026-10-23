#if DEBUG
#include <dolphin/gx.h>

#include "__gx.h"

#define FUNC_1PARAM(name, T) \
void name##1##T(T x) { GXWGFifo.T = x; }

// GXParam
FUNC_1PARAM(GXParam, f32)

#endif  // DEBUG
