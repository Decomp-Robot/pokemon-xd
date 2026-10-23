#include <peBankData.h>
#include <HSDArchive.h>
#include <dolphin/os/OSFastCast.h>

extern "C" {
void *memset(void *destination, int value, size_t size);
u32 GSmemGetSize(void *allocation);
void GSboundCreate(GSbound *bound, void *vertices);
void GSboundFree(GSbound *bound);
void GSboundAlwaysInView(GSbound *bound, s32 alwaysInView);
s32 GSboundIsInViewFast(GSbound *bound);
bool GSgfxTokenCleared(u16 token);
void GSgfxTokenWaitForClear(u16 token);
u8 GSgfxVideoGetVsyncRate();
u16 shapeGenerateIndex();
bool shapeLoad(u16 shape, void *data);
u32 shapeGetUsedMemorySize(u16 shape);
void shapeRelease(u16 shape);
f32 HSD_Randf();
bool particleEnginePrepareRender(_GScamera *camera);
bool particleEnginePrepareUpdate(_GScamera *camera);
}
void GSboundDraw(GSbound *bound, GSgfxLayer *layer);

extern "C" GStexture *GStextureLoad(void *data);

extern GSmtx *peViewMatrix;

enum peBankStorage {
    PE_BANK_RENDER_PASS_SHIFT = 16,
    PE_BANK_BOUND_DRAW_LAYER = 4,
    PE_BANK_EMIT_PARAM_BLOCK_SIZE = 10,
    PE_BANK_PARTICLE_LIMIT = 1000,
};

_peBank *peCurrentBank;
GSbound *peCurrentBound;
u16 peCurrentRenderToken;
_EmitParams *peEmitParams;
u16 peEmitParamsCount;
_peBank *peEmitParamsOwner;

extern "C" bool peBankIsInfinite(_peBank *bank) {
    if (bank == 0) return false;
    if (!(bank->mFlags & PE_BANK_ACTIVE)) return false;
    return bank->mRoot.mEmitter->mFlags & PE_EMITTER_LOOP;
}

extern "C" f32 peBankGetLife(_peBank *bank) {
    if (bank == 0) return 0.0f;
    if (!(bank->mFlags & PE_BANK_ACTIVE)) return 0.0f;
    return bank->mRoot.mRemainingLife;
}

extern "C" void peBankStopLoop(_peBank *bank) {
    if (peBankIsInfinite(bank)) bank->mRoot.SetLastLoop();
}

extern "C" void peBankStop(_peBank *bank) {
    if (bank == 0) return;
    if (!(bank->mFlags & PE_BANK_ACTIVE)) return;
    peCurrentBank = bank;
    peCurrentBound = bank->mFlags & PE_BANK_HAS_BOUND ? &bank->mBound : 0;
    bank->mRoot.Stop(bank->mData->mEmitter);
    bank->mFlags = 0;
}

static inline GSgfxLayerID peGetLayerID(GSgfxLayer *layer) {
    return layer->mLayerID;
}

extern "C" void peBankRender(_peBank *bank) {
    u32 layerMask;
    if (bank == 0) return;
    if (!(bank->mFlags & PE_BANK_ACTIVE)) return;
    if (!(bank->mFlags & PE_BANK_VISIBLE)) return;
    if (!(bank->mFlags & PE_BANK_IN_VIEW)) return;
    layerMask = 1 << peGetLayerID(peCurrentLayer);
    if (peCurrentPass == GSGFX_LAYER_PASS_XLU) layerMask <<= PE_BANK_RENDER_PASS_SHIFT;
    if (bank->mRenderedLayers & layerMask) return;
    peCurrentBank = bank;
    peCurrentBound = bank->mFlags & PE_BANK_HAS_BOUND ? &bank->mBound : 0;
    if (!particleEnginePrepareRender(bank->mCamera)) return;
    bank->mRenderedLayers |= layerMask;
    peCurrentLayer->mMatrixStack->push(peViewMatrix);
    peCurrentRenderToken = 0;
    bank->mRoot.Render(bank->mData->mEmitter);
    if (peCurrentRenderToken) bank->mRenderToken = peCurrentRenderToken;
    if ((bank->mFlags & PE_BANK_HAS_BOUND) &&
        (bank->mFlags & PE_BANK_DRAW_BOUND) && peCurrentLayer->mLayerID == PE_BANK_BOUND_DRAW_LAYER &&
        peCurrentPass == GSGFX_LAYER_PASS_XLU) {
        GSboundDraw(&bank->mBound, peCurrentLayer);
    }
    peCurrentLayer->mMatrixStack->pop();
}

extern "C" void peBankUpdate(_peBank *bank, f32 delta) {
    if (bank == 0) return;
    if (!(bank->mFlags & PE_BANK_ACTIVE)) return;
    if (bank->mFlags & PE_BANK_PAUSED) return;
    if (bank->mRenderToken && GSgfxTokenCleared(bank->mRenderToken)) bank->mRenderToken = 0;
    if (!particleEnginePrepareUpdate(bank->mCamera)) return;
    peCurrentBank = bank;
    if (bank->mFlags & PE_BANK_HAS_BOUND) {
        peCurrentBound = &bank->mBound;
        GSboundReset(peCurrentBound);
        GSboundAlwaysInView(peCurrentBound, 0);
    } else {
        peCurrentBound = 0;
    }
    if ((f32)bank->mRoot.Update(0, bank->mData->mEmitter, delta) <= 0.0f) {
        bank->mRoot.Stop(bank->mData->mEmitter);
        bank->mFlags = 0;
    } else if (!peCurrentBound || GSboundIsInViewFast(peCurrentBound)) {
        bank->mFlags |= PE_BANK_IN_VIEW;
    } else {
        bank->mFlags &= ~PE_BANK_IN_VIEW;
    }
}

extern "C" bool peBankStart(_peBank *bank) {
    static _EmitParams params;
    if (bank == 0) return false;
    params.mLife = 1.0f;
    peBankStop(bank);
    peCurrentBank = bank;
    peCurrentBound = 0;
    _peEmitter *emitter = bank->mData->mEmitter;
    bank->mFlags = PE_BANK_ACTIVE | PE_BANK_VISIBLE;
    if (emitter->mFlags & PE_EMITTER_USE_BOUND) {
        bank->mFlags |= PE_BANK_HAS_BOUND;
        peCurrentBound = &bank->mBound;
        GSboundReset(peCurrentBound);
        GSboundAlwaysInView(peCurrentBound, 0);
    }
    bank->mRoot.Start(0, bank->mData->mEmitter, &params);
    bank->mRenderToken = 0;
    return true;
}

extern "C" void peBankFree(_peBank *bank) {
    if (bank == 0) return;
    peBankStop(bank);
    if (bank->mRenderToken) {
        GSgfxTokenWaitForClear(bank->mRenderToken);
        bank->mRenderToken = 0;
    }
    bank->mRoot.Release();
    bank->mFile->mBankCount--;
    GSboundFree(&bank->mBound);
    if (bank && peEmitParamsCount && bank == peEmitParamsOwner) {
        peMemory.mTotal -= peMemory.mEmitParams;
        peMemory.mEmitParams = 0;
        delete[] peEmitParams;
        peEmitParams = 0;
        peEmitParamsOwner = 0;
        peEmitParamsCount = 0;
        for (_peBank *item = peBanks; item; item = item->mNext) {
            if (item == bank) continue;
            u16 maximum = item->mMaxParticles;
            if (maximum > peEmitParamsCount) {
                peEmitParamsCount = (maximum + PE_BANK_EMIT_PARAM_BLOCK_SIZE - 1) & ~(PE_BANK_EMIT_PARAM_BLOCK_SIZE - 1);
                peEmitParamsOwner = item;
            }
        }
        if (peEmitParamsCount) {
            peEmitParams = new _EmitParams[peEmitParamsCount];
            peMemory.mEmitParams += peEmitParamsCount * sizeof(_EmitParams);
            peMemory.mTotal += peEmitParamsCount * sizeof(_EmitParams);
        }
    }
    _peBank *previous = 0;
    _peBank *item = peBanks;
    while (item) {
        if (item == bank) break;
        previous = item;
        item = item->mNext;
    }
    if (previous) previous->mNext = item->mNext;
    else peBanks = item->mNext;
    item->mNext = 0;
    u32 size = GSmemGetSize(bank);
    peMemory.mBankData -= size;
    peMemory.mTotal -= size;
    if (bank) delete bank;
}

inline _EmitParams::_EmitParams();

extern "C" _peBank *peBankCreate(void *fileData) {
    _gpdHeader *file = (_gpdHeader *)fileData;
    if (file == 0) return 0;
    _peBank *bank = new _peBank;
    if (bank == 0) return 0;
    u32 size = GSmemGetSize(bank);
    peMemory.mBankData += size;
    peMemory.mTotal += size;
    GSboundCreate(&bank->mBound, 0);
    char *bankData = (char *)(file + 1);
    bankData += file->mResourceCount * sizeof(_gpdResourceItem);
    _particleData *data = (_particleData *)bankData;
    bank->mFile = file;
    file->mBankCount++;
    memset(&bank->mRoot, 0, sizeof(peEmitterNode));
    bank->mData = data;
    bank->mPosition.x = 0.0f;
    bank->mPosition.y = 0.0f;
    bank->mPosition.z = 0.0f;
    bank->mRotation[0] = 0.0f;
    bank->mRotation[1] = 0.0f;
    bank->mRotation[2] = 0.0f;
    bank->mRotation[3] = 1.0f;
    bank->mScale.x = 1.0f;
    bank->mScale.y = 1.0f;
    bank->mScale.z = 1.0f;
    bank->mFlags = 0;
    bank->mRenderedLayers = 0;
    bank->mRenderToken = 0;
    bank->mCamera = 0;
    _peBank *item = peBanks;
    if (item) {
        while (item->mNext) item = item->mNext;
        item->mNext = bank;
    } else peBanks = bank;
    bank->mNext = 0;
    peCurrentBank = bank;
    bank->mRoot.mTransform = (peSRT *)&bank->mPosition;
    bank->mRoot.Init(data->mEmitter);
    bank->mMaxParticles = _peBankCalculateMaxParticles(data, 4.0f / GSgfxVideoGetVsyncRate());
    if (bank && bank->mMaxParticles > peEmitParamsCount) {
        if (peEmitParams) {
            peMemory.mTotal -= peMemory.mEmitParams;
            peMemory.mEmitParams = 0;
            delete[] peEmitParams;
        }
        u16 maximum = bank->mMaxParticles;
        peEmitParamsCount = ((maximum + PE_BANK_EMIT_PARAM_BLOCK_SIZE - 1) / PE_BANK_EMIT_PARAM_BLOCK_SIZE) * PE_BANK_EMIT_PARAM_BLOCK_SIZE;
        peEmitParamsOwner = bank;
        peEmitParams = new _EmitParams[peEmitParamsCount];
        peMemory.mEmitParams += peEmitParamsCount * sizeof(_EmitParams);
        peMemory.mTotal += peEmitParamsCount * sizeof(_EmitParams);
    }
    return bank;
}

extern "C" void peBankFreeFile(void *fileData) {
    GStexture **textures;
    u32 index;
    u32 textureCount;
    u16 shape;
    u32 size;
    char *bankData;
    _gpdResourceItem *resource;
    _gpdHeader *file;
    u32 resourceCount;
    file = (_gpdHeader *)fileData;
    if (file->mSignature != GPD_FILE_SIGNATURE || file->mVersion != GPD_FILE_VERSION) return;
    if (file->mBankCount) return;
    bankData = (char *)(file + 1);
    bankData += file->mResourceCount * sizeof(_gpdResourceItem);
    _peFreeInternalBankData((_particleData *)bankData);
    resource = (_gpdResourceItem *)(file + 1);
    resourceCount = file->mResourceCount;
    while (resourceCount) {
        if ((resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED) {
            switch (resource->mKind) {
            case GPD_RESOURCE_TEXTURE:
                GStextureFree(resource->mTexture);
                break;
            case GPD_RESOURCE_SHAPE:
                shape = resource->mShapeHandle - 1;
                size = shapeGetUsedMemorySize(shape);
                peMemory.mShapes -= size;
                peMemory.mTotal -= size;
                shapeRelease(shape);
                break;
            case GPD_RESOURCE_TEXTURE_LIST:
                textureCount = *(u32 *)resource->mData;
                textures = (GStexture **)((u32 *)resource->mData + 1);
                for (index = 0; index < textureCount; index++, textures++) {
                    if (*textures) GStextureFree(*textures);
                }
                break;
            }
        }
        resource++;
        resourceCount--;
    }
}

extern "C" void *peBankLoadFile(void *fileData) {
    char *data = (char *)fileData;
    _gpdHeader *file = (_gpdHeader *)data;
    const char *base = data;
    data += sizeof(_gpdHeader);
    _gpdResourceItem *resources = (_gpdResourceItem *)data;
    data += file->mResourceCount * sizeof(_gpdResourceItem);
    if (_peConvertFileData(file, resources, (_particleData *)data, base)) return file;
    return 0;
}

static inline f32 peRandomFloat() {
    return HSD_Randf();
}

static inline u16 peFloatToU16(f32 value) {
    u16 result;
    OSf32tou16(&value, &result);
    return result;
}

u16 _peBankCalculateMaxParticles(_particleData *data, f32 frameScale) {
    if (data->mKind != PE_DATA_EMITTER) return 0;
    _peEmitter *emitter = data->mEmitter;
    f32 random = peRandomFloat();
    f32 count = emitter->mParticleCount + emitter->mParticleCountRandom * random;
    u16 maximum;
    if (emitter->mEmitInterval <= 0.00001f) maximum = peFloatToU16(0.5f + count);
    else {
        count *= frameScale;
        maximum = peFloatToU16(0.5f + count);
    }
    if (maximum == 0) maximum = 1;
    for (_particleData *child = emitter->mChildren; child; child = child->mNext) {
        u16 childMaximum = _peBankCalculateMaxParticles(child, frameScale);
        if (childMaximum > maximum) maximum = childMaximum;
    }
    if (maximum > PE_BANK_PARTICLE_LIMIT) maximum = PE_BANK_PARTICLE_LIMIT;
    return maximum;
}

void _peFreeInternalBankData(_particleData *data) {
    HSD_JObj *joint;
    void *contents = data->mData;
    if (!contents) return;
    switch (data->mKind) {
    case PE_DATA_EMITTER: {
        _peEmitter *emitter = (_peEmitter *)contents;
        if (emitter) {
            _particleData *child = emitter->mChildren;
            while (child) {
                _peFreeInternalBankData(child);
                child = child->mNext;
            }
        }
        return;
    }
    case PE_DATA_MODEL: {
        _peModel *model = (_peModel *)contents;
        if (!model) return;
        if (model->mFlags & PE_MODEL_SHAPE_RESOURCE) return;
        joint = model->mJoint;
        if (joint) goto releaseModelJoint;
    }
    default:
        return;
    case PE_DATA_LIGHT:
        goto releaseLights;
    }

releaseModelJoint:
    HSD_JObjUnref(joint);
    return;
releaseLights:
    {
        _peLight *light = (_peLight *)contents;
        if (light->mDiffuseLight) {
            peMemory.mBankData -= sizeof(GSgfxLight);
            peMemory.mTotal -= sizeof(GSgfxLight);
            delete light->mDiffuseLight;
            light->mDiffuseLight = 0;
        }
        if (light->mSpecularLight) {
            peMemory.mBankData -= sizeof(GSgfxLight);
            peMemory.mTotal -= sizeof(GSgfxLight);
            delete light->mSpecularLight;
            light->mSpecularLight = 0;
        }
    }

}

static const char gpdSceneDataSymbol[] = "scene_data";

extern GSvec zeroVector;
extern GSvec zAxisVector;

static inline bool peBankIsFileOffset(const void *pointer) {
    return pointer && (u32)pointer < PE_BANK_RELOCATED_ADDRESS;
}

#define PE_BANK_RELOCATE_ADDRESS(pointer, base) (peBankIsFileOffset(pointer) ? (u32)(pointer) + (u32)(base) : (u32)(pointer))

bool _peConvertFileData(_gpdHeader *file, _gpdResourceItem *resources,
                        _particleData *data, const char *base) {
    if (file->mSignature != GPD_FILE_SIGNATURE || file->mVersion != GPD_FILE_VERSION)
        return false;
    if (file->mBankCount) return true;

    u32 resourceCount;
    _gpdResourceItem *resource;
    u32 dataCount;
    GStexture **texture;
    resourceCount = file->mResourceCount;
    dataCount = file->mParticleDataCount;
    resource = resources;
    while (resourceCount) {
        resource->mData = (void *)PE_BANK_RELOCATE_ADDRESS(resource->mData, base);
        resource->mLoadFlags &= ~GPD_RESOURCE_LOADED;
        switch (resource->mKind) {
        case GPD_RESOURCE_TEXTURE: {
            GStexture *texture = GStextureLoad(resource->mData);
            if (texture) resource->mLoadFlags |= GPD_RESOURCE_LOADED;
            resource->mTexture = texture;
            break;
        }
        case GPD_RESOURCE_SCENE: {
            HSD_Archive archive;
            HSD_ArchiveParse(&archive, resource->mData,
                             resource->mArchiveSizeBlocks * GPD_ARCHIVE_BLOCK_BYTES -
                             (resource->mLoadFlags & GPD_ARCHIVE_PADDING_MASK));
            void *scene = HSD_ArchiveGetPublicAddress(&archive, gpdSceneDataSymbol);
            if (scene) resource->mLoadFlags |= GPD_RESOURCE_LOADED;
            resource->mData = scene;
            break;
        }
        case GPD_RESOURCE_TEXTURE_LIST:
            if (resource->mData) {
                peTextureList *list = (peTextureList *)resource->mData;
                u32 count = list->mCount;
                texture = list->mTextures;
                for (u32 index = 0; index < count; ++index, ++texture) {
                    *texture = (GStexture *)PE_BANK_RELOCATE_ADDRESS(*texture, (const char *)resource->mData);
                    *texture = GStextureLoad(*texture);
                }
                resource->mLoadFlags |= GPD_RESOURCE_LOADED;
            }
            break;
        case GPD_RESOURCE_SHAPE: {
            u16 shape = shapeGenerateIndex() + 1;
            if (shape) {
                if (shapeLoad(shape - 1, resource->mData)) {
                    u32 size = shapeGetUsedMemorySize(shape - 1);
                    peMemory.mShapes += size;
                    peMemory.mTotal += size;
                    resource->mLoadFlags |= GPD_RESOURCE_LOADED;
                } else shape = 0;
            }
            resource->mShapeHandle = shape;
            break;
        }
        }
        ++resource;
        --resourceCount;
    }

    while (dataCount) {
        data->mData = (void *)PE_BANK_RELOCATE_ADDRESS(data->mData, base);
        data->mNext = (_particleData *)PE_BANK_RELOCATE_ADDRESS(data->mNext, base);
        void *contents = data->mData;
        if (!contents) {
            ++data;
            --dataCount;
            continue;
        }
        switch (data->mKind) {
        case PE_DATA_EMITTER: {
            _peEmitter *emitter = (_peEmitter *)contents;
            emitter->mChildren = (_particleData *)PE_BANK_RELOCATE_ADDRESS(emitter->mChildren, base);
            emitter->mParticleCountSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mParticleCountSpline, base);
            emitter->mColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mColorSpline, base);
            emitter->mAlphaSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mAlphaSpline, base);
            emitter->mAccelerationSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mAccelerationSplines[0], base);
            emitter->mAccelerationSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mAccelerationSplines[1], base);
            emitter->mAccelerationSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mAccelerationSplines[2], base);
            emitter->mPositionSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mPositionSplines[0], base);
            emitter->mPositionSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mPositionSplines[1], base);
            emitter->mPositionSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mPositionSplines[2], base);
            emitter->mRotationSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationSplines[0], base);
            emitter->mRotationSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationSplines[1], base);
            emitter->mRotationSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationSplines[2], base);
            emitter->mRotationVelocitySplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationVelocitySplines[0], base);
            emitter->mRotationVelocitySplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationVelocitySplines[1], base);
            emitter->mRotationVelocitySplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mRotationVelocitySplines[2], base);
            emitter->mScaleSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mScaleSplines[0], base);
            emitter->mScaleSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mScaleSplines[1], base);
            emitter->mScaleSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(emitter->mScaleSplines[2], base);
            break;
        }
        case PE_DATA_PARTICLE: {
            _peParticle *particle = (_peParticle *)contents;
            resourceCount = file->mResourceCount;
            _gpdResourceItem *resource = resources;
            while (resourceCount) {
                if (particle->mResourceID == resource->mResourceID) {
                    if (resource->mKind == GPD_RESOURCE_TEXTURE_LIST) {
                        particle->mFlags |= PE_PARTICLE_TEXTURE_ANIMATION;
                        particle->mTextureList = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            (peTextureList *)resource->mData : 0;
                    } else {
                        particle->mFlags &= ~PE_PARTICLE_TEXTURE_ANIMATION;
                        particle->mTexture = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            resource->mTexture : 0;
                    }
                    break;
                }
                ++resource;
                --resourceCount;
            }
            if (!resourceCount) particle->mTexture = 0;
            particle->mRotationSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mRotationSpline, base);
            particle->mWidthSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mWidthSpline, base);
            particle->mHeightSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mHeightSpline, base);
            particle->mColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mColorSpline, base);
            particle->mSecondColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mSecondColorSpline, base);
            if (!particle->mSecondColorSpline) particle->mFlags &= ~PE_PARTICLE_DUAL_COLOR;
            particle->mAlphaSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(particle->mAlphaSpline, base);
            if (!(particle->mFlags & PE_TEXTURE_COORDINATES_FIXED)) {
                f32 *coordinates = (f32 *)&particle->mTexCoordLeft;
                f32 left = coordinates[0];
                f32 top = coordinates[1];
                particle->mTexCoordLeft = PE_TEXTURE_COORDINATE_SCALE * left;
                particle->mTexCoordTop = PE_TEXTURE_COORDINATE_SCALE * top;
                f32 right = coordinates[2];
                f32 bottom = coordinates[3];
                particle->mTexCoordRight = PE_TEXTURE_COORDINATE_SCALE * right;
                particle->mTexCoordBottom = PE_TEXTURE_COORDINATE_SCALE * bottom;
                particle->mOrientationY = particle->mOrientationZ = 0.0f;
                particle->mFlags |= PE_TEXTURE_COORDINATES_FIXED;
            }
            break;
        }
        case PE_DATA_PARTICLE_STRIP: {
            _peParticleStrip *strip = (_peParticleStrip *)contents;
            resourceCount = file->mResourceCount;
            _gpdResourceItem *resource = resources;
            while (resourceCount) {
                if (strip->mResourceID == resource->mResourceID) {
                    if (resource->mKind == GPD_RESOURCE_TEXTURE_LIST) {
                        strip->mFlags |= PE_STRIP_TEXTURE_LIST;
                        if (strip->mFlags & PE_STRIP_DUAL_COLOR) strip->mFlags &= ~PE_STRIP_SECOND_COLOR_SPLINE;
                        strip->mTextureList = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            (peTextureList *)resource->mData : 0;
                    } else {
                        strip->mFlags &= ~PE_STRIP_TEXTURE_LIST;
                        strip->mTexture = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            resource->mTexture : 0;
                    }
                    break;
                }
                ++resource;
                --resourceCount;
            }
            if (!resourceCount) strip->mTexture = 0;
            strip->mColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(strip->mColorSpline, base);
            strip->mSecondColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(strip->mSecondColorSpline, base);
            if (!strip->mSecondColorSpline) strip->mFlags &= ~PE_STRIP_SECOND_COLOR_SPLINE;
            strip->mAlphaSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(strip->mAlphaSpline, base);
            break;
        }
        case PE_DATA_DISTORTION: {
            _peDistortion *distortion = (_peDistortion *)contents;
            resourceCount = file->mResourceCount;
            _gpdResourceItem *resource = resources;
            while (resourceCount) {
                if (distortion->mResourceID == resource->mResourceID) {
                    distortion->mTexture = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                        resource->mTexture : 0;
                    break;
                }
                ++resource;
                --resourceCount;
            }
            if (!resourceCount) distortion->mTexture = 0;
            distortion->mRotationSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(distortion->mRotationSpline, base);
            distortion->mWidthSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(distortion->mWidthSpline, base);
            distortion->mHeightSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(distortion->mHeightSpline, base);
            distortion->mColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(distortion->mColorSpline, base);
            break;
        }
        case PE_DATA_MODEL: {
            _peModel *model = (_peModel *)contents;
            resourceCount = file->mResourceCount;
            _gpdResourceItem *resource = resources;
            while (resourceCount) {
                if (model->mResourceID == resource->mResourceID) {
                    if ((model->mFlags & PE_MODEL_SHAPE_RESOURCE) && resource->mKind == GPD_RESOURCE_SHAPE) {
                        model->mShapeHandle = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            resource->mShapeHandle : 0;
                    } else if (resource->mKind == GPD_RESOURCE_SCENE) {
                        HSD_SceneDesc *scene = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                            (HSD_SceneDesc *)resource->mData : 0;
                        if (scene) {
                            HSD_SceneModel *sceneModel = *scene->mModels;
                            HSD_JObj *joint = HSD_JObjLoadJoint(sceneModel->mJoint);
                            if (joint) {
                                model->mJoint = joint;
                                model->mAnimationLength = model->mAnimationFrame = 0.0f;
                                HSD_AnimJoint *jointAnimation = 0;
                                HSD_MatAnimJoint *materialAnimation = 0;
                                HSD_ShapeAnimJoint *shapeAnimation = 0;
                                if (sceneModel->mJointAnimations) jointAnimation = *sceneModel->mJointAnimations;
                                if (sceneModel->mMaterialAnimations) materialAnimation = *sceneModel->mMaterialAnimations;
                                if (sceneModel->mShapeAnimations) shapeAnimation = *sceneModel->mShapeAnimations;
                                HSD_JObjAddAnimAll(joint, jointAnimation, materialAnimation, shapeAnimation);
                                if (model->mFlags & PE_MODEL_LOOP)
                                    HSD_ForeachAnim(joint, HSD_ANIMATION_JOINT, HSD_ANIMATION_ALL,
                                        (void (*)())HSD_AObjSetFlags, HSD_ANIMATION_FLAGS_ARGUMENT, HSD_ANIMATION_LOOP);
                                else
                                    HSD_ForeachAnim(joint, HSD_ANIMATION_JOINT, HSD_ANIMATION_ALL,
                                        (void (*)())HSD_AObjClearFlags, HSD_ANIMATION_FLAGS_ARGUMENT, HSD_ANIMATION_LOOP);
                                HSD_ForeachAnim(joint, HSD_ANIMATION_JOINT, HSD_ANIMATION_ALL,
                                    (void (*)())HSD_AObjSetRate, HSD_ANIMATION_FLOAT_ARGUMENT, 0.0);
                                HSD_ForeachAnim(joint, HSD_ANIMATION_JOINT, HSD_ANIMATION_ALL,
                                    (void (*)())_modelGetEndFrame, HSD_ANIMATION_POINTER_ARGUMENT, &model->mAnimationLength);
                                HSD_JObjReqAnimAll(joint, 0.0f);
                                HSD_JObjAnimAll(joint);
                                model->mAnimationLength /= PE_ANIMATION_FRAMES_PER_SECOND;
                            } else model->mJoint = 0;
                        }
                    } else model->mJoint = 0;
                    break;
                }
                ++resource;
                --resourceCount;
            }
            if (!resourceCount) model->mJoint = 0;
            model->mScaleSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mScaleSplines[0], base);
            model->mScaleSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mScaleSplines[1], base);
            model->mScaleSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mScaleSplines[2], base);
            model->mRotationSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mRotationSplines[0], base);
            model->mRotationSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mRotationSplines[1], base);
            model->mRotationSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(model->mRotationSplines[2], base);
            break;
        }
        case PE_DATA_LIGHT: {
            _peLight *light = (_peLight *)contents;
            resourceCount = file->mResourceCount;
            _gpdResourceItem *resource = resources;
            while (resourceCount) {
                if (light->mResourceID == resource->mResourceID) {
                    light->mResource = (resource->mLoadFlags & GPD_RESOURCE_LOADED) == GPD_RESOURCE_LOADED ?
                        resource->mData : 0;
                    break;
                }
                ++resource;
                --resourceCount;
            }
            if (!resourceCount) light->mResource = 0;
            light->mColorSpline = (peColorSpline *)PE_BANK_RELOCATE_ADDRESS(light->mColorSpline, base);
            light->mAlphaSpline = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mAlphaSpline, base);
            light->mPositionSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mPositionSplines[0], base);
            light->mPositionSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mPositionSplines[1], base);
            light->mPositionSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mPositionSplines[2], base);
            light->mDirectionSplines[0] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mDirectionSplines[0], base);
            light->mDirectionSplines[1] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mDirectionSplines[1], base);
            light->mDirectionSplines[2] = (peSpline *)PE_BANK_RELOCATE_ADDRESS(light->mDirectionSplines[2], base);
            if (light->mFlags & PE_LIGHT_DIFFUSE) {
                GSgfxLight *diffuse = new GSgfxLight;
                if (diffuse) {
                    peMemory.mBankData += sizeof(GSgfxLight);
                    peMemory.mTotal += sizeof(GSgfxLight);
                    diffuse->MakeDiff(&light->mColor, &zeroVector, false);
                    light->mDiffuseLight = diffuse;
                    if (light->mFlags & PE_LIGHT_SPECULAR) {
                        GSgfxLight *specular = new GSgfxLight;
                        if (specular) {
                            peMemory.mBankData += sizeof(GSgfxLight);
                            peMemory.mTotal += sizeof(GSgfxLight);
                            specular->MakeSpec(&light->mColor, &zAxisVector, light->mShininess, 0);
                            light->mSpecularLight = specular;
                        } else light->mFlags &= ~PE_LIGHT_SPECULAR;
                    }
                } else light->mFlags &= ~(PE_LIGHT_DIFFUSE | PE_LIGHT_SPECULAR);
            }
            break;
        }
        }
        ++data;
        --dataCount;
    }
    return true;
}
