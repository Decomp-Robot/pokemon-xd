#ifndef GSPARTICLE_H
#define GSPARTICLE_H

#include <types.h>
#include <GSgfx.h>
#include <particleEngine.h>
#include <GSpart.h>
#include <GSattach.h>

struct _GSmodel;
struct _HSD_JObj;

struct _GSparticleBank;

/* Bookkeeping record a generator slot owns when it is not one of a bank's
 * preallocated slots: GSparticleCreateGenerator allocates it and GSparticle-
 * DestroyGenerator releases it together with the engine generator. The
 * transforms it carries are the copies the slot last applied. */
struct _GSparticleGenWork {
    u8 mBankGenerator;        // 0x00: the creation call's bank-generator flag.
    u8 mOneShotUpdate;        // 0x01: engine generator ran in one-shot mode.
    u16 mIndex;               // 0x02: index of the generator inside the bank.
    u32 mCreateTime;          // 0x04: vsync stamp the slot was created at.
    _psGenerator *mGenerator; // 0x08: engine generator the slot drives.
    GSvec mPosition;          // 0x0C
    GSvec mRotation;          // 0x18: Euler radians.
    GSvec mScale;             // 0x24
};

/* One generator slot. A bank owns an array of these (stride 0x3C); the slot
 * only holds the state the GS layer keeps about a generator, while the engine
 * object it drives hangs off mLink. */
struct _GSparticleGen {
    u8 mBankGenerator;   // 0x00: mLink is a _peBank, not a _GSparticleGenWork.
    u8 mOneShotAttach;   // 0x01: refresh the attachment once, then stop.
    bool mUpdatePending; // 0x02: _particleUpdateAttachment runs while set.
    u8 _03;
    u32 mPartIndex;      // 0x04: index of the model part it is attached to.
    GSattachMode mAttachMode; // 0x08: 0 while detached.
    _GSmodel *mModel;    // 0x0C: model the attached part belongs to.
    GSvec mLocalPosition; // 0x10: offset in the part's local space.
    GSvec mLocalRotation; // 0x1C: Euler radians, added to the part rotation.
    GSvec mLocalScale;    // 0x28: multiplied into the part scale.
    _GSparticleBank *mBank; // 0x34: bank that owns the slot.
    void *mLink;          // 0x38: _peBank or _GSparticleGenWork (see 0x00).
};

/* One loaded particle bank file, an element of the array GSparticleInit
 * allocates. The bank owns the generator slots its file describes. */
struct _GSparticleBank {
    u8 mActive;         // 0x00: the bank holds a loaded file.
    u8 mBankID;         // 0x01: id handed to psCreateGeneratorID / peBankCreate.
    u8 mHasSlots;       // 0x02: the file preallocates its generator slots.
    u8 mGeneratorCount; // 0x03: number of generator slots.
    u8 mDisactivated;   // 0x04: bank switched off by GSparticleDisactivateBank.
    u8 _05[0x03];
    void *mFileData;           // 0x08: packed particle file (peBankCreate arg).
    _GSparticleGen *mGenerators; // 0x0C: slot array, stride 0x3C.
    _GScamera *mCamera;        // 0x10: camera the file preallocated slots use.
};

/* Cameras used by the active generators in this frame. */
extern _GScamera *g_particle_cameras[4];
extern u32 g_particle_cameras_count;
/* Loaded banks, allocated by GSparticleInit. */
extern _GSparticleBank *_particleBankStructList;
extern u32 _particleBankStructCount;
/* A 50Hz display needs an extra engine tick every fifth frame. */
extern u8 GSparticleFrameSkipCounter;
/* Set when an engine bank or a generator has been started this frame and the
 * layer command word (GSgfxLayer +0x04) still has to carry its Z draw flag;
 * _particleRender publishes the flag and clears the byte. */
extern u8 GSparticleZDrawPendingBank;
extern u8 GSparticleZDrawPendingGenerator;
/* Zero vector of the GS math module's GSvec object, one of the vectors its
 * static initialiser fills in; this unit resets an engine bank's transform to
 * it while a generator is attached to a model part. */
extern GSvec zeroVector;

/* ---- particle layer entry points (this unit) ---- */

void _particlePreUpdate(f32 delta);
void _particleRender(GSgfxLayer *layer, GSgfxLayerPass pass);
void _particlePostUpdate(f32 delta);
void _particleUpdateAttachment(_GSparticleGen *gen);

extern "C" {

_GSparticleBank *GSparticleLoad(void *fileData);
void GSparticleFree(_GSparticleBank *bank);
_GSparticleGen *GSparticleCreateGenerator(_GSparticleBank *bank, u32 index,
                                          bool bankGenerator);
void GSparticleDestroyGenerator(_GSparticleGen *gen, bool killFamily);
void GSparticleDestroyAllGenerators(_GSparticleBank *bank, bool killFamily);
void GSparticleStopGenerator(_GSparticleGen *gen);
void GSparticleGeneratorSetCamera(_GSparticleGen *gen, _GScamera *camera);
void GSparticleBankSetCamera(_GSparticleBank *bank, _GScamera *camera);
void GSparticleSetBankVisibility(_GSparticleBank *bank, bool visible);
void GSparticleSetGeneratorVisibility(_GSparticleGen *gen, bool visible);
void GSparticleSetGeneratorZDraw(_GSparticleGen *gen, bool enable);
void GSparticleSetRandomVelocityScaling(_GSparticleGen *gen, bool enable);
void GSparticleSetNodeScaling(_GSparticleGen *gen, bool enable);
void GSparticleSetParticleTextureScaling(_GSparticleGen *gen, bool enable);
void GSparticleSetVelocityRotationInLocal(_GSparticleGen *gen, bool enable);
void GSparticleSetTornadoScaling(_GSparticleGen *gen, bool enable,
                                        bool mode);
void GSparticleSetOffsetRotationInLocal(_GSparticleGen *gen, bool enable,
                                      bool mode);
void GSparticleActivateBank(_GSparticleBank *bank);
void GSparticleDisactivateBank(_GSparticleBank *bank);
bool GSparticleIsGeneratorAlive(_GSparticleGen *gen);
s32 GSparticleGetGeneratorLife(_GSparticleGen *gen);
void GSparticleSetGeneratorScale(_GSparticleGen *gen, GSvec *scale,
                                 bool enableAngleRadiusScale,
                                 bool angleRadiusScale);
void GSparticleSetGeneratorRotation(_GSparticleGen *gen, GSvec *rotation);
void GSparticleSetGeneratorPosition(_GSparticleGen *gen, GSvec *position);
void GSparticleDetachGeneratorFromGSpart(_GSparticleGen *gen, bool keepLink);
void GSparticleAttachGeneratorToGSpart(_GSparticleGen *gen, GSpart *part,
                                       GSattachMode mode, bool oneShot,
                                       bool resetTransform, bool linkChildren);
void GSparticleInit(u32 bankCount);

}  // extern "C"

#endif
