#include <GSmem.h>
#include <GSmath.h>
#include <GSquat.h>
#include <GScamera.h>
#include <GSparticle.h>
#include <GSmodel.h>
#include <GSlog.h>
#include <HSD_JObj.h>

/* GS particle layer: the particle API the game code drives, the frame hooks
 * the GS render module list calls and the loader of the packed particle bank
 * files. The module keeps a small state block per loaded bank and hands the
 * generator slots of it out to the game. */

/* ---- other units the layer calls ---- */

/* Frame services of the GS engine: the CPU profile entries a hook opens and
 * closes, the video frame rate and the render module list. */
extern "C" {
u32 GSgfxPerfbarCPUEntry(u32 tag);
u8 GSgfxVideoGetVsyncRate(void);
u32 GSgfxVideoGetVsyncCount(void);
}

/* Render module list of the frame and the barrier that has to be waited for
 * before a bank is torn down while the renderer may still read it. */
void GSgfxRenderWaitForDrawComplete(void);
void GSgfxRenderModuleRegister(u8 id, GSgfxRenderModule *module, void *param);

/* Memory manager of the GS layer. GSmemAlloc hands the block out without
 * clearing it, every caller initialises what it reads. */
extern "C" {
void DCFlushRange(void *addr, u32 size);
}

/* Hooks of the engine and of the JObj layer the particle layer installs. */
void _particleBankDestroyOldestGenerator(_GSparticleBank *bank);
void _particleJObjCallback(int, int, int, _HSD_JObj *jobj);

/* Tag the update hooks label their CPU profile entry with, and the tag the
 * render hook uses. */
enum {
    GS_PARTICLE_UPDATE_PROFILE_TAG = 0x00800080,
    GS_PARTICLE_PROFILE_TAG = 0x00FF00FF,
};

/* Display rates the frame hook compensates for: the engine runs one tick per
 * frame of a 60Hz display, so a 50Hz display steps every fifth frame twice to
 * keep that tick rate. */
enum {
    GS_PARTICLE_DISPLAY_HZ_50 = 50,
    GS_PARTICLE_EXTRA_TICK_PERIOD = 5,
};

/* The layers the particle module draws on, each one carrying one display
 * group of the particles. The ids are the ones the layer set is configured
 * with. */
enum GSParticleLayerID {
    GS_PARTICLE_LAYER_GROUP_0 = 2,
    GS_PARTICLE_LAYER_GROUP_1 = 4,
};

/* Visibility bit of the engine bank a preallocated generator slot drives. */
enum PEBankFlag {
    PE_BANK_FLAG_VISIBLE = 0x4,
};

static inline GSgfxRenderModule _particleRenderModule(void)
{
    GSgfxRenderModule module = {
        0,
        _particlePreUpdate,
        _particleRender,
        0,
        _particlePostUpdate,
        0,
        GSGFX_RENDER_MODULE_SIGNATURE,
    };
    return module;
}

_GScamera *g_particle_cameras[4];
u32 g_particle_cameras_count;
_GSparticleBank *_particleBankStructList;
u32 _particleBankStructCount;
u8 GSparticleFrameSkipCounter;
u8 GSparticleZDrawPendingBank;
u8 GSparticleZDrawPendingGenerator;

/* ---- generator state helpers ---- */

/* Frames of life the engine half a slot drives has left: a bank generator
 * reports the life of its own particle file, a private generator the maximum
 * life of the children it spawns. -1 means the generator never ends. */
inline u32 GSparticleGenGetLife(_GSparticleGen *gen)
{
    if (gen->mLink == 0) return 0;

    if (gen->mBankGenerator != 0) {
        if (peBankIsInfinite((_peBank *)gen->mLink)) return -1;
        return (u32)(peBankGetLife((_peBank *)gen->mLink) * (f32)GSgfxVideoGetVsyncRate());
    }

    if (((_GSparticleGenWork *)gen->mLink)->mOneShotUpdate == 1) return -1;
    return psGetGeneratorChildMaxLife(((_GSparticleGenWork *)gen->mLink)->mGenerator);
}

/* True while the engine half a slot drives still has particles. A bank
 * generator follows the life of its particle file; a private generator is
 * interpreted once, so it is alive while its one-shot update is pending and
 * while one of the particles it spawned is still there. */
inline bool GSparticleGenIsAlive(_GSparticleGen *gen)
{
    if (gen->mLink == 0) return false;

    if (gen->mBankGenerator != 0) return GSparticleGenGetLife(gen) != 0;

    if (((_GSparticleGenWork *)gen->mLink)->mOneShotUpdate != 0) return true;
    if (psGetParticleChildCount(((_GSparticleGenWork *)gen->mLink)->mGenerator) != 0) return true;

    return GSparticleGenGetLife(gen) != 0;
}

/* Camera a generator slot is interpreted with: a slot that owns a preallocated
 * engine bank carries the camera on the bank, every other slot on the engine
 * generator record. */
inline _GScamera *GSparticleGenGetCamera(_GSparticleGen *gen)
{
    if (gen->mLink == 0) return 0;

    if (gen->mBankGenerator != 0) {
        _peBank *bank = (_peBank *)gen->mLink;
        if (bank != 0) return bank->mCamera;
        return 0;
    }
    return psGeneratorGetCamera(((_GSparticleGenWork *)gen->mLink)->mGenerator);
}

inline void GSparticleGenApplyCamera(_GSparticleGen *gen, _GScamera *camera)
{
    if (gen->mLink == 0) return;

    if (gen->mBankGenerator != 0) {
        _peBank *bank = (_peBank *)gen->mLink;
        if (bank != 0) bank->mCamera = camera;
    } else {
        psGeneratorSetCamera(((_GSparticleGenWork *)gen->mLink)->mGenerator, camera);
    }
}

inline void GSparticleBankApplyCamera(_GSparticleBank *bank, _GScamera *camera)
{
    _GSparticleGen *gen;
    u8 count;

    if (bank == 0 || bank->mActive == 0) return;

    bank->mCamera = camera;
    count = bank->mGeneratorCount;
    gen = bank->mGenerators;
    while (count-- != 0) {
        if (gen->mLink != 0) GSparticleGenApplyCamera(gen, camera);
        gen++;
    }
}

/* Smallest magnitude a generator scale component is allowed to have. */
const f32 GS_PARTICLE_MIN_SCALE = 1.0e-5f;

/* Clamps one scale component to the smallest magnitude a transform tolerates,
 * keeping the sign of the component. */
inline void GSparticleClampScaleComponent(f32 *component)
{
    f32 value;
    f32 magnitude;

    value = *component;
    magnitude = (value > 0.0f) ? value : -value;
    if (magnitude < GS_PARTICLE_MIN_SCALE)
        *component = (value < 0.0f) ? -GS_PARTICLE_MIN_SCALE : GS_PARTICLE_MIN_SCALE;
}

/* Writes a generator position into the engine half it drives. */
inline void GSparticleGenApplyPosition(_GSparticleGen *gen, const GSvec *position)
{
    if (gen->mBankGenerator != 0) {
        _peBank *bank = (_peBank *)gen->mLink;
        if (bank != 0) bank->transform.position = *position;
    } else {
        _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;
        work->mPosition = *position;
        work->mGenerator->mPosition.x = position->x;
        work->mGenerator->mPosition.y = position->y;
        work->mGenerator->mPosition.z = position->z;
    }
}

/* Writes a generator rotation (Euler radians) into the engine half it drives;
 * a bank generator keeps its rotation as a quaternion. */
inline void GSparticleGenApplyRotation(_GSparticleGen *gen, const GSvec *euler)
{
    if (gen->mBankGenerator != 0) {
        f32 quat[4];
        _peBank *bank;

        EulerToQuat(euler, quat);
        bank = (_peBank *)gen->mLink;
        if (bank != 0) {
            bank->transform.rotation[0] = quat[0];
            bank->transform.rotation[1] = quat[1];
            bank->transform.rotation[2] = quat[2];
            bank->transform.rotation[3] = quat[3];
        }
    } else {
        _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;
        work->mRotation = *euler;
        work->mGenerator->mRotation.x = euler->x;
        work->mGenerator->mRotation.y = euler->y;
        work->mGenerator->mRotation.z = euler->z;
    }
}

/* Writes a generator scale into the engine half it drives. Every component is
 * clamped first: a transform cannot carry a scale below 1.0e-5f. */
inline void GSparticleGenApplyScale(_GSparticleGen *gen, GSvec *scale)
{
    GSparticleClampScaleComponent(&scale->x);
    GSparticleClampScaleComponent(&scale->y);
    GSparticleClampScaleComponent(&scale->z);

    if (gen->mBankGenerator != 0) {
        _peBank *bank = (_peBank *)gen->mLink;
        if (bank != 0) bank->transform.scale = *scale;
    } else {
        _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;
        work->mScale = *scale;
        work->mGenerator->mScale.x = scale->x;
        work->mGenerator->mScale.y = scale->y;
        work->mGenerator->mScale.z = scale->z;
    }
}

/* The record of a generator slot that is not one of a bank's preallocated
 * slots keeps the transform the slot itself carries; attaching a slot starts
 * that transform from the identity. */
inline void GSparticleGenWorkResetPosition(_GSparticleGen *gen)
{
    _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;

    work->mPosition.x = 0.0f;
    work->mPosition.y = 0.0f;
    work->mPosition.z = 0.0f;
}

inline void GSparticleGenWorkResetRotation(_GSparticleGen *gen)
{
    _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;

    work->mRotation.x = 0.0f;
    work->mRotation.y = 0.0f;
    work->mRotation.z = 0.0f;
}

inline void GSparticleGenWorkResetScale(_GSparticleGen *gen)
{
    _GSparticleGenWork *work = (_GSparticleGenWork *)gen->mLink;

    work->mScale.x = 1.0f;
    work->mScale.y = 1.0f;
    work->mScale.z = 1.0f;
}

/* Applies a visibility flag to the engine half a slot drives: a preallocated
 * bank slot carries the flag in its engine bank, a dynamic slot forwards it to
 * the generator it owns. */
static inline void GSparticleGenSetVisibility(_GSparticleGen *gen, bool visible)
{
    if (gen->mBankGenerator != 0) {
        _peBank *bank = (_peBank *)gen->mLink;
        if (bank != 0) {
            if (visible) bank->mFlags |= PE_BANK_FLAG_VISIBLE;
            else bank->mFlags &= ~PE_BANK_FLAG_VISIBLE;
        }
    } else {
        psSetParticleVisibility(((_GSparticleGenWork *)gen->mLink)->mGenerator, visible);
    }
}

void _particleRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    bool fog;
    _GScamera *camera = GScameraGetActiveCamera();
    u32 group = GS_PARTICLE_DISP_GROUP_0;

    fog = layer->mFog != 0;

    if (camera == 0 || !GScameraIsValid(camera)) return;

    u32 profile = GSgfxPerfbarCPUEntry(GS_PARTICLE_PROFILE_TAG);

    if (GSparticleZDrawPendingBank != 0) {
        layer->mRenderFlags |= GSGFX_RENDER_FLAG_ZDRAW_BANK;
        GSparticleZDrawPendingBank = 0;
    }
    if (GSparticleZDrawPendingGenerator != 0) {
        layer->mRenderFlags |= GSGFX_RENDER_FLAG_ZDRAW_GENERATOR;
        GSparticleZDrawPendingGenerator = 0;
    }
    if (fog) layer->fog(false);

    switch (layer->mLayerID) {
    case GS_PARTICLE_LAYER_GROUP_1:
        group = GS_PARTICLE_DISP_GROUP_1;
        /* The second layer draws the same two lists of the other group. */
    case GS_PARTICLE_LAYER_GROUP_0:
        if (pass == GSGFX_LAYER_PASS_XLU && psDispHasRenderData(group) == 1) {
            bool rendered = false;

            layer->mClipMode = GS_GFX_CLIP_ENABLED;
            layer->mPassState[0].mGroupBFlags |= GSGFX_PASS_GROUP_B_PARTICLES;
            layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
            layer->mPassState[1].mGroupBFlags |= GSGFX_PASS_GROUP_B_PARTICLES;
            layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
            GScameraLoadProjection(camera, layer);
            layer->FlushState();
            psDispSetProjection(&layer->mProjectionState.mType);
            psSetBillboardCamera(camera->mCObj);
            psDispParticles(group, GS_PARTICLE_DISP_MODE_Z, 0);
            psDispParticles(group, GS_PARTICLE_DISP_MODE_PLAIN, 0);
            layer->InvalidateState();
            _GScamera *cam;
            for (u32 i = 0; i < g_particle_cameras_count; i++) {
                cam = g_particle_cameras[i];
                if (!GScameraIsValid(cam)) continue;
                rendered = true;
                GScameraLoadProjection(cam, layer);
                layer->FlushState();
                psDispSetProjection(&layer->mProjectionState.mType);
                psSetBillboardCamera(cam->mCObj);
                psDispParticles(group, GS_PARTICLE_DISP_MODE_Z, cam);
                psDispParticles(group, GS_PARTICLE_DISP_MODE_PLAIN, cam);
                layer->InvalidateState();
            }
            layer->mTexCoordLineOffset0 = 0;
            layer->mTexCoordPointOffset0 = 0;
            layer->mPassState[0].mGroupAFlags |= GSGFX_PASS_GROUP_A_PARTICLES;
            layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
            layer->mPassState[1].mGroupAFlags |= GSGFX_PASS_GROUP_A_PARTICLES;
            layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
            if (rendered) GScameraLoadActiveCameraMatrix(layer->mLayerID);
        }
        /* Every layer of the set ends the pass in the engine's layer call. */
    case 0:
    case 1:
    case 3:
    case 5:
        particleEngineRender(layer);
        break;
    }

    if (fog) layer->fog(true);
    GSgfxPerfbarCPUEntry(profile);
}

void _particlePostUpdate(f32 delta)
{
    u32 bankIndex;
    u32 slot;
    u32 perfbar;
    _GSparticleGen *gen;
    _GSparticleBank *bank;

    perfbar = GSgfxPerfbarCPUEntry(GS_PARTICLE_UPDATE_PROFILE_TAG);

    for (bankIndex = 0; bankIndex < _particleBankStructCount; bankIndex++) {
        bank = &_particleBankStructList[bankIndex];
        if (bank->mActive != 1) continue;

        gen = bank->mGenerators;
        for (slot = 0; slot < bank->mGeneratorCount; slot++, gen++) {
            /* A slot that was never linked to the engine has nothing to release. */
            if (gen->mLink == 0) continue;

            if (!GSparticleGenIsAlive(gen)) GSparticleDestroyGenerator(gen, 1);
        }
    }

    particleEnginePostUpdate();

    GSgfxPerfbarCPUEntry(perfbar);
}

void _particlePreUpdate(f32 delta)
{
    u32 bankIndex;
    u32 slot;
    u32 perfbar;
    u32 ticks;
    _GSparticleGen *gen;
    _GSparticleBank *bank;
    _GScamera *camera;
    u32 cameraCount;
    u32 i;

    static f32 sysdolRemain = 0.0f;

    perfbar = GSgfxPerfbarCPUEntry(GS_PARTICLE_UPDATE_PROFILE_TAG);
    memset(g_particle_cameras, 0, sizeof(g_particle_cameras));
    g_particle_cameras_count = 0;

    for (bankIndex = 0; bankIndex < _particleBankStructCount; bankIndex++) {
        bank = &_particleBankStructList[bankIndex];
        if (bank->mActive != 1) continue;

        if (bank->mCamera != 0 && !GScameraIsValid(bank->mCamera))
            GSparticleBankApplyCamera(bank, 0);

        gen = bank->mGenerators;
        for (slot = 0; slot < bank->mGeneratorCount; slot++, gen++) {
            if (gen->mLink == 0) continue;

            _particleUpdateAttachment(gen);
            camera = GSparticleGenGetCamera(gen);

            if (GScameraIsValid(camera)) {
                bool found = false;

                cameraCount = g_particle_cameras_count;
                for (i = 0; i < cameraCount; i++) {
                    if (g_particle_cameras[i] == camera) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    g_particle_cameras[cameraCount] = camera;
                    g_particle_cameras_count++;
                }
            } else {
                GSparticleGenApplyCamera(gen, 0);
            }
        }
    }

    /* The delta adds up to whole engine ticks, the fraction left over is
     * carried to the next frame. */
    ticks = (u32)delta;
    sysdolRemain += delta - (f32)ticks;
    if (sysdolRemain >= 1.0f) {
        u32 extra = (u32)sysdolRemain;

        sysdolRemain -= (f32)extra;
        ticks += extra;
    }

    /* A 50Hz display feeds the engine fewer frames than the 60 ticks a second
     * it runs on, so every fifth frame is stepped twice. */
    if (GSgfxVideoGetVsyncRate() == GS_PARTICLE_DISPLAY_HZ_50) {
        GSparticleFrameSkipCounter++;
        if (GSparticleFrameSkipCounter >= GS_PARTICLE_EXTRA_TICK_PERIOD) {
            GSparticleFrameSkipCounter = 0;
            ticks++;
        }
    }

    camera = GScameraGetActiveCamera();
    if (camera != 0 && GScameraIsValid(camera)) {
        psSetBillboardCamera(camera->mCObj);
        for (i = 0; i < ticks; i++) {
            psInterpretParticles(0);
            psExecGenerator(0);
        }
    }

    particleEngineUpdate(delta / (f32)GSgfxVideoGetVsyncRate());
    psDispBeginFrame();

    GSgfxPerfbarCPUEntry(perfbar);
}

void _particleUpdateAttachment(_GSparticleGen *gen)
{
    GSvec position;
    GSvec rotation;
    GSvec scale;
    f32 quat[4];
    GSvec offset;
    GSpart *part;

    if (gen->mAttachMode == GS_ATTACH_NONE) return;
    if (gen->mUpdatePending == 0) return;

    part = GSmodelGetPart(gen->mModel, gen->mPartIndex);
    GSpartGetTransform(part, &position, &rotation, &scale);
    GSpartFree(part);

    /* The generator's local offset rotates with the part. */
    EulerToQuat(&rotation, quat);
    GSvecTransformQuat(&offset, quat, &gen->mLocalPosition);
    PSVECAdd((const Vec *)&position, (const Vec *)&offset, (Vec *)&position);
    PSVECAdd((const Vec *)&rotation, (const Vec *)&gen->mLocalRotation, (Vec *)&rotation);
    scale.x *= gen->mLocalScale.x;
    scale.y *= gen->mLocalScale.y;
    scale.z *= gen->mLocalScale.z;

    /* The engine half keeps the generator's own transform; the part only takes
     * the components the attach mode selects back over. */
    GSparticleGenApplyPosition(gen, &gen->mLocalPosition);
    GSparticleGenApplyRotation(gen, &gen->mLocalRotation);
    GSparticleGenApplyScale(gen, &gen->mLocalScale);

    switch (gen->mAttachMode) {
    case GS_ATTACH_POSITION:
        GSparticleGenApplyPosition(gen, &position);
        break;
    case GS_ATTACH_ROTATION:
        GSparticleGenApplyRotation(gen, &rotation);
        break;
    case GS_ATTACH_SCALE:
        GSparticleGenApplyScale(gen, &scale);
        break;
    case GS_ATTACH_POSITION_ROTATION:
        GSparticleGenApplyPosition(gen, &position);
        GSparticleGenApplyRotation(gen, &rotation);
        break;
    case GS_ATTACH_ROTATION_SCALE:
        GSparticleGenApplyRotation(gen, &rotation);
        GSparticleGenApplyScale(gen, &scale);
        break;
    case GS_ATTACH_POSITION_SCALE:
        GSparticleGenApplyPosition(gen, &position);
        GSparticleGenApplyScale(gen, &scale);
        break;
    case GS_ATTACH_ALL:
        GSparticleGenApplyPosition(gen, &position);
        GSparticleGenApplyRotation(gen, &rotation);
        GSparticleGenApplyScale(gen, &scale);
        break;
    }

    gen->mUpdatePending = (gen->mOneShotAttach == 0);
}

extern "C" void GSparticleGeneratorSetCamera(_GSparticleGen *gen, _GScamera *camera)
{
    GSparticleGenApplyCamera(gen, camera);
}

extern "C" void GSparticleBankSetCamera(_GSparticleBank *bank, _GScamera *camera)
{
    _GSparticleGen *gen;
    u8 count;

    if (bank == 0 || bank->mActive == 0) return;

    bank->mCamera = camera;
    count = bank->mGeneratorCount;
    gen = bank->mGenerators;
    while (count-- != 0) {
        if (gen->mLink != 0) GSparticleGeneratorSetCamera(gen, camera);
        gen++;
    }
}

extern "C" void GSparticleDestroyAllGenerators(_GSparticleBank *bank, bool killFamily)
{
    _GSparticleGen *gen = bank->mGenerators;
    for (u32 i = 0; i < bank->mGeneratorCount; i++, gen++) {
        if (gen->mLink != 0) GSparticleDestroyGenerator(gen, killFamily);
    }
}

extern "C" void GSparticleDestroyGenerator(_GSparticleGen *gen, bool killFamily)
{
    if (gen->mLink == 0) return;

    /* A slot that is still attached to a model part is released from it. */
    if (gen->mAttachMode != GS_ATTACH_NONE) GSparticleDetachGeneratorFromGSpart(gen, 0);

    if (gen->mBankGenerator != 0) {
        peBankFree((_peBank *)gen->mLink);
        gen->mLink = 0;
    } else {
        /* Dynamic slots own their generator record and the particles it has
         * already spawned. */
        if (killFamily == 1) {
            psKillFamily(((_GSparticleGenWork *)gen->mLink)->mGenerator->mFamilyID,
                         ((_GSparticleGenWork *)gen->mLink)->mGenerator->mListGroup);
        }
        psKillGenerator(((_GSparticleGenWork *)gen->mLink)->mGenerator);
        if (gen->mLink != 0) {
            GSmemFree(gen->mLink);
            gen->mLink = 0;
        }
    }
}

extern "C" void GSparticleStopGenerator(_GSparticleGen *gen)
{
    if (gen->mLink == 0) return;
    if (gen->mBankGenerator == 0) return;

    if (peBankIsInfinite((_peBank *)gen->mLink)) {
        peBankStopLoop((_peBank *)gen->mLink);
    } else {
        peBankStop((_peBank *)gen->mLink);
    }
}

extern "C" void GSparticleSetBankVisibility(_GSparticleBank *bank, bool visible)
{
    _GSparticleGen *gen = bank->mGenerators;
    for (u32 i = 0; i < bank->mGeneratorCount; i++, gen++) {
        if (gen->mLink != 0) GSparticleGenSetVisibility(gen, visible);
    }
}

extern "C" void GSparticleSetGeneratorVisibility(_GSparticleGen *gen, bool visible)
{
    if (gen->mLink == 0) return;

    GSparticleGenSetVisibility(gen, visible);
}

extern "C" void GSparticleSetGeneratorZDraw(_GSparticleGen *gen, bool enable)
{
    if (gen->mLink == 0) return;
    const char message[] = "GSparticle: changing the Z draw flag of a generator currently has no effect\n";
    GSlogWrite(message);
}

extern "C" void GSparticleSetRandomVelocityScaling(_GSparticleGen *gen, bool enable)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetRandomVelocityScaling(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable);
}

extern "C" void GSparticleSetNodeScaling(_GSparticleGen *gen, bool enable)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetNodeScaling(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable);
}

extern "C" void GSparticleSetTornadoScaling(_GSparticleGen *gen, bool enable, bool mode)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetTornadoScaling(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable, mode);
}

extern "C" void GSparticleSetParticleTextureScaling(_GSparticleGen *gen, bool enable)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetParticleTexScaling(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable);
}

extern "C" void GSparticleSetOffsetRotationInLocal(_GSparticleGen *gen, bool enable, bool mode)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetOffsetRotationInLocal(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable, mode);
}

extern "C" void GSparticleSetVelocityRotationInLocal(_GSparticleGen *gen, bool enable)
{
    if (gen->mLink == 0 || gen->mBankGenerator != 0) return;

    psSetVelocityRotationInLocal(((_GSparticleGenWork *)gen->mLink)->mGenerator, enable);
}

extern "C" void GSparticleActivateBank(_GSparticleBank *bank)
{
    bank->mDisactivated = 0;
}

extern "C" void GSparticleDisactivateBank(_GSparticleBank *bank)
{
    bank->mDisactivated = 1;
}

extern "C" {

bool GSparticleIsGeneratorAlive(_GSparticleGen *gen)
{
    return GSparticleGenIsAlive(gen);
}

s32 GSparticleGetGeneratorLife(_GSparticleGen *gen)
{
    return GSparticleGenGetLife(gen);
}

void GSparticleSetGeneratorScale(_GSparticleGen *gen, GSvec *scale, bool enableAngleRadiusScale,
                                 bool angleRadiusScale)
{
    if (gen->mLink == 0) return;

    if (gen->mAttachMode == GS_ATTACH_NONE) {
        GSparticleGenApplyScale(gen, scale);
    } else {
        gen->mLocalScale = *scale;
    }

    if (gen->mBankGenerator != 0) return;
    if (enableAngleRadiusScale != 1) return;

    psSetGeneratorAngleRadiusScale(((_GSparticleGenWork *)gen->mLink)->mGenerator, &scale->x,
                                   angleRadiusScale);
}

void GSparticleSetGeneratorRotation(_GSparticleGen *gen, GSvec *rotation)
{
    if (gen->mLink == 0) return;

    if (gen->mAttachMode == GS_ATTACH_NONE) {
        GSparticleGenApplyRotation(gen, rotation);
    } else {
        gen->mLocalRotation = *rotation;
    }
}

void GSparticleSetGeneratorPosition(_GSparticleGen *gen, GSvec *position)
{
    if (gen->mLink == 0) return;

    if (gen->mAttachMode == GS_ATTACH_NONE) {
        GSparticleGenApplyPosition(gen, position);
    } else {
        gen->mLocalPosition = *position;
    }
}

}  // extern "C"

extern "C" {

void GSparticleDetachGeneratorFromGSpart(_GSparticleGen *gen, bool keepLink)
{
    if (gen->mLink == 0) return;
    if (gen->mAttachMode == GS_ATTACH_NONE) return;

    GSmodelSet60fpsAnimFlag(gen->mModel, 0);
    gen->mModel = 0;
    gen->mPartIndex = 0;
    gen->mUpdatePending = 0;
    gen->mOneShotAttach = 0;

    if (gen->mBankGenerator == 0)
        psUnlinkChildGensFromJObj(((_GSparticleGenWork *)gen->mLink)->mGenerator);

    gen->mAttachMode = GS_ATTACH_NONE;
    if (keepLink) return;

    /* The engine half takes the generator transform over again: with the
     * attachment already cleared the setters push the local transform
     * straight down to the engine. */
    GSparticleSetGeneratorPosition(gen, &gen->mLocalPosition);
    GSparticleSetGeneratorRotation(gen, &gen->mLocalRotation);
    GSparticleSetGeneratorScale(gen, &gen->mLocalScale, false, false);
}

void GSparticleAttachGeneratorToGSpart(_GSparticleGen *gen, GSpart *part, GSattachMode mode,
                                       bool oneShot, bool resetTransform, bool linkChildren)
{
    if (gen->mLink == 0) return;
    if (gen->mAttachMode != GS_ATTACH_NONE) return;
    if (mode == GS_ATTACH_NONE) return;

    gen->mModel = part->mModel;
    gen->mPartIndex = part->mIndex;

    if (resetTransform == 1) {
        /* Start the attachment from the part transform. */
        gen->mLocalPosition.x = 0.0f;
        gen->mLocalPosition.y = 0.0f;
        gen->mLocalPosition.z = 0.0f;
        gen->mLocalRotation.x = 0.0f;
        gen->mLocalRotation.y = 0.0f;
        gen->mLocalRotation.z = 0.0f;
        gen->mLocalScale.x = 1.0f;
        gen->mLocalScale.y = 1.0f;
        gen->mLocalScale.z = 1.0f;
    } else if (gen->mBankGenerator != 0) {
        /* Continue the attachment from the transform the bank carries. */
        f32 quat[4];

        if (gen->mLink != 0) {
            gen->mLocalPosition = ((_peBank *)gen->mLink)->transform.position;
        } else {
            gen->mLocalPosition.x = 0.0f;
            gen->mLocalPosition.y = 0.0f;
            gen->mLocalPosition.z = 0.0f;
        }
        if (gen->mLink != 0) {
            quat[0] = ((_peBank *)gen->mLink)->transform.rotation[0];
            quat[1] = ((_peBank *)gen->mLink)->transform.rotation[1];
            quat[2] = ((_peBank *)gen->mLink)->transform.rotation[2];
            quat[3] = ((_peBank *)gen->mLink)->transform.rotation[3];
        } else {
            quat[0] = 0.0f;
            quat[1] = 0.0f;
            quat[2] = 0.0f;
            quat[3] = 1.0f;
        }
        GSvecMakeFromQuat(&gen->mLocalRotation, quat);
        if (gen->mLink != 0) {
            gen->mLocalScale = ((_peBank *)gen->mLink)->transform.scale;
        } else {
            gen->mLocalScale.x = 1.0f;
            gen->mLocalScale.y = 1.0f;
            gen->mLocalScale.z = 1.0f;
        }
    } else {
        gen->mLocalPosition = ((_GSparticleGenWork *)gen->mLink)->mPosition;
        gen->mLocalRotation = ((_GSparticleGenWork *)gen->mLink)->mRotation;
        gen->mLocalScale = ((_GSparticleGenWork *)gen->mLink)->mScale;
    }

    /* The attachment update writes the engine transform from now on. */
    if (gen->mBankGenerator != 0) {
        if (gen->mLink != 0) ((_peBank *)gen->mLink)->transform.position = zeroVector;
        if (gen->mLink != 0) {
            f32 *rotation = ((_peBank *)gen->mLink)->transform.rotation;

            rotation[0] = zeroQuaternion.x;
            rotation[1] = zeroQuaternion.y;
            rotation[2] = zeroQuaternion.z;
            rotation[3] = zeroQuaternion.w;
        }
        if (gen->mLink != 0) ((_peBank *)gen->mLink)->transform.scale = unitScaleVector;
    } else {
        GSparticleGenWorkResetPosition(gen);
        GSparticleGenWorkResetRotation(gen);
        GSparticleGenWorkResetScale(gen);

        if (linkChildren)
            psLinkChildGensToJObj(((_GSparticleGenWork *)gen->mLink)->mGenerator, part->mJoint);
    }

    gen->mAttachMode = mode;
    gen->mOneShotAttach = oneShot;
    gen->mUpdatePending = true;
}

}  // extern "C"

/* Id the particle render module is registered with: the render module list is
 * kept sorted by it, so the modules of a frame run in id order. */
enum { GS_PARTICLE_RENDER_MODULE_ID = 64 };
/* Byte size of each application-SRT record allocated by the engine. */
enum { GS_PARTICLE_SRT_RECORD_SIZE = 116 };

/* Slot budgets a loaded file works with: the emitter-bank format preallocates
 * one engine bank per slot, a generator file spreads over a bank of its own. */
enum { GS_PARTICLE_BANK_SLOT_COUNT = 16 };
enum { GS_PARTICLE_FILE_SLOT_COUNT = 64 };

/* Signs of the packed particle files the loader accepts. */
enum GSparticleFileMagic {
    GS_PARTICLE_FILE_MAGIC = 0x01F056DA,  // packed emitter bank file
    GS_PARTICLE_FILE_MAGIC_GPT0 = 'GPT0', // generator file, version 0
    GS_PARTICLE_FILE_MAGIC_GPT1 = 'GPT1', // generator file, version 1
};
/* Version pair the packed emitter format carries (major 3, minor 2). */
enum { GS_PARTICLE_FILE_VERSION = 0x00030002 };
/* Version of the particle data bank the engine is built against; every file
 * has to carry it in its data bank header. */
enum { GS_PARTICLE_DATA_VERSION = 67 };
/* Cache line the particle data block is flushed in before the engine reads
 * it. */
enum { GS_PARTICLE_CACHE_LINE_SIZE = 32 };
/* Bank ids are handed out of this range; the marker means every id is taken. */
enum { GS_PARTICLE_BANK_ID_MAX = 64 };
enum { GS_PARTICLE_BANK_ID_NONE = 0xFF };

/* Header of an emitter bank file: the signature followed by the version pair
 * of the format. */
typedef struct _GSparticleBankFile {
    u32 mSignature; // 0x00: GS_PARTICLE_FILE_MAGIC.
    u32 mVersion;   // 0x04: has to be GS_PARTICLE_FILE_VERSION.
} _GSparticleBankFile;

/* Header of a generator file. Everything behind the signature is stored as an
 * offset from the start of the file and patched into a pointer in place. */
typedef struct _GSparticleGenFile {
    u32 mSignature; // 0x00: GS_PARTICLE_FILE_MAGIC_GPT0 / _GPT1.
    u32 mDataBank;  // 0x04: offset of the data bank header.
    u32 mDataBlock; // 0x08: offset of the particle data block.
    u32 mDataSize;  // 0x0C: byte size of the particle data block.
    u32 mTable;     // 0x10: offset of the table block.
    u32 mUnknown;   // 0x14: not read by the loader.
} _GSparticleGenFile;

/* Index of the slot a bank starts its next generator in: the first slot that
 * carries the bank's generator tag and has no engine link yet. A slot of
 * another tag ends the search, the array belongs to a single file. */
static inline int _particleFindFreeSlot(_GSparticleBank *bank)
{
    _GSparticleGen *slot = bank->mGenerators;

    for (s32 index = 0; index < bank->mGeneratorCount; index++, slot++) {
        if (bank->mHasSlots != slot->mBankGenerator) return -1;
        if (slot->mLink == 0) return index;
    }

    return -1;
}

/* True while another loaded bank already carries the id. */
static inline bool _particleIsBankIDUsed(u8 bankID)
{
    _GSparticleBank *bank = _particleBankStructList;

    for (s32 i = 0; i < _particleBankStructCount; i++, bank++) {
        if (bank->mBankID == bankID) return true;
    }

    return false;
}

/* First bank record that does not hold a file yet. */
static inline _GSparticleBank *_particleFindFreeBank(void)
{
    _GSparticleBank *bank = _particleBankStructList;

    for (s32 i = 0; i < _particleBankStructCount; i++, bank++) {
        if (bank->mActive == 0) return bank;
    }

    return 0;
}

/* Bank record a loaded file goes into: the first record that holds no file
 * yet, 0 while every record is in use. */
static inline _GSparticleBank *_particleAllocBank(void)
{
    return _particleFindFreeBank();
}

/* Id a new bank gets: the first one no other loaded bank uses. */
static inline u8 _particleFindFreeBankID(void)
{
    for (u8 bankID = 0; bankID < GS_PARTICLE_BANK_ID_MAX; bankID++) {
        if (!_particleIsBankIDUsed(bankID)) return bankID;
    }

    return GS_PARTICLE_BANK_ID_NONE;
}

extern "C" _GSparticleGen *GSparticleCreateGenerator(_GSparticleBank *bank,
                                                     u32 index,
                                                     bool bankGenerator)
{
    _GSparticleGen *slot;
    int slotIndex;

    if (bank->mActive == 0) return 0;
    if (bank->mDisactivated != 0) return 0;

    if (bank->mHasSlots != 0) {
        /* The file preallocated an engine bank for every slot: take the free
         * slot the bank's tag is reserved for and start that engine bank. */
        _peBank *bankLink;

        slotIndex = _particleFindFreeSlot(bank);
        if (slotIndex < 0) return 0;

        bankLink = peBankCreate(bank->mFileData);
        if (bankLink == 0) return 0;

        slot = &bank->mGenerators[slotIndex];
        if (slot->mBankGenerator == 0) return 0;

        /* The engine bank takes over the camera of the bank it belongs to. */
        _GScamera *camera = bank->mCamera;
        if (bankLink != 0) bankLink->mCamera = camera;
        peBankStart(bankLink);
        slot->mLink = bankLink;
        GSparticleZDrawPendingBank = 1;
    } else {
        /* Every other file drives an engine generator of its own per slot. */
        _GSparticleGenWork *genWork;
        u32 listGroup;

        slotIndex = _particleFindFreeSlot(bank);
        if (slotIndex < 0) {
            /* Every slot is busy: the oldest generator makes room. */
            _particleBankDestroyOldestGenerator(bank);
            slotIndex = _particleFindFreeSlot(bank);
            if (slotIndex < 0) return 0;
        }

        genWork = (_GSparticleGenWork *)GSmemAlloc(sizeof(_GSparticleGenWork));
        if (genWork == 0) return 0;

        /* The group selects which particle list owns this generator. */
        listGroup = 0;
        if (bankGenerator == 1) listGroup = 1;
        genWork->mGenerator = psCreateGeneratorID(listGroup, bank->mBankID, index);
        if (genWork->mGenerator == 0) {
            GSmemFree(genWork);
            return 0;
        }

        genWork->mGenerator->mLoop = 1;
        psGeneratorSetCamera(genWork->mGenerator, bank->mCamera);
        genWork->mIndex = index;
        genWork->mBankGenerator = bankGenerator;
        genWork->mCreateTime = GSgfxVideoGetVsyncCount();
        if (genWork->mGenerator->mRemainFrames == 0) {
            genWork->mOneShotUpdate = 1;
        } else {
            genWork->mOneShotUpdate = 0;
        }

        slot = &bank->mGenerators[slotIndex];
        if (slot->mBankGenerator != 0) return 0;

        slot->mLink = genWork;
    }

    GSparticleZDrawPendingGenerator = 1;
    slot->mAttachMode = GS_ATTACH_NONE;
    slot->mModel = 0;
    slot->mPartIndex = 0;
    slot->mOneShotAttach = 0;
    slot->mUpdatePending = 0;
    slot->mBank = bank;

    return slot;
}

extern "C" void GSparticleFree(_GSparticleBank *bank)
{
    /* The slots drive engine objects the render pass may still be reading. */
    GSgfxRenderWaitForDrawComplete();
    GSparticleDestroyAllGenerators(bank, true);

    if (bank->mGenerators != 0) {
        GSmemFree(bank->mGenerators);
        bank->mGenerators = 0;
    }
    bank->mGenerators = 0;
    bank->mGeneratorCount = 0;

    if (bank->mHasSlots != 0) peBankFreeFile(bank->mFileData);

    bank->mActive = 0;
}

extern "C" _GSparticleBank *GSparticleLoad(void *fileData)
{
    _GSparticleBank *bank;
    u32 i;
    u8 bankID;

    bank = _particleAllocBank();

    if (bank == 0) return 0;

    bankID = _particleFindFreeBankID();
    if (bankID == GS_PARTICLE_BANK_ID_NONE) return 0;

    bank->mCamera = 0;
    bank->mGenerators = 0;
    bank->mDisactivated = 0;

    if (((_GSparticleBankFile *)fileData)->mSignature == GS_PARTICLE_FILE_MAGIC) {
        /* The packed emitter format carries one engine bank per slot, and the
         * file hands the bank data over itself. */
        _GSparticleBankFile *file = (_GSparticleBankFile *)fileData;

        if (file->mVersion != GS_PARTICLE_FILE_VERSION) return 0;

        bank->mHasSlots = 1;
        bank->mGeneratorCount = GS_PARTICLE_BANK_SLOT_COUNT;
        bank->mGenerators = (_GSparticleGen *)GSmemAllocClr(
            GS_PARTICLE_BANK_SLOT_COUNT * sizeof(_GSparticleGen));
        if (bank->mGenerators == 0) return 0;

        for (i = 0; i < GS_PARTICLE_BANK_SLOT_COUNT; i++) {
            bank->mGenerators[i].mBankGenerator = 1;
        }

        bank->mFileData = peBankLoadFile(fileData);
        if (bank->mFileData == 0) goto fail;
    } else {
        /* A generator file spreads over a bank of slots whose generator tag
         * stays clear. */
        _GSparticleGenFile *file = (_GSparticleGenFile *)fileData;
        s32 dataVersion;

        bank->mHasSlots = 0;
        bank->mGeneratorCount = GS_PARTICLE_FILE_SLOT_COUNT;
        bank->mGenerators = (_GSparticleGen *)GSmemAllocClr(
            GS_PARTICLE_FILE_SLOT_COUNT * sizeof(_GSparticleGen));
        if (bank->mGenerators == 0) return 0;

        switch (file->mSignature) {
        case GS_PARTICLE_FILE_MAGIC_GPT0:
            GSlogWrite("GSparticle: ***************************************************************************\n");
            GSlogWrite("GSparticle: ************* version 0 .gpt files are no longer supported ****************\n");
            GSlogWrite("GSparticle: ***************************************************************************\n");
            goto fail;

        default:
            GSlogWrite("GSparticle: unknown version\n");
            goto fail;

        case GS_PARTICLE_FILE_MAGIC_GPT1:
            /* The sections of the file are addressed through the header. */
            file->mDataBank += (u32)fileData;
            file->mDataBlock += (u32)fileData;
            file->mTable += (u32)fileData;

            dataVersion = *(u16 *)file->mDataBank;
            if (dataVersion != GS_PARTICLE_DATA_VERSION) {
                GSlogWrite("GSparticle: version mismatch between system(%d) and ptcl-file(%d)",
                            GS_PARTICLE_DATA_VERSION, dataVersion);
                goto fail;
            }

            bank->mFileData = fileData;
            DCFlushRange((void *)file->mDataBlock,
                         (file->mDataSize + GS_PARTICLE_CACHE_LINE_SIZE - 1) &
                             ~(GS_PARTICLE_CACHE_LINE_SIZE - 1));
            psInitDataBank(bankID, (void *)file->mDataBank,
                           (void *)file->mDataBlock, (void *)file->mTable, 0);
            break;
        }
    }

    bank->mActive = 1;
    bank->mBankID = bankID;

    return bank;

fail:
    if (bank->mGenerators != 0) {
        GSmemFree(bank->mGenerators);
        bank->mGenerators = 0;
    }

    return 0;
}

extern "C" void GSparticleInit(u32 bankCount)
{
    GSgfxRenderModule module = _particleRenderModule();
    u32 i;

    memset(g_particle_cameras, 0, sizeof(g_particle_cameras));

    _particleBankStructCount = bankCount;
    g_particle_cameras_count = 0;
    _particleBankStructList = (_GSparticleBank *)GSmemAlloc(bankCount *
                                                         sizeof(_GSparticleBank));
    if (_particleBankStructList == 0) return;

    for (i = 0; i < _particleBankStructCount; i++) _particleBankStructList[i].mActive = 0;

    psSetToken(0);
    psInitParticle(0);
    psInitGenerator(0);
    psInitAppSRT(0, GS_PARTICLE_SRT_RECORD_SIZE);

    HSD_JObjSetSPtclCallback(_particleJObjCallback);
    HSD_JObjSetDPtclCallback(_particleJObjCallback);
    HSD_JObjSetPtclTargetCallback(psSetPointJObjNodup);

    particleEngineInit();
    GSgfxRenderModuleRegister(GS_PARTICLE_RENDER_MODULE_ID, &module, 0);
}

extern "C" {
_GSmodel *GSmodelSearchModelList(_HSD_JObj *root);
_GSparticleBank *GSmodelGetLinkedGSparticleBank(_GSmodel *model);
bool GSmodelIsRootNullAdded(_GSmodel *model);
u32 GSpartGetJObjIndex(_HSD_JObj *root, _HSD_JObj *jobj);
GSattachMode GSmodelGetGSparticleLinkAttachMode(_GSmodel *model);
bool GSmodelGetVisibility(_GSmodel *model);
}

static const u32 GS_PART_INVALID_INDEX = ~0u;

static inline _HSD_JObj *HSD_JObjGetParent(_HSD_JObj *jobj)
{
    if (jobj == 0) return 0;
    return jobj->mParent;
}

void _particleBankDestroyOldestGenerator(_GSparticleBank *bank)
{
    int slotIndex;
    _GSparticleGen *gen;
    _GSparticleGen *generators;
    _GSparticleGenWork *work;
    u32 oldestTime = ~0u;
    int oldestIndex = -1;

    generators = bank->mGenerators;
    gen = generators;
    for (slotIndex = 0; slotIndex < bank->mGeneratorCount; slotIndex++, gen++) {
        work = (_GSparticleGenWork *)gen->mLink;
        if (work != 0 && work->mCreateTime < oldestTime) {
            oldestTime = work->mCreateTime;
            oldestIndex = slotIndex;
        }
    }
    if (oldestIndex != -1) GSparticleDestroyGenerator(&generators[oldestIndex], true);
}

void _particleJObjCallback(int, int, int generatorIndex, _HSD_JObj *jobj)
{
    _HSD_JObj *root = jobj;
    while (HSD_JObjGetParent(root) != 0) root = HSD_JObjGetParent(root);

    _GSmodel *model = GSmodelSearchModelList(root);
    if (model == 0) return;
    _GSparticleBank *bank = GSmodelGetLinkedGSparticleBank(model);
    if (bank == 0) return;
    if (GSmodelIsRootNullAdded(model)) root = root->child;

    u32 partIndex = GSpartGetJObjIndex(root, jobj);
    if (partIndex == GS_PART_INVALID_INDEX) return;
    GSpart *part = GSmodelGetPart(model, partIndex);
    if (part == 0) return;

    _GSparticleGen *gen = GSparticleCreateGenerator(bank, generatorIndex, false);
    if (gen != 0) {
        GSparticleSetParticleTextureScaling(gen, true);
        GSparticleSetRandomVelocityScaling(gen, true);
        GSparticleAttachGeneratorToGSpart(gen, part,
                                        GSmodelGetGSparticleLinkAttachMode(model),
                                        false, true, false);
        GSparticleSetGeneratorVisibility(gen, GSmodelGetVisibility(model));
    }
    GSpartFree(part);
}
