#ifndef PE_BANK_H
#define PE_BANK_H

#include <types.h>
#include <GSbound.h>
#include <peNodeBase.h>

struct _GScamera;
struct _peEmitter;
struct _particleData;
class peContainer;
class peNode;


class peEmitterNode : public peNode {
public:
    peEmitterNode() {
        mTransform = 0;
        mNext = 0;
        mPrevious = 0;
        mEmitter = 0;
        mChildren = 0;
    }
    void Init(_peEmitter *emitter);
    void Release();
    void Start(peEmitterNode *parent, _peEmitter *emitter, _EmitParams *params);
    bool Update(peEmitterNode *parent, _peEmitter *emitter, f32 delta);
    void Stop(_peEmitter *emitter);
    void Render(_peEmitter *emitter);
    void SetLastLoop();

    u8 mEmitterState[0xCC];
    _peEmitter *mEmitter;      // 0xFC
    peContainer *mChildren; // 0x100
};

enum ParticleDataKind {
    PE_DATA_EMITTER = 0,
    PE_DATA_PARTICLE = 1,
    PE_DATA_MODEL = 2,
    PE_DATA_DISTORTION = 3,
    PE_DATA_LIGHT = 4,
    PE_DATA_POSITION = 5,
    PE_DATA_PARTICLE_STRIP = 6,
};

struct _particleData {
    u32 mFlags;
    ParticleDataKind mKind;
    union {
        _peEmitter *mEmitter;
        void *mData;
    };
    _particleData *mNext;
};

struct peSpline;
struct peColorSpline;
struct _pePositionFunction;

struct _peEmitter {
    u32 mFlags;
    u32 mChildCount;
    f32 mLife;
    f32 mParticleCount;
    f32 mParticleCountRandom;
    peSpline *mParticleCountSpline;
    u8 mEmissionParameters[0x04];
    f32 mEmitInterval;
    u8 mParticleParameters[0x80];
    f32 mDrag;                 // 0xA0
    f32 mBounce;               // 0xA4
    f32 mAttraction;           // 0xA8
    GSvec mAccelerationParameters;
    peSpline *mAccelerationSplines[3];
    GSvec mPositionParameters;
    peSpline *mPositionSplines[3];
    GSvec mRotationParameters;
    peSpline *mRotationSplines[3];
    GSvec mRotationVelocityParameters;
    GSvec mRotationVelocityRandom;
    peSpline *mRotationVelocitySplines[3];
    GSvec mScaleParameters;
    peSpline *mScaleSplines[3];
    peColorSpline *mColorSpline;
    peSpline *mAlphaSpline;
    _particleData *mChildren;  // 0x138
};

struct _gpdHeader {
    u32 mSignature;
    u32 mVersion;
    u16 mParticleDataCount;
    u16 mResourceCount;
    u8 mBankCount;
    u8 mReserved[3];
};

enum GpdResourceKind {
    GPD_RESOURCE_TEXTURE = 0,
    GPD_RESOURCE_SCENE = 1,
    GPD_RESOURCE_TEXTURE_LIST = 2,
    GPD_RESOURCE_SHAPE = 3,
};

enum GpdFileFormat {
    GPD_FILE_SIGNATURE = 0x01F056DA,
    GPD_FILE_VERSION = 0x00030002,
};

enum GpdResourceLoadFlags {
    GPD_RESOURCE_LOADED = 0x40,
};

struct _gpdResourceItem {
    u8 mResourceID;
    u8 mLoadFlags;
    u16 mArchiveSizeBlocks;
    GpdResourceKind mKind;
    union {
        GStexture *mTexture;
        void *mData;
        u32 mShapeHandle;
    };
};

enum peBankFlags {
    PE_BANK_ACTIVE = 0x01,
    PE_BANK_PAUSED = 0x02,
    PE_BANK_VISIBLE = 0x04,
    PE_BANK_IN_VIEW = 0x08,
    PE_BANK_HAS_BOUND = 0x10,
    PE_BANK_DRAW_BOUND = 0x20,
};

enum peEmitterFlags {
    PE_EMITTER_LOOP = 0x01,
    PE_EMITTER_USE_BOUND = 0x20000,
};

struct _peBank {
    _peBank() {}

    _gpdHeader *mFile;         // 0x000
    peEmitterNode mRoot;      // 0x004
    _particleData *mData;      // 0x108
    GSvec mPosition;           // 0x10C
    f32 mRotation[4];          // 0x118
    GSvec mScale;              // 0x128
    u32 mFlags;                // 0x134
    u32 mRenderedLayers;       // 0x138
    u16 mRenderToken;          // 0x13C
    u16 mMaxParticles;         // 0x13E
    GSbound mBound;            // 0x140
    _GScamera *mCamera;        // 0x160
    _peBank *mNext;            // 0x164
};

struct peMemoryUsage {
    u32 mEmitParams;
    u32 mBankData;
    u32 mNodes;
    u32 mRenderData;
    u32 mShapes;
    u32 mTotal;
    u32 mReserved[3];
};

extern "C" {
_peBank *peBankCreate(void *fileData);
void *peBankLoadFile(void *fileData);
void peBankFreeFile(void *fileData);
void peBankFree(_peBank *bank);
bool peBankStart(_peBank *bank);
void peBankStop(_peBank *bank);
void peBankStopLoop(_peBank *bank);
bool peBankIsInfinite(_peBank *bank);
f32 peBankGetLife(_peBank *bank);
void peBankRender(_peBank *bank);
void peBankUpdate(_peBank *bank, f32 delta);
}

u16 _peBankCalculateMaxParticles(_particleData *data, f32 frameScale);
void _peFreeInternalBankData(_particleData *data);
bool _peConvertFileData(_gpdHeader *file, _gpdResourceItem *resources,
                        _particleData *data, const char *base);

extern _peBank *peCurrentBank;
extern GSbound *peCurrentBound;
extern u16 peCurrentRenderToken;
extern _EmitParams *peEmitParams;
extern u16 peEmitParamsCount;
extern _peBank *peEmitParamsOwner;
extern _peBank *peBanks;
extern peMemoryUsage peMemory;
extern GSgfxLayer *peCurrentLayer;
extern GSgfxLayerPass peCurrentPass;

#endif
