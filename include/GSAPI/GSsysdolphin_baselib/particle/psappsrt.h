#ifndef GS_PARTICLE_PSAPPSRT_H
#define GS_PARTICLE_PSAPPSRT_H

#include <GSAPI/GSsysdolphin_baselib/particle/particle.h>
#include <dolphin/mtx.h>

typedef struct _psAppSRT _psAppSRT;
struct _psAppSRT {
    _psAppSRT *next;
    _psGenerator *generator;
    Vec translation;
    Vec rotation;
    u8 _20[4];
    Vec scale;
    u8 matrixMode;
    u8 cacheToken;
    u16 referenceCount;
    Mtx matrix;
    u8 _64[8];
    void (*cleanup)(_psAppSRT *appSRT);
    u16 familyID;
    u8 cameraRelative;
};

enum PSAppSRTMatrixMode {
    PS_APP_SRT_MATRIX_DYNAMIC = 0,
    PS_APP_SRT_MATRIX_ONCE = 1,
    PS_APP_SRT_MATRIX_FIXED = 2
};

#ifdef __cplusplus
extern "C" {
#endif

int psRemoveGeneratorAppSRT(_psGenerator *generator);
int psRemoveParticleAppSRT(_psParticle *particle);
int psChangeGeneratorAppSRT(_psGenerator *generator, _psAppSRT *appSRT);
int psChangeParticleAppSRT(_psParticle *particle, _psAppSRT *appSRT);
int psAttachGeneratorAppSRT(_psGenerator *generator, _psAppSRT *appSRT);
int psAttachParticleAppSRT(_psParticle *particle, _psAppSRT *appSRT);
_psAppSRT *psAddGeneratorAppSRT(_psGenerator *generator, u8 matrixMode);
void psRemoveAppSRT(void);
int psInitAppSRT(int count, u32 size);

#ifdef __cplusplus
}
#endif

#endif
