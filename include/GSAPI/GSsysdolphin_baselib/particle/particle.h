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

enum {
    PS_NUM_LINK = 16,
    PS_Z_WRITE_SHIFT = 3,
    PS_SORT_ORDER_SHIFT = 25,
    PS_SORT_ORDER_COUNT = 8
};

enum psBankCount { PS_NUM_BANK = 64 };
#define PS_FLAG_HIDDEN 0x20000000U
#define PS_EMISSION_SHAPE_MASK 0xF
#define PS_FLAG_Z_WRITE 0x00000008U
#define PS_SORT_ORDER_MASK 0x0E000000U

struct _psAppSRT;

typedef struct _psParticle _psParticle;
struct _psParticle {
    _psParticle *next;
    u32 flags;
    u8 _08[21];
    u8 linkNo;
    u16 familyID;
    u8 _20[112];
    struct _psAppSRT *appSRT;
};

enum psEmissionShape {
    PS_EMIT_RADIAL = 0,
    PS_EMIT_LINE = 1,
    PS_EMIT_TORNADO = 2,
    PS_EMIT_DISK_RADIAL_SPEED = 3,
    PS_EMIT_DISK = 4,
    PS_EMIT_BOX = 5,
    PS_EMIT_CONE = 6,
    PS_EMIT_CYLINDER = 7,
    PS_EMIT_SPHERE = 8
};

typedef struct _psCommandList {
    u8 _00[32];
    f32 radius;
    u8 _24[12];
    f32 emissionParameters[3];
    u8 commands[1];
} _psCommandList;

typedef union _psEmission {
    struct {
        f32 azimuthStart;
        f32 azimuthEnd;
        f32 height;
    } radial;
    GSvec line;
    f32 axialSpeed;
    struct {
        GSvec dimensions;
        f32 basis[3][3];
    } box;
    struct {
        f32 radialVelocity;
        f32 velocityElevation;
        f32 polarRange;
        f32 velocityAzimuth;
        f32 angle;
    } sphere;
} _psEmission;

/* Generator records shared by the particle-system modules. */
typedef struct _psGenerator _psGenerator;
struct _psGenerator {
    _psGenerator *next;
    u32 flags;
    u8 _08[8];
    u16 mRemainFrames;   // 0x10: frames the generator still runs; 0: never ends.
    u16 mType;
    u8 mBankID;
    u8 mListGroup;       // 0x15: particle-list group.
    u8 _16;
    u8 mLoop;            // 0x17: generator restarts when its frames run out.
    u16 mFamilyID;       // 0x18: particle family the generator spawns.
    u8 _1A[6];
    GSvec mPosition;     // 0x20: base position of every particle it spawns.
    u8 _2C[12];
    f32 mGravity;
    f32 mVelocityMultiplier;
    u8 _40[4];
    f32 mEmissionRadius;
    u8 _48[8];
    struct _psAppSRT *mAppSRT;
    _psEmission mEmission;
    u8 _84[4];
    _GScamera *mCamera;  // 0x88: camera the generator is interpreted with.
    u16 mOptionFlags;    // 0x8C: bits the psSet*Scaling setters toggle.
    u16 mIndex;          // 0x8E: index psCreateGeneratorID was called with.
    GSvec mRotation;     // 0x90: Euler radians.
    GSvec mScale;        // 0x9C
    _HSD_JObj *mJObj;    // 0xA8: joint the linked child generators follow.
    GSvec mEmissionScale;
};

#ifdef __cplusplus
extern "C" {
#endif

extern _psGenerator *activeGenerator;
extern _psCommandList **psCmdListArray[PS_NUM_BANK];
extern u32 psNumTexGroup[PS_NUM_BANK];

extern u16 g_ps_token;
extern u16 numPeakPSAppSRT;
extern u16 numPeakGenerators;
extern u16 numPeakParticles;
extern u16 numActivePSAppSRT;
extern u16 numActiveGenerators;
extern u16 numActiveParticles;

_psParticle *particleSort(int linkNo, _psParticle **zWriteHead, _psParticle **xluHead);
_psParticle *_psListGetFirst(int linkNo);
void _psListDelete(_psParticle *pp, _psParticle *parent);
_psParticle *_psListNew(_psParticle *parent, int linkNo);
void _psListClear(void);
int _psLinkInit(int count);

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
void psSetGeneratorAngleRadiusScale(_psGenerator *gen, f32 *scale, u8 scaleDynamics);
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
