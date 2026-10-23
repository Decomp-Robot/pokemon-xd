#ifndef PE_BANK_H
#define PE_BANK_H

#include <GSgfx.h>
#include <GSbound.h>
#include <peEmitter.h>

struct _GScamera;
struct _peEmitter;
struct _particleData;
class peContainer;
class peNode;


enum ParticleDataKind {
    PE_DATA_EMITTER = 0,
    PE_DATA_PARTICLE = 1,
    PE_DATA_MODEL = 2,
    PE_DATA_DISTORTION = 3,
    PE_DATA_LIGHT = 4,
    PE_DATA_POSITION = 5,
    PE_DATA_PARTICLE_STRIP = 6,
};

class _EmitParams {
public:
    _EmitParams() { life = 0.0f; forceResponse = 0.0f; }
    f32 life;
    f32 forceResponse;
    GSvec position;
    GSvec velocity;
    peNodeTransform transform;
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

struct _peBank {
    _peBank() {}

    _gpdHeader *mFile;         // 0x000
    peEmitterNode mRoot;      // 0x004
    _particleData *mData;      // 0x108
    peNodeTransform transform; // 0x10C
    u32 mFlags;                // 0x134
    u32 mRenderedLayers;       // 0x138
    u16 mRenderToken;          // 0x13C
    u16 mMaxParticles;         // 0x13E
    GSbound mBound;            // 0x140
    _GScamera *mCamera;        // 0x160
    _peBank *mNext;            // 0x164
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

extern _peBank *g_currentBank;
extern GSbound *g_currentBound;
extern u16 g_bank_token;
extern _EmitParams *g_emitParams;
extern u16 g_maxEmitParams;
extern _peBank *g_emitParamsBank;
extern _peBank *g_particleBanks;
extern GSmtx *g_CameraMatrix;
extern GSgfxLayer *g_current_layer;
extern GSgfxLayerPass g_render_pass;

#endif
