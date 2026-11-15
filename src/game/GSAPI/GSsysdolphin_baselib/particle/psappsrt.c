#include <GSAPI/GSsysdolphin_baselib/particle/psappsrt.h>
#include <hsd/memory.h>
#include <string.h>

static _psAppSRT *freeAppSRT;
static u16 psSizeofAppSRT;

static inline void releaseAppSRT(_psAppSRT *appSRT)
{
    if (appSRT->cleanup) {
        appSRT->cleanup(appSRT);
    }
    appSRT->next = freeAppSRT;
    freeAppSRT = appSRT;
    numActivePSAppSRT--;
}

int psRemoveGeneratorAppSRT(_psGenerator *generator)
{
    int remaining;

    if (generator->mAppSRT == NULL) {
        return -1;
    }
    if (generator->mAppSRT->generator == generator) {
        generator->mAppSRT->generator = NULL;
    }
    remaining = --generator->mAppSRT->referenceCount;
    if (remaining == 0) {
        releaseAppSRT(generator->mAppSRT);
    }
    generator->mAppSRT = NULL;
    return remaining;
}

int psRemoveParticleAppSRT(_psParticle *particle)
{
    int remaining;

    if (particle->appSRT == NULL) {
        return -1;
    }
    remaining = --particle->appSRT->referenceCount;
    if (remaining == 0) {
        releaseAppSRT(particle->appSRT);
    }
    particle->appSRT = NULL;
    return remaining;
}

int psChangeGeneratorAppSRT(_psGenerator *generator, _psAppSRT *appSRT)
{
    if (appSRT == NULL || generator == NULL) {
        return -1;
    }
    if (generator->mAppSRT) {
        psRemoveGeneratorAppSRT(generator);
    }
    generator->mAppSRT = appSRT;
    return ++appSRT->referenceCount;
}

int psChangeParticleAppSRT(_psParticle *particle, _psAppSRT *appSRT)
{
    if (appSRT == NULL || particle == NULL) {
        return -1;
    }
    if (particle->appSRT) {
        psRemoveParticleAppSRT(particle);
    }
    particle->appSRT = appSRT;
    return ++appSRT->referenceCount;
}

int psAttachGeneratorAppSRT(_psGenerator *generator, _psAppSRT *appSRT)
{
    if (appSRT == NULL || generator == NULL || generator->mAppSRT) {
        return -1;
    }
    generator->mAppSRT = appSRT;
    return ++appSRT->referenceCount;
}

int psAttachParticleAppSRT(_psParticle *particle, _psAppSRT *appSRT)
{
    if (appSRT == NULL || particle == NULL || particle->appSRT) {
        return -1;
    }
    particle->appSRT = appSRT;
    return ++appSRT->referenceCount;
}

_psAppSRT *psAddGeneratorAppSRT(_psGenerator *generator, u8 matrixMode)
{
    u16 familyID = generator->mFamilyID;
    _psAppSRT *appSRT;

    if (freeAppSRT == NULL) {
        freeAppSRT = HSD_Alloc(psSizeofAppSRT);
        if (freeAppSRT) {
            memset(freeAppSRT, 0, psSizeofAppSRT);
        }
    }
    appSRT = freeAppSRT;
    if (appSRT) {
        freeAppSRT = appSRT->next;
        appSRT->next = NULL;
        appSRT->referenceCount = 1;
        appSRT->cacheToken = 0;
        appSRT->matrixMode = matrixMode;
        appSRT->translation.x = appSRT->translation.y = appSRT->translation.z = 0.0F;
        appSRT->rotation.x = appSRT->rotation.y = appSRT->rotation.z = 0.0F;
        appSRT->scale.x = appSRT->scale.y = appSRT->scale.z = 1.0F;
        appSRT->cleanup = NULL;
        appSRT->cameraRelative = 0;
        appSRT->generator = NULL;
        appSRT->familyID = familyID;
        numActivePSAppSRT++;
        if (numActivePSAppSRT > numPeakPSAppSRT) {
            numPeakPSAppSRT = numActivePSAppSRT;
        }
    }
    generator->mAppSRT = appSRT;
    return appSRT;
}

void psRemoveAppSRT(void)
{
    _psAppSRT *appSRT = freeAppSRT;
    _psAppSRT *next;
    while (appSRT) {
        next = appSRT->next;
        HSD_Free(appSRT);
        appSRT = next;
    }
    freeAppSRT = NULL;
}

int psInitAppSRT(int count, u32 size)
{
    int i;
    _psAppSRT *appSRT;

    numActivePSAppSRT = 0;
    numPeakPSAppSRT = 0;
    psSizeofAppSRT = size;
    freeAppSRT = NULL;
    for (i = 0; i < count; i++) {
        appSRT = HSD_Alloc(size);
        if (appSRT == NULL) {
            return i;
        }
        memset(appSRT, 0, size);
        appSRT->next = freeAppSRT;
        freeAppSRT = appSRT;
    }
    return i;
}
