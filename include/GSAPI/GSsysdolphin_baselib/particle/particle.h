#ifndef GS_PARTICLE_PARTICLE_H
#define GS_PARTICLE_PARTICLE_H

#include <types.h>
#include <GSvec.h>
typedef struct GSvec GSvec;

#ifdef __cplusplus
#include <GSgfx.h>
#include <GScamera.h>
#endif
struct _GScamera;
typedef struct _GScamera _GScamera;
struct _HSD_CObj;
typedef struct _HSD_CObj _HSD_CObj;
struct _HSD_JObj;
typedef struct _HSD_JObj _HSD_JObj;

/* Generator records shared by the particle-system modules. */
typedef struct _psGenerator _psGenerator;
struct _psGenerator {
    u8 _00[16];
    u16 mRemainFrames;   // 0x10: frames the generator still runs; 0: never ends.
    u8 _12[3];
    u8 mListGroup;       // 0x15: particle-list group.
    u8 _16;
    u8 mLoop;            // 0x17: generator restarts when its frames run out.
    u16 mFamilyID;       // 0x18: particle family the generator spawns.
    u8 _1A[6];
    GSvec mPosition;     // 0x20: base position of every particle it spawns.
    u8 _2C[92];
    _GScamera *mCamera;  // 0x88: camera the generator is interpreted with.
    u16 mOptionFlags;    // 0x8C: bits the psSet*Scaling setters toggle.
    u16 mIndex;          // 0x8E: index psCreateGeneratorID was called with.
    GSvec mRotation;     // 0x90: Euler radians.
    GSvec mScale;        // 0x9C
    u8 _A8[16];
};

#ifdef __cplusplus
extern "C" {
#endif

void psInitParticle(u32 count);
void psInitGenerator(u32 count);
int psInitAppSRT(int count, u32 size);
void psInitDataBank(int bankID, void *dataBank, void *particleData,
                    void *table, void *formData);
_psGenerator *psCreateGeneratorID(int listGroup, int bankID, int index);
void psKillGenerator(_psGenerator *gen);
void psKillFamily(u16 familyID, int listGroup);
u32 psGetGeneratorChildMaxLife(_psGenerator *gen);
u32 psGetParticleChildCount(_psGenerator *gen);
_GScamera *psGeneratorGetCamera(_psGenerator *generator);
void psGeneratorSetCamera(_psGenerator *gen, _GScamera *camera);
void psSetParticleVisibility(_psGenerator *gen, u8 visible);
void psSetRandomVelocityScaling(_psGenerator *gen, u8 enable);
void psSetNodeScaling(_psGenerator *gen, u8 enable);
void psSetTornadoScaling(_psGenerator *gen, u8 enable, u8 mode);
void psSetParticleTexScaling(_psGenerator *gen, u8 enable);
void psSetOffsetRotationInLocal(_psGenerator *gen, u8 enable, u8 mode);
void psSetVelocityRotationInLocal(_psGenerator *gen, u8 enable);
void psSetGeneratorAngleRadiusScale(_psGenerator *gen, f32 *scale, u8 flag);
void psLinkChildGensToJObj(_psGenerator *gen, _HSD_JObj *jobj);
void psUnlinkChildGensFromJObj(_psGenerator *gen);
void psSetBillboardCamera(_HSD_CObj *cobj);
void psInterpretParticles(u32 listGroupMask);
void psExecGenerator(u32 listGroupMask);
void psDispBeginFrame(void);
s32 psDispHasRenderData(u32 group);

enum GSParticleDispGroup {
    GS_PARTICLE_DISP_GROUP_0 = 1,
    GS_PARTICLE_DISP_GROUP_1 = 2,
};
enum GSParticleDispMode {
    GS_PARTICLE_DISP_MODE_Z = 1,
    GS_PARTICLE_DISP_MODE_PLAIN = 2,
};
void psDispSetProjection(void *projection);
void psDispParticles(u32 group, u32 mode, _GScamera *camera);
void psSetToken(u16 token);
void psSetPointJObjNodup(_HSD_JObj *jobj, int id);

#ifdef __cplusplus
}
#endif

#endif
