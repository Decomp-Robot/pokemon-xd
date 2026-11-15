#include <app/wazaSequence/billboard.h>
#include <app/wazaSequence/wazaSequenceSys.h>
#include <app/floor/floorLoad.h>
#include <app/camera/camera.h>
#include <GSres.h>
#include <GSAPI/GSvec/GSvec.h>
#include <libc/math.h>

namespace {
const unsigned long kEffectResourceGroup = 20000;
const unsigned long kSequenceDataAlignment = 32;

struct BillboardParameters {
    u32 archiveSize;
    void* modelArchive;
    u8 unusedHeaderFields[8];
    u32 animationIndex;
    u32 generatorIndex;
    GSmodelAnimType animationMode;
    u8 unusedTail[4];
};
}

billboardEffect::billboardEffect(u8** stream)
{
    void* archive;
    BillboardParameters* parameters = (BillboardParameters*)*stream;
    mAnimationIndex = parameters->animationIndex;
    switch (parameters->animationMode) {
    case GS_MODEL_ANIM_ONCE:
        mAnimationType = GS_MODEL_ANIM_ONCE;
        break;
    case GS_MODEL_ANIM_LOOP:
        mAnimationType = GS_MODEL_ANIM_LOOP;
        break;
    }
    mGeneratorIndex = parameters->generatorIndex;
    *stream += sizeof(BillboardParameters);
    *stream = (u8*)(((unsigned long)*stream + kSequenceDataAlignment - 1) & ~(kSequenceDataAlignment - 1));
    mModel = 0;
    mParticleBank = 0;
    mGenerator = 0;
    mModelResourceID = 0;
    archive = parameters->modelArchive;
    if (archive) {
        mModelResourceID = wazaSequenceSysGetResID();
        loadModel(archive, kEffectResourceGroup, 0, mModelResourceID);
        GSresIsUniqueID(kEffectResourceGroup, mModelResourceID, false);
        mModel = (GSmodel*)GSresGetResource(kEffectResourceGroup, mModelResourceID);
        if (mModel) {
            GSmodelSetVisibility(mModel, false);
        }
    }
    *stream += (parameters->archiveSize + kSequenceDataAlignment - 1) & ~(kSequenceDataAlignment - 1);
    mParticleResourceID = 0;
}

bool billboardEffect::_start()
{
    if (!mModel && !mParticleBank) {
        return false;
    }
    if (mParticleBank) {
        mGenerator = GSparticleCreateGenerator(mParticleBank, mGeneratorIndex, true);
    }
    if (mModel) {
        GSgfxLayerID layers[] = { GSGFX_LAYER_FILTER };
        GSmodelSetVisibility(mModel, true);
        GSmodelSetBoundCheck(mModel, false);
        GSmodelSetRenderLayer(mModel, 1, layers);
        if (GSmodelCanAnimate(mModel)) {
            GSmodelSetAnimIndex(mModel, mAnimationIndex);
            GSmodelSetAnimType(mModel, mAnimationType);
            GSmodelSetAnimRate(mModel, 0.5f);
            GSmodelSetAnimFrame(mModel, 0.0f);
            GSmodelStartAnimation(mModel);
        }
        if (GSmodelCanTexAnimate(mModel)) {
            GSmodelSetTexAnimIndex(mModel, mAnimationIndex);
            GSmodelSetTexAnimType(mModel, mAnimationType);
            GSmodelSetTexAnimRate(mModel, 0.5f);
            GSmodelSetTexAnimFrame(mModel, 0.0f);
            GSmodelStartTexAnimation(mModel);
        }
    }
    return true;
}

bool billboardEffect::_update(f32 delta)
{
    GScamera* camera = cameraGetActive();
    GSvec position(0.0f, 0.0f, 0.0f);
    GSvec scale(1.0f, 1.0f, 1.0f);
    bool alive = false;
    if (camera) {
        GScameraLoadActiveCameraMatrix(GSGFX_LAYER_FILTER);
        GScameraGetPosition(camera, &position);
        GSvec interest, direction;
        GScameraGetLookAt(camera, &direction, &interest);
        PSVECSubtract(&interest, &position, &direction);
        PSVECNormalize(&direction, &direction);
        f32 fov, nearPlane, aspect, farPlane;
        GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
        nearPlane += 1.0f;
        fov = (3.1415927f / 180.0f) * fov;
        fov *= 0.5f;
        scale.y = 2.0f * nearPlane * (f32)tan(fov);
        scale.x = scale.y * aspect;
        scale.z = 0.0f;
        PSVECScale(&direction, &direction, nearPlane);
        PSVECAdd(&position, &direction, &position);
    }
    if (mGenerator && GSparticleGetGeneratorLife(mGenerator)) {
        GSparticleSetGeneratorPosition(mGenerator, &position);
        alive = true;
    }
    if (mModel &&
        ((GSmodelCanAnimate(mModel) && !GSmodelHasAnimationEnded(mModel)) ||
         (GSmodelCanTexAnimate(mModel) && !GSmodelHasTexAnimationEnded(mModel)))) {
        GSvec size;
        GSboundGetSize(GSmodelGetBound(mModel), &size, false, false);
        scale.x /= size.x;
        scale.y /= size.y;
        GSmodelSetPosition(mModel, &position);
        GSmodelSetScale(mModel, &scale);
        alive = true;
    }
    return alive;
}

void billboardEffect::_stop()
{
    if (mParticleBank) {
        GSparticleDestroyAllGenerators(mParticleBank, true);
    }
    if (mModel) {
        GSmodelStopAnimation(mModel);
        GSmodelStopTexAnimation(mModel);
        GSmodelSetVisibility(mModel, false);
    }
}

billboardEffect::~billboardEffect()
{
    Stop();
    if (mParticleBank) {
        GSparticleFree(mParticleBank);
    }
    if (mModelResourceID) {
        GSresFreeResource(kEffectResourceGroup, mModelResourceID);
    }
    if (mParticleResourceID) {
        GSresFreeResource(kEffectResourceGroup, mParticleResourceID);
    }
}
