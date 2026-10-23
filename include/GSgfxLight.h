#ifndef GS_GFX_LIGHT_H
#define GS_GFX_LIGHT_H
#include <GSgfx.h>

struct GSgfxColAu8 {
    u8 r, g, b, a;
};

class GSgfxLight {
public:
    GSgfxLight();
    void MakeDiff(GSgfxColAu8 *color, GSvec *position, bool attenuation);
    void MakeSpec(GSgfxColAu8 *color, GSvec *direction, f32 shininess,
                  GSvec *position);
    u8 mLightState[0x80];
};
#endif
