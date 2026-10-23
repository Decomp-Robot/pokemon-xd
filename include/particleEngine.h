#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include <types.h>
#include <GSgfx.h>

struct _HSD_CObj;

/* The camera the GS camera module hands out. Everything the layer does with a
 * camera goes through the HSD camera object it is built on. */
struct _GScamera {
    u8 _00[0x08];
    _HSD_CObj *mCObj; // 0x08: HSD camera object the projection comes from.
};

/* Partial views of the two objects the particle engine hands out and the GS
 * particle layer (GSparticle.cpp) binds its generators to. Only the fields
 * that cross the unit boundary are named; the rest of each object stays
 * padding. The layouts come from the accesses in both units. */

/* Generator of the engine's particle-system half (the ps* units).
 * psInitGenerator allocates a pool of 0xB8-byte generators. */
struct _psGenerator {
    u8 _00[0x10];
    u16 mRemainFrames;   // 0x10: frames the generator still runs; 0: never ends.
    u8 _12[0x03];
    u8 mKillParam;       // 0x15: byte psKillFamily copies into the particles.
    u8 _16;
    u8 mLoop;            // 0x17: generator restarts when its frames run out.
    u16 mFamilyID;       // 0x18: particle family the generator spawns.
    u8 _1A[0x06];
    GSvec mPosition;     // 0x20: base position of every particle it spawns.
    u8 _2C[0x5C];
    _GScamera *mCamera;  // 0x88: camera the generator is interpreted with.
    u16 mOptionFlags;    // 0x8C: bits the psSet*Scaling setters toggle.
    u16 mIndex;          // 0x8E: index psCreateGeneratorID was called with.
    GSvec mRotation;     // 0x90: Euler radians.
    GSvec mScale;        // 0x9C
    u8 _A8[0x10];
};

/* Bank of the engine's emitter half (the peBank* units). peBankCreate
 * allocates the 0x168-byte object; a bank owns one particle file and drives
 * its own generator. The GS layer gives one to every slot of a bank file that
 * preallocates its generators. */
struct _peBank {
    u8 _00[0x08];
    _psGenerator *mGenerator;  // 0x08: engine generator the bank drives.
    u8 _0C[0x100];
    GSvec mPosition;           // 0x10C: transform the layer sets.
    f32 mRotation[4];          // 0x118: quaternion (Euler stored as a quat here).
    GSvec mScale;              // 0x128
    u32 mFlags;                // 0x134: bit 0x4 is the bank's visibility.
    u8 _138[0x28];
    _GScamera *mCamera;        // 0x160
    u8 _164[0x04];
};

extern "C" {

/* --- emitter half --- */
/* The two file entry points take the packed particle file itself: the loader
 * converts it into engine data and hands the file back as the loaded handle. */
_peBank *peBankCreate(void *fileData);
void *peBankLoadFile(void *fileData);
void peBankFreeFile(void *fileData);
void peBankFree(_peBank *bank);
void peBankStart(_peBank *bank);
void peBankStop(_peBank *bank);
void peBankStopLoop(_peBank *bank);
bool peBankIsInfinite(_peBank *bank);
f32 peBankGetLife(_peBank *bank);

/* --- particle-system half --- */
void psInitParticle(u32 count);
void psInitGenerator(u32 count);
void psInitAppSRT(u32 count, u32 size);
void psInitDataBank(u32 bankID, void *dataBank, void *particleData,
                    void *table, u32 flags);
_psGenerator *psCreateGeneratorID(u32 flag, u32 bankID, u32 index);
void psKillGenerator(_psGenerator *gen);
void psKillFamily(u16 familyID, u8 killParam);
u32 psGetGeneratorChildMaxLife(_psGenerator *gen);
u32 psGetParticleChildCount(_psGenerator *gen);
void psGeneratorSetCamera(_psGenerator *gen, _GScamera *camera);
void psSetParticleVisibility(_psGenerator *gen, u32 visible);
void psSetRandomVelocityScaling(_psGenerator *gen, u32 enable);
void psSetNodeScaling(_psGenerator *gen, u32 enable);
void psSetTornadoScaling(_psGenerator *gen, u32 a, u32 b);
void psSetParticleTexScaling(_psGenerator *gen, u32 enable);
void psSetOffsetRotationInLocal(_psGenerator *gen, u32 a, u32 b);
void psSetVelocityRotationInLocal(_psGenerator *gen, u32 enable);
void psSetGeneratorAngleRadiusScale(_psGenerator *gen, f32 *scale, u32 flag);
void psLinkChildGensToJObj(_psGenerator *gen, void *jobj);
void psUnlinkChildGensFromJObj(_psGenerator *gen);
void psSetBillboardCamera(_HSD_CObj *cobj);
void psInterpretParticles(void *dataBank);
void psExecGenerator(u32 familyMask);
void psDispBeginFrame(void);
s32 psDispHasRenderData(u32 group);
/* Display group of the particles a layer draws: the engine sorts the
 * particles of a group, the layer draws one group per call. */
enum GSParticleDispGroup {
    GS_PARTICLE_DISP_GROUP_0 = 0x1,
    GS_PARTICLE_DISP_GROUP_1 = 0x2,
};
/* Which of the two lists a group is sorted into: the particles the particle
 * file marks for Z drawing, and the ones it draws without. */
enum GSParticleDispMode {
    GS_PARTICLE_DISP_MODE_Z = 1,
    GS_PARTICLE_DISP_MODE_PLAIN = 2,
};
void psDispSetProjection(void *projection);
void psDispParticles(u32 group, u32 mode, _GScamera *camera);

/* --- frame/engine entry points --- */
void particleEngineInit(void);
void particleEngineUpdate(f32 delta);
void particleEnginePostUpdate(void);
void particleEngineRender(GSgfxLayer *layer);

}  // extern "C"

#endif
