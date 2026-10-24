#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include <types.h>
#include <GSgfx.h>
#include <GScamera.h>

#include <peBank.h>

extern "C" {

void particleEngineSetupTexture(u8 slot, GStexture *texture, u32 wrapS, u32 wrapT,
                                u32 minFilter, u32 magFilter);
void particleEngineSetupBlendMode(u32 mode);
void particleEngineSetupTevStages(u8 forceUpdate, bool extended, bool modulate);
bool particleEnginePrepareRender(_GScamera *camera);
bool particleEnginePrepareUpdate(_GScamera *camera);
void particleEngineSetDefaultRenderState(bool restore);
void particleEngineInit(void);
void particleEngineUpdate(f32 delta);
void particleEnginePostUpdate(void);
void particleEngineRender(GSgfxLayer *layer);

}  // extern "C"

struct peMemoryStats {
    u32 emitParameterBytes;
    u32 containerBytes;
    u32 nodeBytes;
    u32 renderDataBytes;
    u32 shapeBytes;
    u32 totalBytes;
    u32 reserved18[3];
};

extern peMemoryStats g_particle_debug_stats;
extern GSvec g_CameraPosition;
extern GSvec g_CameraLookDir;
extern GSvec g_CameraUpDir;
extern GSvec g_CameraSideDir;
extern GSgfxVF g_vertex_format;
extern GSgfxVF g_ext_vertex_format;

#endif
