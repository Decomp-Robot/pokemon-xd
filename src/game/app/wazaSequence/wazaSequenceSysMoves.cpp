#include <NullSequence.h>
#include <wazaSequence.h>
#include <GSsnd.h>
#include <GSres.h>
#include <GSparticle.h>
#include <GSAPI/GSfsys/GSfsys.h>
#include <string.h>

// Live-move bookkeeping and cleanup for the move-animation system.

// Resource group the camera-shake models are read into and released from.
#define CAMERA_SHAKE_RESOURCE_GROUP 1588

// Resource group holding the ice and sleep status particles.
#define STATUS_PARTICLE_RESOURCE_GROUP 1783

extern NullSequence *g_Sequences;
extern WazaSequence *g_LastStartedWaza;

GSmodel *gCameraShakeModels[9] = { NULL };
void *_modelShadowList[16];
_GSparticleBank *g_IceParticle;
_GSparticleBank *g_SleepParticle;
u32 _resource_id_count;
u32 _modelShadowCount;
GSlight *_modelShadowLight;
GSsndListener *g_SoundListener;
GSmodel *gCameraShakeModel;
bool wazaLayerChangeEnabled;

// A stage is owned by the move system only while it is on the live list.
static inline bool isStageOnList(NullSequence *sequence)
{
    NullSequence *seq = g_Sequences;
    if (sequence == NULL) {
        return false;
    }
    while (seq != NULL) {
        if (seq == sequence) {
            return true;
        }
        seq = seq->next;
    }
    return false;
}

extern "C" {

void wazaSequenceSysSetVisibilityAll(bool visible)
{
    NullSequence *stage = g_Sequences;
    while (stage != NULL) {
        stage->SetVisibility(visible);
        stage = stage->next;
    }
}

u16 wazaSequenceSysGetActiveWazaNum(void)
{
    u16 count = 0;
    NullSequence *stage = g_Sequences;
    while (stage != NULL) {
        WazaSequence *waza = stage->waza;
        while (waza != NULL) {
            bool active = waza->playing && !waza->stopRequested;
            if (active) {
                count++;
            }
            waza = waza->next;
        }
        stage = stage->next;
    }
    return count;
}

bool wazaSequenceSysLastWazaIsImpactPoint(u8 variation)
{
    return g_LastStartedWaza != NULL ? g_LastStartedWaza->IsImpactPoint(variation) : true;
}

void wazaSequenceSysFreeAllWazaExcept(NullSequence *stage, WazaSequence *keep)
{
    if (isStageOnList(stage)) {
        WazaSequence *waza = stage->waza;
        while (waza != NULL) {
            WazaSequence *next = waza->next;
            if (waza != keep) {
                delete waza;
            }
            waza = next;
        }
    }
}

void wazaSequenceSysFreeAllWaza(void)
{
    NullSequence *stage = g_Sequences;
    while (stage != NULL) {
        stage->FreeAllWaza();
        stage = stage->next;
    }
}

static inline void _wazaSequenceSysCameraShakeRelease(void)
{
    for (int i = 0; i < 9; i++) {
        if (gCameraShakeModels[i] != NULL) {
            GSmodelFree(gCameraShakeModels[i]);
            gCameraShakeModels[i] = NULL;
        }
    }

    gCameraShakeModel = NULL;
    GSresFreeGroup(CAMERA_SHAKE_RESOURCE_GROUP);
    GSfsysFree(CAMERA_SHAKE_RESOURCE_GROUP);
}

void wazaSequenceSysRelease(void)
{
    NullSequence *stage = g_Sequences;
    while (stage != NULL) {
        NullSequence *next = stage->next;
        delete stage;
        stage = next;
    }
    g_Sequences = NULL;

    if (g_IceParticle != NULL || g_SleepParticle != NULL) {
        GSresFreeGroup(STATUS_PARTICLE_RESOURCE_GROUP);
        GSfsysFree(STATUS_PARTICLE_RESOURCE_GROUP);
        g_IceParticle = NULL;
        g_SleepParticle = NULL;
    }

    _wazaSequenceSysCameraShakeRelease();

    _modelShadowCount = 0;
    _modelShadowLight = NULL;
    memset(_modelShadowList, 0, _modelShadowCount * sizeof(void *));

    if (g_SoundListener != NULL) {
        GSsndFreeListener(g_SoundListener);
        g_SoundListener = NULL;
    }
}

bool wazaSequenceSysIsValidSequence(NullSequence *sequence)
{
    return isStageOnList(sequence);
}

bool wazaSequenceSysIsValidWaza(WazaSequence *sequence)
{
    NullSequence *stage = g_Sequences;
    if (sequence == NULL) {
        return false;
    }
    while (stage != NULL) {
        WazaSequence *waza = stage->waza;
        while (waza != NULL) {
            if (waza == sequence) {
                return true;
            }
            waza = waza->next;
        }
        stage = stage->next;
    }
    return false;
}

void wazaSequenceSysEnableLayerChange(void)
{
    wazaLayerChangeEnabled = true;
}

} // extern "C"
