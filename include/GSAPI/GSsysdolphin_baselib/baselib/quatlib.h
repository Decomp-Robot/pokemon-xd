#ifndef INCLUDE_GSAPI_GSSYSDOLPHIN_BASELIB_BASELIB_QUATLIB_H
#define INCLUDE_GSAPI_GSSYSDOLPHIN_BASELIB_BASELIB_QUATLIB_H

#include <GSgfx.h>

extern "C" {
int EulerToQuat(const GSvec* rotation, f32* quaternion);
}

#endif
