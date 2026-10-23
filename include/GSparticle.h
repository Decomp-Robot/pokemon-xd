#ifndef GSPARTICLE_H
#define GSPARTICLE_H

#include <types.h>
#include <GSgfx.h>
#include <particleEngine.h>

struct _GSmodel;
struct _HSD_JObj;

/* Handle GSmodelGetPart hands out for one part of a model: GSpartCreate takes
 * it from a pool of 0xC-byte handles, GSpartFree gives it back. The generator
 * slots keep the part they are attached to as the model pointer plus the part
 * index. */
struct GSpart {
    u8 mInUse;         // 0x00: set while the handle is taken from the pool.
    u8 _01;
    u16 mIndex;        // 0x02: index of the part inside the model.
    _GSmodel *mModel;  // 0x04: model the part belongs to.
    _HSD_JObj *mJObj;  // 0x08: render JObj of the part.
};

/* Which components of the model part a generator follows while it is attached.
 * These are enumerated combinations, not bit sets: the generator update
 * switches on the exact value, so a combined mode has its own selector rather
 * than being the union of the single-component ones. */
enum GSattachMode {
    GS_ATTACH_NONE = 0,
    GS_ATTACH_POSITION = 1,
    GS_ATTACH_ROTATION = 2,
    GS_ATTACH_SCALE = 3,
    GS_ATTACH_POSITION_ROTATION = 4,
    GS_ATTACH_ROTATION_SCALE = 5,
    GS_ATTACH_POSITION_SCALE = 6,
    GS_ATTACH_ALL = 7,
};

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

/* The generator slots of the banks that still have a bank link are updated
 * every frame; _particlePreUpdate collects their cameras here so the render
 * pass can set up a projection per camera. The array lives in .bss, the count
 * in .sbss. */
extern _GScamera *GSparticleCameras[4];
extern u32 GSparticleCameraCount;
/* Loaded banks (array of GSparticleBankCount elements, allocated by
 * GSparticleInit) and the fractional engine tick carried between frames. */
extern _GSparticleBank *GSparticleBanks;
extern u32 GSparticleBankCount;
extern f32 GSparticleTickFraction;
/* Clear until _particlePreUpdate runs for the first time, when it starts the
 * fractional engine tick from zero. GSparticleFrameSkipCounter counts the
 * frames of a 50Hz display, which needs an extra engine tick every fifth
 * frame to keep the 60Hz tick rate. */
extern s8 GSparticleTickInitialised;
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
/* Module data of the particle layer, GSparticleModuleData.cpp: the render
 * module descriptor GSgfxRenderModuleRegister is handed and, behind it, the
 * texts GSparticleLoad logs about the files it is given. The loader reads the
 * texts through the block and GSparticleInit copies the descriptor out of it,
 * so both view it with this type. */
typedef struct _GSparticleModuleData {
    GSgfxRenderModule mRenderModule; // 0x00: entry of the render module list.
    char mZDrawWarning[0x50];        // 0x1C: kept for a warning never logged.
    char mGpt0Banner[0x5C];          // 0x6C: banner above the message below.
    char mGpt0Unsupported[0x5C];     // 0xC8: version 0 files are unsupported.
    char mUnknownVersion[0x20];      // 0x124: unrecognised file version.
    char mVersionMismatch[0x44];     // 0x144: data bank version mismatch.
} _GSparticleModuleData;

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
void GSparticleSetVelocityRotationInLocal(_GSparticleGen *gen, bool enable);
void GSparticleSetNodeScaling(_GSparticleGen *gen, bool enable);
void GSparticleSetParticleTexScaling(_GSparticleGen *gen, bool enable);
void GSparticleSetLocalVelocityRotation(_GSparticleGen *gen, bool enable);
void GSparticleSetOffsetRotationInLocal(_GSparticleGen *gen, bool enable,
                                        bool mode);
void GSparticleSetLocalOffsetRotation(_GSparticleGen *gen, bool enable,
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
