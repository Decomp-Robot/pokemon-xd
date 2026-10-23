#include <GSmtx.h>

extern "C" void C_MTXPerspective(f32 matrix[4][4], f32 fovY, f32 aspect,
                                  f32 nearPlane, f32 farPlane);

extern "C" void GSmtx44Perspective(f32 matrix[4][4], f32 fovY, f32 aspect,
                                    f32 nearPlane, f32 farPlane)
{
    C_MTXPerspective(matrix, fovY, aspect, nearPlane, farPlane);
}
