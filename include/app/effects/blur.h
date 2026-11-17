#ifndef APP_EFFECTS_BLUR_H
#define APP_EFFECTS_BLUR_H

#include <GSeffect.h>

struct _GSmodel;

// Motion blur / afterimage effect. With a model bound it redraws a short
// history of that model's animation, fading the older frames out. In
// framebuffer mode it captures the back framebuffer into a texture and
// composites it back over the frame once a capture has landed.
class blurEffect : public GSeffect {
public:
    blurEffect(bool framebuffer);

    virtual bool _start();
    virtual void _stop();
    virtual void _render(GSgfxLayer *layer, GSgfxLayerPass pass);
    virtual ~blurEffect();

    void RenderFB(GSgfxLayer *layer);

    union {
        GStexture *mCapture;
        _GSmodel *mModel;
    };
    u16 mToken;
    bool mCaptureReady;
    u8 mReserved;
    bool mFramebuffer;
    f32 mStrength;
};

#endif
