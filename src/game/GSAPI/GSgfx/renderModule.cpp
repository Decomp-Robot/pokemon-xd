#include <GSgfx.h>

extern "C" f32 __float_epsilon[];

enum { GSGFX_RENDER_MODULE_MAX = 9 };

struct GSgfxRenderModuleEntry {
    GSgfxRenderModuleEntry *mNext;
    u8 mRegistered;
    u8 mPriority;
    u8 padding[2];
    GSgfxRenderModule mModule;
};

GSgfxRenderModuleEntry _renderModuleList[GSGFX_RENDER_MODULE_MAX];
GSgfxRenderModuleEntry *_renderModuleBottom;

static bool _renderModuleRendering;

bool GSgfxRenderModuleIsRendering(void)
{
    return _renderModuleRendering;
}

void GSgfxRenderModuleDoPostUpdate(f32 delta)
{
    GSgfxRenderModuleEntry *entry;
    u32 frames;
    u32 i;

    frames = (u32)(delta + __float_epsilon[0]);
    for (i = 0; i < frames; i++) {
        for (entry = _renderModuleBottom; entry != 0; entry = entry->mNext) {
            if (entry->mModule.postFrame != 0) entry->mModule.postFrame();
        }
    }
    for (entry = _renderModuleBottom; entry != 0; entry = entry->mNext) {
        if (entry->mModule.postUpdate != 0) entry->mModule.postUpdate(delta);
    }
}

void GSgfxRenderModuleDoRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    GSgfxRenderModuleEntry *entry;

    _renderModuleRendering = true;
    for (entry = _renderModuleBottom; entry != 0; entry = entry->mNext) {
        if (entry->mModule.render != 0) entry->mModule.render(layer, pass);
    }
    _renderModuleRendering = false;
}

void GSgfxRenderModuleDoRenderFast(void)
{
    GSgfxRenderModuleEntry *entry;

    _renderModuleRendering = true;
    for (entry = _renderModuleBottom; entry != 0; entry = entry->mNext) {
        if (entry->mModule.fastRender != 0) entry->mModule.fastRender();
    }
    _renderModuleRendering = false;
}

void GSgfxRenderModuleDoPreUpdate(f32 delta)
{
    GSgfxRenderModuleEntry *entry;

    for (entry = _renderModuleBottom; entry != 0; entry = entry->mNext) {
        if (entry->mModule.preUpdate != 0) entry->mModule.preUpdate(delta);
    }
}

static inline void SetRenderModulePriority(GSgfxRenderModuleEntry *entry, u8 priority)
{
    GSgfxRenderModuleEntry *link;
    GSgfxRenderModuleEntry *next;
    if (entry->mPriority != priority) {
        if ((link = _renderModuleBottom) == entry) {
            _renderModuleBottom = entry->mNext;
        } else {
            for (; link != 0; link = link->mNext) {
                if (link->mNext == entry) break;
            }
            link->mNext = entry->mNext;
        }
        entry->mNext = 0;
        entry->mPriority = priority;
        link = _renderModuleBottom;
        if (link == 0 || link->mPriority > priority) {
            entry->mNext = link;
            _renderModuleBottom = entry;
        } else {
            next = link->mNext;
            while (next != 0) {
                if (next->mPriority > priority) break;
                link = next;
                next = next->mNext;
            }
            entry->mNext = next;
            link->mNext = entry;
        }
    }
}

void GSgfxRenderModuleRegister(u8 priority, GSgfxRenderModule *module, void *argument)
{
    GSgfxRenderModuleEntry *entry;
    u32 i;

    for (i = 0; i < GSGFX_RENDER_MODULE_MAX; i++) {
        entry = &_renderModuleList[i];
        if (entry->mRegistered != 0) continue;
        entry->mRegistered = 1;
        entry->mPriority = 0;
        entry->mNext = _renderModuleBottom;
        _renderModuleBottom = entry;
        memcpy(&entry->mModule, module, sizeof(GSgfxRenderModule));

        /* The entry starts out at the head of the list; a module that does not
         * run before all of them is taken out again and put behind the last
         * module of an equal or lower priority. */
        SetRenderModulePriority(entry, priority);
        if (module->init != 0) module->init(argument);
        break;
    }
}

void GSgfxRenderModuleInit(void)
{
    memset(_renderModuleList, 0, sizeof(_renderModuleList));
    _renderModuleBottom = 0;
    _renderModuleRendering = false;
}
