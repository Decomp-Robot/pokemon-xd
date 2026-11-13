#include <GSeffect.h>

/* Priority the effect module takes in the renderer's per-frame module list. */
enum { GS_EFFECT_RENDER_MODULE_ID = 96 };

void _effectUpdate(f32 delta);
void _effectRender(GSgfxLayer* layer, GSgfxLayerPass pass);
void _effectFastRender(void);

/* Head of the list of live effects; the constructor prepends and the
   destructor unlinks. */
GSeffect* g_effects_list;

extern "C" void GSeffectInitSystem(void)
{
    GSgfxRenderModule module = {0, _effectUpdate, _effectRender, _effectFastRender, 0, 0,
                                GSGFX_RENDER_MODULE_SIGNATURE};

    GSgfxRenderModuleRegister(GS_EFFECT_RENDER_MODULE_ID, &module, 0);
}

void _effectFastRender(void)
{
    GSeffect* effect;

    for (effect = g_effects_list; effect != 0; effect = effect->mNext) {
        if (effect->mActive && effect->mVisible) {
            effect->_fastRender();
        }
    }
}

void _effectRender(GSgfxLayer* layer, GSgfxLayerPass pass)
{
    GSeffect* effect;

    for (effect = g_effects_list; effect != 0; effect = effect->mNext) {
        if (effect->mActive && effect->mVisible) {
            effect->_render(layer, pass);
        }
    }
}

void _effectUpdate(f32 delta)
{
    GSeffect* effect;

    for (effect = g_effects_list; effect != 0; effect = effect->mNext) {
        f32 frame = delta / GSgfxVideoGetVsyncRate();

        if (effect->mActive) {
            if (!effect->_update(frame) && effect->mActive) {
                effect->_stop();
                effect->mActive = false;
                effect->mVisible = false;
            }
        }
    }
}

GSeffect::~GSeffect()
{
    GSeffect* prev = g_effects_list;

    if (this == prev) {
        g_effects_list = mNext;
        mNext = 0;
    } else {
        while (prev != 0) {
            if (prev->mNext == this) {
                prev->mNext = mNext;
                mNext = 0;
                break;
            }
            prev = prev->mNext;
        }
    }
}

void GSeffect::Stop()
{
    if (mActive) {
        _stop();
        mActive = false;
        mVisible = false;
    }
}

bool GSeffect::Start()
{
    if (_start()) {
        mActive = true;
        mVisible = true;
    } else {
        mActive = false;
        mVisible = false;
    }
    return mActive;
}

void GSeffect::_fastRender()
{
}

void GSeffect::_render(GSgfxLayer* layer, GSgfxLayerPass pass)
{
}

bool GSeffect::_update(f32 delta)
{
    return true;
}

void GSeffect::_stop()
{
}

bool GSeffect::_start()
{
    return true;
}

GSeffect::GSeffect() : mActive(false), mVisible(false), mNext(0)
{
    mNext = g_effects_list;
    g_effects_list = this;
}
