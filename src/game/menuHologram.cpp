#include <types.h>
#include <CMenuGlobalState.h>
#include <CMetaMenu.h>
#include <GSmemory.h>
#include <GSmodel.h>
#include <GSthread.h>
#include <GSflag.h>
#include <GSmsg.h>
#include <GSlight.h>
#include <GSmaterial.h>
#include <GSgfx.h>
#include <dolphin/gx/GXEnum.h>
#include <GStexture.h>
#include <GSmtx.h>
#include <GSres.h>
#include <GSAPI/GSfsys/GSfsys.h>
#include <HSDArchive.h>
#include <scene.h>
#include <fightSeq.h>
#include <wazaSequence.h>
#include <sequence.h>
#include <scriptModel.h>
#include <menuHologram.h>
#include <menuReliveHall.h>
#include <menuPocketTool.h>
#include <pokemonRelive.h>
#include <relivehall.h>
#include <relglobal.h>
#include <pcbox.h>
#include <floor.h>
#include <camera.h>
#include <app/camera/CameraType.h>
#include <pokemonDB.h>
#include <window.h>
#include <winSprite.h>
#include <peopleInfoBios.h>
#include <menuModel.h>
#include <sound.h>
#include <new.h>

extern "C" void menuDrawReliveUpdate();

namespace {

class CMenuHologram : public CMetaMenu {
public:
    struct StageModel {
        unsigned long mResource;
        GSmodel *mModel;
        ModelSequence *mSequence;
    };
    enum ModelSlot {
        MODEL_SCRIPT_BASE = 0,
        MODEL_SCRIPT_PLATE = 1,
        MODEL_VISITOR = 2,
        MODEL_DANCER = 3,
        MODEL_QUANTITY = 7
    };
    enum ScriptAnimation {
        SCRIPT_ANIMATION_ENTER = 1,
        SCRIPT_ANIMATION_WAIT = 2,
        SCRIPT_ANIMATION_RELIVE = 3,
        SCRIPT_ANIMATION_AFTER_RELIVE = 4
    };
    enum State {
        STATE_START = 1,
        STATE_MAIN = 2,
        STATE_MENU = 3,
        STATE_RELIVE_CEREMONY = 4,
        STATE_GO_RELIVE_HALL = 5,
        STATE_EXIT_QUERY = 6,
        STATE_EXIT_CEREMONY = 7,
        STATE_EXIT = 8
    };
    void reloadDatas(bool kickBgm);
    void unloadDatas();
    void main(CMenuGlobalState &state);
    virtual void drawAll();
    void stateReliveCeremony();
    void stateMainProc();
    void setStage(int stage);
    void setStageToReliveReady();
    void moveStageWait(int stage);
    GSgfxColAu8 pokemonToAisyouColor(Pokemon *pokemon, Pokemon *partner);
    void bgmStop();
    void bgmKick();
    void stageAllModelLoad();
    void releaseOffscreenData();
    void renderOffscreenData();
    void initializeOffscreenData();
    void stageAllModelUnLoad();
    StageModel *modelPokemonSetScale(long index);
    void modelVisitorSetYPosition(long index);
    StageModel *modelPokemonLoad(long index);
    long actionYesNo(unsigned long messageID, bool waitMessage);
    void loop();
    void init(CMenuGlobalState &state);
    static GSgfxLayerID model_z_layer;
    inline void scriptModelLoad(long index, unsigned long resource)
    {
        memset(&mModels[index], 0, sizeof(StageModel));
        GSgfxLayerID layer[] = { GSGFX_LAYER_BASE };
        mModels[index].mResource = scriptModelOpen(resource);
        mModels[index].mModel = (GSmodel *)GSresGetResourceAllGroup(mModels[index].mResource);
        GSmodelSetRenderLayer(mModels[index].mModel, 1, layer);
        if (mModels[index].mModel != NULL) {
            GSmodelSetAnimIndex(mModels[index].mModel, SCRIPT_ANIMATION_ENTER);
            GSmodelSetAnimType(mModels[index].mModel, GS_MODEL_ANIM_ONCE);
            GSmodelStartAnimation(mModels[index].mModel);
        }
    }
private:
    long mReserved;
    long mStageIndex;
    long mSelectedStageIndex;
    CReliveStage *mStage;
    bool mStageTurning;
    GSlight *mLights[8];
    int mLightQuantity;
    GStexture *mOffscreenTexture;
    unsigned long mStageResource;
    bool mStageModelLoaded;
    bool mReliveCeremonyInProgress;
    GSmodel *mHeroModel;
    NullSequence *mSequenceStage;
    WazaSequence *mStageWazaSequence;
    ModelSequence *mRelivedModel;
    StageModel mModels[MODEL_QUANTITY];
    CInfoWindow mInfoWindow;
    CMenuGlobalState *mGlobalState;
    long mReservedEnd;
};

CMenuHologram *pMenuHologram;
void thisNew();
void thisFree();
void thisExec(CMenuGlobalState &state);
float _getNeckRotationCB(GSpart *, GSpartMixType, void *, GSvec *rotation);
_HSD_SceneDesc *_makeArchiveHeader(unsigned long resource);

inline long getModelAnimation(GSmodel *model)
{
    if (model == NULL) {
        return 0;
    }
    unsigned long animation;
    GSmodelGetAnimIndex(model, &animation, NULL);
    return animation;
}

inline void playModelAnimation(long animation, GSmodel *&model, GSmodelAnimType type)
{
    if (model != NULL) {
        GSmodelSetAnimIndex(model, animation);
        GSmodelSetAnimType(model, type);
        GSmodelStartAnimation(model);
    }
}

static const char sModelZLayerName[] = "model z layer";
static const char sSceneDataName[] = "scene_data";

enum {
    STAGE_QUANTITY = 9,
    BGM_HOLOGRAM_NO_TEMPO = 1467,
    BGM_HOLOGRAM_TEMPO_SLOW = 1296,
    BGM_HOLOGRAM_TEMPO_MIDDLE = 1295,
    BGM_HOLOGRAM_TEMPO_FAST = 1294,
    SE_STAGE_CHANGE = 1119,
    SE_MENU_DECIDE = 1114,
    SE_MENU_CANCEL = 1115,
    HOLOGRAM_RESTORE_BEFORE_LAYER = 4,
    HOLOGRAM_COMPOSITE_LAYER = 6,
    HOLOGRAM_SEQUENCE_NONE = 0,
    HOLOGRAM_SEQUENCE_DARK = 37,
    HOLOGRAM_SEQUENCE_DARK_HYPER_MODE = 135,
    HOLOGRAM_SEQUENCE_MAX_TEMPO = 239,
    HOLOGRAM_SEQUENCE_VISITOR = 240,
    HOLOGRAM_SEQUENCE_TEMPO = 243,
    HOLOGRAM_SEQUENCE_CATEGORY = 4,
    HOLOGRAM_PASS_ZERO_FIFO_BYTES = 57344,
    HOLOGRAM_PASS_ONE_FIFO_BYTES = 8192,
    HOLOGRAM_MATRIX_STACK_CAPACITY = 8,
    HOLOGRAM_NECK_MIX_MASK = 0x20202,
    HOLOGRAM_ARCHIVE_NO_TEMPO = 2276,
    HOLOGRAM_ARCHIVE_TEMPO = 2277,
    HOLOGRAM_ARCHIVE_MAX_TEMPO = 2278,
    HOLOGRAM_ARCHIVE_RELIVE_READY = 2279,
    HOLOGRAM_HERO_ANIMATION = 3,
    HOLOGRAM_LIGHT_ANIMATION = 0,
    HOLOGRAM_MENU_RELIVE_HALL = 0,
    HOLOGRAM_MENU_MOVE_STAGE = 1,
    HOLOGRAM_MENU_CANCEL = 2,
    MSG_RELIVE_HALL = 53556,
    MSG_MOVE_STAGE = 53557,
    MSG_CANCEL = 53558,
    MSG_EXIT_CONFIRM = 53596,
    MSG_YES = 17002,
    MSG_NO = 17003,
    WINDOW_HOLOGRAM_INFO = 208,
    WINDOW_HOLOGRAM_INFO_ITEM = 1498
};

enum HologramResource {
    HOLOGRAM_NO_TEMPO_BASE_MODEL = 0x18D91000,
    HOLOGRAM_NO_TEMPO_PLATE_MODEL = 0x18D91001,
    HOLOGRAM_NO_TEMPO_CAMERA = 0x18D71800,
    HOLOGRAM_NO_TEMPO_SCENE = 0x18D90400,
    HOLOGRAM_TEMPO_BASE_MODEL = 0x18DC1000,
    HOLOGRAM_TEMPO_CAMERA = 0x18DA1800,
    HOLOGRAM_TEMPO_SCENE = 0x18DC0400,
    HOLOGRAM_MAX_TEMPO_BASE_MODEL = 0x18DF1000,
    HOLOGRAM_MAX_TEMPO_PLATE_MODEL = 0x18DF1001,
    HOLOGRAM_MAX_TEMPO_CAMERA = 0x18DD1800,
    HOLOGRAM_MAX_TEMPO_SCENE = 0x18DF0400,
    HOLOGRAM_READY_BASE_MODEL = 0x18E41000,
    HOLOGRAM_READY_CAMERA = 0x18E01800,
    HOLOGRAM_READY_SCENE = 0x18E40400,
    HOLOGRAM_HERO_MODEL = 0x16C90400,
    HOLOGRAM_CEREMONY_CAMERA = 0x18E21800
};

static inline void hologramColor4u8(u8 r, u8 g, u8 b, u8 a)
{
    GSgfxFifo.u8 = r;
    GSgfxFifo.u8 = g;
    GSgfxFifo.u8 = b;
    GSgfxFifo.u8 = a;
}

static inline void hologramVertex2u16(u16 x, u16 y)
{
    GSgfxFifo.u16 = x;
    GSgfxFifo.u16 = y;
}

static inline void hologramTexCoord2u8(u8 s, u8 t)
{
    GSgfxFifo.u8 = s;
    GSgfxFifo.u8 = t;
}

long localWindowTable[] = { 113, 114, 136, WINDOW_HOLOGRAM_INFO, 135, 0 };

void CMenuHologram::main(CMenuGlobalState &state)
{
    systemFadeOutKeepAfterFloorPush();
    init(state);
    systemFadeInKick();
    loop();
    unloadDatas();
    bgmStop();
}

void CMenuHologram::init(CMenuGlobalState &state)
{
    CMetaMenu::init(pcReliveHallSoundTbl);
    mReliveCeremonyInProgress = false;
    mStageIndex = 0;
    mSelectedStageIndex = 0;
    mStage = NULL;
    mStageTurning = false;
    memset(mLights, 0, sizeof(mLights));
    mLightQuantity = 0;
    mOffscreenTexture = NULL;
    mStageModelLoaded = false;
    mSequenceStage = NULL;
    mStageWazaSequence = NULL;
    mRelivedModel = NULL;
    mGlobalState = &state;
    localWindowSetTable(localWindowTable);
    mInfoWindow.init(WINDOW_HOLOGRAM_INFO, WINDOW_HOLOGRAM_INFO_ITEM);
    setStageToReliveReady();
    reloadDatas(true);
}

void CMenuHologram::loop()
{
    static unsigned long msgTbl[] = { MSG_RELIVE_HALL, MSG_MOVE_STAGE, MSG_CANCEL };
    long *work = mState.mWork;
    setState(STATE_START, work);
    for (;;) {
        mState.update();
        switch (mState.mCurrent) {
        case STATE_START:
            if (mStage->isReliveReady()) {
                setState(STATE_RELIVE_CEREMONY, work);
            } else {
                setState(STATE_MAIN, work);
            }
            break;
        case STATE_MAIN:
            mStageTurning = true;
            stateMainProc();
            break;
        case STATE_MENU:
            switch (actionMenu(msgTbl, 3, 440, 120, 0)) {
            case HOLOGRAM_MENU_RELIVE_HALL:
                setState(STATE_GO_RELIVE_HALL, work);
                break;
            case HOLOGRAM_MENU_MOVE_STAGE:
                moveStageWait(mSelectedStageIndex);
                setState(STATE_MAIN, work);
                break;
            case HOLOGRAM_MENU_CANCEL:
                setState(STATE_MAIN, work);
                break;
            }
            break;
        case STATE_RELIVE_CEREMONY:
            stateReliveCeremony();
            break;
        case STATE_GO_RELIVE_HALL:
            relivehallSetLastViewStage(mSelectedStageIndex);
            systemFadeOutSetAndWait(-1.0f);
            {
                CMenuGlobalState *globalState = mGlobalState;
                globalState->mMode = globalState->mResult;
                globalState->mResult = 2;
            }
            return;
        case STATE_EXIT_QUERY:
            if (actionYesNo(MSG_EXIT_CONFIRM, false) == 0) {
                setState(STATE_MAIN, work);
            } else {
                setState(STATE_EXIT, work);
            }
            break;
        case STATE_EXIT_CEREMONY:
        case STATE_EXIT:
            relivehallSetLastViewStage(mStageIndex);
            systemFadeOutSetAndWait(-1.0f);
            {
                CMenuGlobalState *globalState = mGlobalState;
                globalState->mMode = globalState->mResult;
                globalState->mResult = 0;
            }
            return;
        }
    }
}

long plateIndexTbl[4][5][5] = {
    {
        { 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 2, 3, 0, 0, 0 },
        { 4, 5, 6, 0, 0 },
        { 7, 8, 9, 10, 0 },
        { 11, 12, 13, 14, 15 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 16, 17, 18, 19, 20 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 21, 22, 0, 0, 0 },
        { 23, 24, 25, 0, 0 },
        { 26, 27, 28, 29, 0 },
        { 30, 31, 32, 33, 34 },
    },
};
long colorLineIndexTbl[4][5][5] = {
    {
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 35, 36, 0, 0, 0 },
        { 37, 39, 38, 0, 0 },
        { 40, 43, 42, 41, 0 },
        { 44, 48, 47, 46, 45 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 49, 53, 52, 51, 50 },
    },
    {
        { 0, 0, 0, 0, 0 },
        { 54, 55, 0, 0, 0 },
        { 56, 58, 57, 0, 0 },
        { 59, 62, 61, 60, 0 },
        { 63, 67, 66, 65, 64 },
    },
};

long CMenuHologram::actionYesNo(unsigned long messageID, bool waitMessage)
{
    static unsigned long yesNoMsg[] = { MSG_YES, MSG_NO };
    if (waitMessage) {
        mInfoWindow.exec(*this, messageID, false);
    } else {
        mInfoWindow.open(messageID, false);
    }
    long result = actionMenu(yesNoMsg, 2, 510, 127, 0);
    mInfoWindow.close();
    return result;
}

GSgfxLayerID CMenuHologram::model_z_layer = GSGFX_LAYER_INVALID;

CMenuHologram::StageModel *CMenuHologram::modelPokemonLoad(long index)
{
    StageModel *model = &mModels[index];
    memset(model, 0, sizeof(StageModel));
    Pokemon *pokemon = index == MODEL_VISITOR ? mStage->getVisitorPointer()
                                              : mStage->getDancerPointer(index - MODEL_DANCER);
    if (pokemon == NULL) {
        return NULL;
    }
    unsigned short waza = HOLOGRAM_SEQUENCE_NONE;
    if (pokemon->isDarkPokemon() && !mStage->isReliveReady()) {
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE) == 1) {
            waza = HOLOGRAM_SEQUENCE_DARK_HYPER_MODE;
        } else {
            waza = HOLOGRAM_SEQUENCE_DARK;
        }
    }
    unsigned long rnd;
    unsigned short species;
    menuModelGetPkxRndFromPokemonForSeqenceLoad(rnd, species, pokemon);
    model->mSequence = Sequence_LoadPokemon(species, rnd, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SHINY, 0) != 0);
    model->mModel = Sequence_GetGSmodel((u32)model->mSequence);
    Sequence_SetVisibility(model->mSequence, 1);
    if (waza != HOLOGRAM_SEQUENCE_NONE) {
        WazaSequence *sequence = Sequence_LoadWaza(model->mSequence, waza, HOLOGRAM_SEQUENCE_CATEGORY, 0);
        if (sequence != NULL) {
            WazaSequence_Start(sequence, 0);
        }
    }
    Sequence_DoAnimation(model->mSequence, menuModelGetPokemonAnimationIndexByPointer(pokemon));
    GSmodelSetForceUseFractionalFramesFlag(model->mModel, 1);
    GSmodelSetSkipFractionalFramesFlag(model->mModel, 1);
    return model;
}

void CMenuHologram::modelVisitorSetYPosition(long index)
{
    StageModel *model = &mModels[index];
    unsigned short species = mStage->getVisitorPointer()->getPokemonDataId();
    float y = 0.0f;
    switch (species) {
    case POKEMON_DATA_ID_LEDYBA: y = -2.0f; break;
    case POKEMON_DATA_ID_LEDIAN: y = -10.0f; break;
    case POKEMON_DATA_ID_CARVANHA: y = -6.5f; break;
    case POKEMON_DATA_ID_SHARPEDO: y = -20.0f; break;
    case POKEMON_DATA_ID_DUSKULL: y = 0.0f; break;
    case POKEMON_DATA_ID_DUSCLOPS: y = 0.0f; break;
    case POKEMON_DATA_ID_SPEAROW: y = -2.0f; break;
    case POKEMON_DATA_ID_FEAROW: y = -9.0f; break;
    case POKEMON_DATA_ID_BEEDRILL: y = -5.5f; break;
    case POKEMON_DATA_ID_BUTTERFREE: y = -8.0f; break;
    case POKEMON_DATA_ID_VENOMOTH: y = -9.0f; break;
    case POKEMON_DATA_ID_WEEPINBELL: y = -2.0f; break;
    case POKEMON_DATA_ID_VICTREEBEL: y = -6.5f; break;
    case POKEMON_DATA_ID_MAGNETON: y = 0.0f; break;
    case POKEMON_DATA_ID_SOLROCK: y = -6.5f; break;
    case POKEMON_DATA_ID_LAPRAS: y = -6.5f; break;
    }
    GSvec position(0.0f, y, 0.0f);
    Sequence_SetPosition(model->mSequence, &position);
}

CMenuHologram::StageModel *CMenuHologram::modelPokemonSetScale(long index)
{
    StageModel *model = &mModels[index];
    Pokemon *pokemon = index == MODEL_VISITOR ? mStage->getVisitorPointer()
                                              : mStage->getDancerPointer(index - MODEL_DANCER);
    if (pokemon == NULL) {
        return NULL;
    }
    unsigned short species = pokemon->getPokemonDataId();
    float scale = menuGetPokemonScale((unsigned long)model->mSequence, species);
    scale = scale * relivehall_data->mModelScale / 100.0f;
    GSvec scaleVec(scale, scale, scale);
    Sequence_SetScale(model->mSequence, &scaleVec);
    return model;
}

void CMenuHologram::stageAllModelUnLoad()
{
    if (mStageModelLoaded) {
        if (mRelivedModel != NULL) {
            Sequence_SetVisibility(mRelivedModel, 0);
            Sequence_Free(mRelivedModel);
            mRelivedModel = NULL;
        }
        if (mStageWazaSequence != NULL) {
            WazaSequence_Free(mStageWazaSequence);
            Sequence_Free(mSequenceStage);
        }
        mStageWazaSequence = NULL;
        mSequenceStage = NULL;
        releaseOffscreenData();
        for (unsigned long i = 0; i < MODEL_QUANTITY; i++) {
            if (mModels[i].mModel != NULL) {
                if (mModels[i].mSequence != NULL) {
                    Sequence_SetVisibility(mModels[i].mSequence, 0);
                    Sequence_Free(mModels[i].mSequence);
                } else {
                    GSmodelSetVisibility(mModels[i].mModel, 0);
                    scriptModelClose(mModels[i].mResource);
                }
            }
        }
        memset(mModels, 0, sizeof(mModels));
        for (int i = 0; i < mLightQuantity; i++) {
            GSlightFree(mLights[i]);
        }
        memset(mLights, 0, sizeof(mLights));
        mLightQuantity = 0;
        if (mStageResource != 0) {
            GSfsysFree(mStageResource);
            GSresFreeGroup(mStageResource);
            mStageResource = 0;
        }
        cameraSetType(CAMERA_FREE);
        if (mHeroModel != NULL) {
            GSmodelFree(mHeroModel);
            mHeroModel = NULL;
        }
        mStageModelLoaded = false;
    }
}

_HSD_SceneDesc *_makeArchiveHeader(unsigned long resource)
{
    return (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress((HSD_Archive *)GSresGetResourceAllGroup(resource), (char *)sSceneDataName);
}

void CMenuHologram::initializeOffscreenData()
{
    mOffscreenTexture = GStextureCreate(0, 0, GS_TEXTURE_FORMAT_32BIT, 0, 0);
    if (mOffscreenTexture != 0) {
        GSgfxLayerConfig::Move(GSGFX_LAYER_DEFAULT, GSGFX_LAYER_BASE, GS_LAYER_INSERT_BEFORE);
        if (model_z_layer == GSGFX_LAYER_INVALID) {
            model_z_layer = GSgfxLayerConfig::Add((char *)sModelZLayerName, HOLOGRAM_PASS_ZERO_FIFO_BYTES, HOLOGRAM_PASS_ONE_FIFO_BYTES,
                                                  HOLOGRAM_MATRIX_STACK_CAPACITY, GS_GFX_LAYER_NORMAL,
                                                  false, false, false, GSGFX_LAYER_DEFAULT, GS_LAYER_INSERT_BEFORE);
            GSgfxLayer* zLayer = GSgfxLayerFindByID(model_z_layer);
            if (zLayer != 0) {
                zLayer->SetClearFlags(true, true, false);
            }
        }
        GSgfxLayer* layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
        layer->SetTextureCopy(mOffscreenTexture, false, 0);
        layer->SetClearFlags(true, true, true);
    }
}

void CMenuHologram::renderOffscreenData()
{
    if (mOffscreenTexture == 0) {
        return;
    }
    GSgfxLayer* layer = GSgfxLayerFindByID(GSgfxLayerID(HOLOGRAM_COMPOSITE_LAYER));
    if (layer == 0) {
        return;
    }
    GSgfxVF* vf = GSgfxVFBuildTemp(GX_POS_XY, GX_U16, 0, GX_NRM_XYZ, 1, GX_RGBA8, 1, GX_TEX_ST, GX_U8);
    spriteSetOffset(HOLOGRAM_COMPOSITE_LAYER, 0, 0);
    spriteClearScissor(HOLOGRAM_COMPOSITE_LAYER);
    spriteSetEnv(HOLOGRAM_COMPOSITE_LAYER);
    f32 matrix[4][4];
    GSmtx44Ortho(matrix, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(matrix);
    GSgfxMatrixStack* stack = layer->mMatrixStack;
    memcpy(stack->currentMatrix(), mtxIdent, sizeof(GSmtx));
    stack->mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->pixelMode(true, false, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_ALWAYS);
    layer->cullMode(GSGFX_CULL_BACK);
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->mVertexFormat = vf;
    layer->texCoordGeneratorCount(1);
    layer->tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, 0, false, 0);
    layer->cunitCount(1);
    layer->cunitChannelGen(0, GSGFX_CUNIT_COLOR_AND_ALPHA, GSGFX_CUNIT_VERTEX_COLOR, GSGFX_CUNIT_VERTEX_COLOR, 0,
                           0, GSGFX_LIGHT_POINT, GSGFX_DIFFUSE_NONE);
    layer->tunitCount(1);
    layer->tunitMapping(0, 0, 0, 0);
    layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_TEXC);
    layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_ONE, true);
    layer->texSelect(0, mOffscreenTexture);
    layer->beginSurface(4);
    hologramVertex2u16(0, 0);
    RelivehallData* data = relivehall_data;
    hologramColor4u8(255, 255, 255, data->mHologramAlpha);
    hologramTexCoord2u8(0, 0);
    hologramVertex2u16(640, 0);
    hologramColor4u8(255, 255, 255, data->mHologramAlpha);
    hologramTexCoord2u8(1, 0);
    hologramVertex2u16(640, 480);
    hologramColor4u8(255, 255, 255, data->mHologramAlpha);
    hologramTexCoord2u8(1, 1);
    hologramVertex2u16(0, 480);
    hologramColor4u8(255, 255, 255, data->mHologramAlpha);
    hologramTexCoord2u8(0, 1);
    layer->endSurface();
}

void CMenuHologram::releaseOffscreenData()
{
    GSgfxLayer* layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
    layer->SetClearFlags(false, false, false);
    layer->SetTextureCopy(0, false, 0);
    if (model_z_layer != GSGFX_LAYER_INVALID) {
        GSgfxLayerConfig::Remove(model_z_layer);
        model_z_layer = GSGFX_LAYER_INVALID;
    }
    GSgfxLayerConfig::Move(GSGFX_LAYER_DEFAULT, GSgfxLayerID(HOLOGRAM_RESTORE_BEFORE_LAYER), GS_LAYER_INSERT_BEFORE);
    if (mOffscreenTexture != 0) {
        GStextureFree(mOffscreenTexture);
        mOffscreenTexture = 0;
    }
}

float _getNeckRotationCB(GSpart *, GSpartMixType, void *, GSvec *rotation)
{
    static GSvec neckRotVec(-0.7853982f, 0.0f, 0.0f);
    *rotation = neckRotVec;
    return 1.0f;
}

void CMenuHologram::stageAllModelLoad()
{
    unsigned short waza;
    unsigned long camera;
    unsigned long scene;

    stageAllModelUnLoad();
    initializeOffscreenData();
    long state = mStage->getState();
    long dancerQuantity = mStage->getDancerQuantity();

    switch (state) {
    case CReliveStage::STATE_NO_TEMPO:
        if (mStage->getVisitorPointer() != NULL) {
            waza = HOLOGRAM_SEQUENCE_VISITOR;
        } else {
            waza = HOLOGRAM_SEQUENCE_NONE;
        }
        mStageResource = HOLOGRAM_ARCHIVE_NO_TEMPO;
        GSfsysReadWait(mStageResource);
        scriptModelLoad(MODEL_SCRIPT_BASE, HOLOGRAM_NO_TEMPO_BASE_MODEL);
        scriptModelLoad(MODEL_SCRIPT_PLATE, HOLOGRAM_NO_TEMPO_PLATE_MODEL);
        camera = HOLOGRAM_NO_TEMPO_CAMERA;
        scene = HOLOGRAM_NO_TEMPO_SCENE;
        break;
    case CReliveStage::STATE_HAS_TEMPO:
        waza = HOLOGRAM_SEQUENCE_TEMPO;
        mStageResource = HOLOGRAM_ARCHIVE_TEMPO;
        GSfsysReadWait(mStageResource);
        scriptModelLoad(MODEL_SCRIPT_BASE, HOLOGRAM_TEMPO_BASE_MODEL);
        scriptModelLoad(MODEL_SCRIPT_PLATE, HOLOGRAM_TEMPO_BASE_MODEL + dancerQuantity);
        camera = HOLOGRAM_TEMPO_CAMERA;
        scene = HOLOGRAM_TEMPO_SCENE;
        break;
    case CReliveStage::STATE_MAX_TEMPO:
        waza = HOLOGRAM_SEQUENCE_MAX_TEMPO;
        mStageResource = HOLOGRAM_ARCHIVE_MAX_TEMPO;
        GSfsysReadWait(mStageResource);
        scriptModelLoad(MODEL_SCRIPT_BASE, HOLOGRAM_MAX_TEMPO_BASE_MODEL);
        scriptModelLoad(MODEL_SCRIPT_PLATE, HOLOGRAM_MAX_TEMPO_PLATE_MODEL);
        camera = HOLOGRAM_MAX_TEMPO_CAMERA;
        scene = HOLOGRAM_MAX_TEMPO_SCENE;
        break;
    case CReliveStage::STATE_RELIVE_READY:
        waza = HOLOGRAM_SEQUENCE_MAX_TEMPO;
        mStageResource = HOLOGRAM_ARCHIVE_RELIVE_READY;
        GSfsysReadWait(mStageResource);
        scriptModelLoad(MODEL_SCRIPT_BASE, HOLOGRAM_READY_BASE_MODEL);
        scriptModelLoad(MODEL_SCRIPT_PLATE, HOLOGRAM_READY_BASE_MODEL + dancerQuantity);
        camera = HOLOGRAM_READY_CAMERA;
        scene = HOLOGRAM_READY_SCENE;
        break;
    }

    Pokemon *visitor = mStage->getVisitorPointer();
    if (visitor != NULL) {
        StageModel *model = modelPokemonLoad(MODEL_VISITOR);
        GSpart *part = GSmodelGetPart(mModels[MODEL_SCRIPT_PLATE].mModel,
                                      relivehall_data->mPartIndex[plateIndexTbl[state][dancerQuantity][0]]);
        GSmodelAttachToGSpart(model->mModel, part, GS_ATTACH_TRANSFORM, 0, 1);
        GSpartFree(part);
        modelVisitorSetYPosition(MODEL_VISITOR);
        modelPokemonSetScale(MODEL_VISITOR);
    }

    for (int i = 0; i < dancerQuantity; i++) {
        int modelIndex = MODEL_DANCER + i;
        StageModel *model = modelPokemonLoad(modelIndex);
        long slot = (i - mStage->mCenterDancer + dancerQuantity) % dancerQuantity + 1;
        GSpart *part = GSmodelGetPart(mModels[MODEL_SCRIPT_PLATE].mModel,
                                      relivehall_data->mPartIndex[plateIndexTbl[state][dancerQuantity][slot]]);
        GSmodelAttachToGSpart(model->mModel, part, GS_ATTACH_TRANSFORM, 0, 1);
        GSpartFree(part);
        modelPokemonSetScale(modelIndex);
    }

    GSmodel *plate = mModels[MODEL_SCRIPT_PLATE].mModel;
    GSmaterial *materials = plate != NULL ? plate->mMaterials : NULL;
    GSmodelDisableModulation(plate);

    if (dancerQuantity != 0) {
        GSgfxColAu8 color;
        memset(&color, 0, sizeof(color));
        if (visitor != NULL) {
            color = pokemonToAisyouColor(visitor, mStage->getDancerPointer((s8)mStage->mCenterDancer));
        }
        GSmaterial *material = &materials[relivehall_data->mPartIndex[colorLineIndexTbl[state][dancerQuantity][0]] - 1];
        GSmaterialSetModulate(material, (GScolor *)&color);
        GSmaterialEnableExtension(material, 1);
    }

    for (int i = 0; i < dancerQuantity; i++) {
        GSgfxColAu8 color = pokemonToAisyouColor(mStage->getDancerPointer(i),
                                                 mStage->getDancerPointer((i + 1) % dancerQuantity));
        long slot = (i - mStage->mCenterDancer + dancerQuantity) % dancerQuantity + 1;
        GSmaterial *material = &materials[relivehall_data->mPartIndex[colorLineIndexTbl[state][dancerQuantity][slot]] - 1];
        GSmaterialSetModulate(material, (GScolor *)&color);
        GSmaterialEnableExtension(material, 1);
    }

    cameraPlayAnime(mStageResource, camera, 0.0f, 1);
    cameraSetType(CAMERA_ANIMATION);

    _HSD_SceneDesc *header = _makeArchiveHeader(scene);
    if (header->mLights != NULL) {
        floorReadMakeLightResID(scene);
        for (long lightIndex = 0;; lightIndex++) {
            if (header->mLights[lightIndex] == NULL) {
                break;
            }
            GSlight *light = GSlightLoad(header->mLights[lightIndex]);
            mLights[mLightQuantity++] = light;
            GSlightSetActive(light, 1);
            if (GSlightCanAnimate(light)) {
                GSlightSetAnimIndex(light, HOLOGRAM_LIGHT_ANIMATION);
                GSlightSetAnimRate(light, 0.5f);
                GSlightStartAnimation(light);
            }
        }
    }

    GSgfxLayerID layers[] = { GSGFX_LAYER_BASE, GSGFX_LAYER_BASE };
    layers[1] = model_z_layer;
    mHeroModel = floorOpenObject(HOLOGRAM_HERO_MODEL);
    GSmodelSetAnimIndex(mHeroModel, HOLOGRAM_HERO_ANIMATION);
    GSmodelSetAnimRate(mHeroModel, 0.5f);
    GSmodelStartAnimation(mHeroModel);
    GSmodelSetVisibility(mHeroModel, true);
    GSmodelSetAnimType(mHeroModel, GS_MODEL_ANIM_LOOP);
    GSmodelSetRenderLayer(mHeroModel, 2, layers);
    s8 neckIndex = peopleInfoBiosGetNeckIndex(peopleInfoBiosGetPtr(HOLOGRAM_HERO_MODEL));
    GSpart *neck = GSmodelGetPart(mHeroModel, neckIndex);
    GSpartRegisterAnimMix(neck, GS_MODEL_PART_MIX_ROTATION, HOLOGRAM_NECK_MIX_MASK, _getNeckRotationCB, NULL);
    GSpartFree(neck);

    if (waza != HOLOGRAM_SEQUENCE_NONE) {
        mSequenceStage = Sequence_CreateNull();
        mStageWazaSequence = Sequence_LoadWaza(mSequenceStage, waza, HOLOGRAM_SEQUENCE_CATEGORY, 0);
        GSvec position(0.0f, 0.0f, 0.0f);
        GSvec rotation(0.0f, 0.0f, 0.0f);
        Sequence_SetPosition(mSequenceStage, &position);
        Sequence_SetRotation(mSequenceStage, &rotation);
        WazaSequence_Start(mStageWazaSequence, 0);
    }
    mStageModelLoaded = true;
}

void CMenuHologram::bgmKick()
{
    unsigned long bgm;
    mStage->getState();
    long tempo = mStage->getTempo();
    if (tempo <= 0) {
        bgm = BGM_HOLOGRAM_NO_TEMPO;
    } else if (tempo <= 53) {
        bgm = BGM_HOLOGRAM_TEMPO_SLOW;
    } else if (tempo <= 95) {
        bgm = BGM_HOLOGRAM_TEMPO_MIDDLE;
    } else {
        bgm = BGM_HOLOGRAM_TEMPO_FAST;
    }
    unsigned long current = soundGetBGMID();
    if (current != bgm) {
        menuPtPlayBgm(bgm);
    }
}

void CMenuHologram::bgmStop()
{
    soundStop(soundGetBGMID(), 50);
}

GSgfxColAu8 CMenuHologram::pokemonToAisyouColor(Pokemon *pokemon, Pokemon *partner)
{
    long aisyou = reliveHallPokemonToAisyou(pokemon, partner);
    if (aisyou == -1) {
        return GSgfxColAu8_Black;
    }
    return relivehallAisyou2ColorTbl[aisyou];
}

void CMenuHologram::unloadDatas()
{
    localWindowDeleteAll();
    stageAllModelUnLoad();
}

void CMenuHologram::reloadDatas(bool kickBgm)
{
    stageAllModelLoad();
    if (mModels[MODEL_SCRIPT_BASE].mModel != NULL) {
        GSmodelSetAnimIndex(mModels[MODEL_SCRIPT_BASE].mModel, SCRIPT_ANIMATION_WAIT);
        GSmodelSetAnimType(mModels[MODEL_SCRIPT_BASE].mModel, GS_MODEL_ANIM_LOOP);
        GSmodelStartAnimation(mModels[MODEL_SCRIPT_BASE].mModel);
    }
    if (mModels[MODEL_SCRIPT_PLATE].mModel != NULL) {
        GSmodelSetAnimIndex(mModels[MODEL_SCRIPT_PLATE].mModel, SCRIPT_ANIMATION_WAIT);
        GSmodelSetAnimType(mModels[MODEL_SCRIPT_PLATE].mModel, GS_MODEL_ANIM_LOOP);
        GSmodelStartAnimation(mModels[MODEL_SCRIPT_PLATE].mModel);
    }
    localWindowCreateAll();
    if (kickBgm) {
        bgmKick();
    }
}

void CMenuHologram::moveStageWait(int stage)
{
    systemFadeOutSetAndWait(0.1f);
    unloadDatas();
    setStage(stage);
    reloadDatas(true);
    systemFadeInKick();
}

void CMenuHologram::setStageToReliveReady()
{
    int stage;
    for (stage = 0; stage < STAGE_QUANTITY; stage++) {
        if (CReliveHall::getPointer()->getStage(stage)->isReliveReady()) {
            break;
        }
    }
    if (stage == STAGE_QUANTITY) {
        setStage(relivehallGetLastViewStage());
    } else {
        setStage(stage);
    }
}

void CMenuHologram::setStage(int stage)
{
    mStageIndex = stage;
    mSelectedStageIndex = stage;
    mStage = CReliveHall::getPointer()->getStage(mStageIndex);
}

void CMenuHologram::stateMainProc()
{
    long *work = mState.mWork;
    do {
        tagWINDOW_KEY *key = getKeyInfo();
        if (key->mKeyTrg | key->mKeyRepeat) {
            switch (key->mKeyRepeat) {
            case WINDOW_KEY_LEFT:
            case WINDOW_KEY_TRIGGER_L:
                mSelectedStageIndex = (mSelectedStageIndex + STAGE_QUANTITY - 1) % STAGE_QUANTITY;
                menuPtPlaySe(SE_STAGE_CHANGE);
                break;
            case WINDOW_KEY_RIGHT:
            case WINDOW_KEY_TRIGGER_R:
                mSelectedStageIndex = (mSelectedStageIndex + 1) % STAGE_QUANTITY;
                menuPtPlaySe(SE_STAGE_CHANGE);
                break;
            }
            switch (key->mKeyTrg) {
            case WINDOW_KEY_ACCEPT:
                menuPtPlaySe(SE_MENU_DECIDE);
                setState(STATE_MENU, work);
                break;
            case WINDOW_KEY_CANCEL:
                menuPtPlaySe(SE_MENU_CANCEL);
                setState(STATE_EXIT_QUERY, work);
                break;
            }
        }
        updateAll();
    } while (mState.mNext == -1);
}

void CMenuHologram::stateReliveCeremony()
{
    enum { FLAG_RELIVE_CEREMONY = 1443, BGM_RELIVE_CEREMONY = 1448, SE_RELIVE_CEREMONY = 1447 };
    enum { MSG_NICKNAME = 50, MSG_RELIVE_COMPLETE = 53598, MSG_PCBOX_FULL = 53554, MSG_RELIVE_ANOTHER = 53553 };
    const unsigned long ceremonyCamera = HOLOGRAM_CEREMONY_CAMERA;
    GSflagSet(FLAG_RELIVE_CEREMONY, 1);
    mStageTurning = false;
    for (;;) {
        if (getModelAnimation(mModels[MODEL_SCRIPT_PLATE].mModel) == SCRIPT_ANIMATION_WAIT) {
            break;
        }
        updateAll();
    }
    aButtonWait();
    menuPtPlayBgm(BGM_RELIVE_CEREMONY);
    menuPtPlaySe(SE_RELIVE_CEREMONY);
    cameraPlayAnime(floorGetCurrentGroupID(), ceremonyCamera, 0.0f, 0);
    cameraSetType(CAMERA_ANIMATION);
    playModelAnimation(SCRIPT_ANIMATION_RELIVE, mModels[MODEL_SCRIPT_BASE].mModel, GS_MODEL_ANIM_ONCE);
    waitAtTime(0.3f);
    GSmodelSetVisibility(mHeroModel, false);
    waitAtTime(6.0f);
    Pokemon *visitor = mStage->getVisitorPointer();
    mReliveCeremonyInProgress = true;
    mRelivedModel = reliveCeremonyFromHologram(visitor, (unsigned long)mModels[MODEL_VISITOR].mSequence);
    mReliveCeremonyInProgress = false;
    systemFadeInKick();
    systemFadeWaitToEnd();
    u16 nickname[12];
    GScharCpy(nickname, visitor->getNicknamePtr());
    unsigned short trigger;
    do {
        GSthreadSwitch();
        trigger = windowGetKeyTrg();
    } while (windowGetKeyOn() != 0 || trigger != 0);
    mInfoWindow.setMsgCtrlValue(MSG_NICKNAME, (unsigned long)nickname);
    mInfoWindow.exec(*this, MSG_RELIVE_COMPLETE, true);
    Pokemon pokemon;
    mStage->pickupVisitor(pokemon);
    PCBOX::getPointer()->addPokemon(&pokemon);
    if (CReliveHall::getPointer()->isReliveReadyPokemonExist()) {
        systemFadeInKick();
        if (PCBOX::getPointer()->isPokemonFull()) {
            mInfoWindow.exec(*this, MSG_PCBOX_FULL, true);
            setState(STATE_EXIT_CEREMONY);
        } else if (actionYesNo(MSG_RELIVE_ANOTHER, true) == 0) {
            systemFadeOutSetAndWait(-1.0f);
            setStageToReliveReady();
            reloadDatas(true);
            systemFadeInKick();
            setState(STATE_RELIVE_CEREMONY);
        } else {
            setState(STATE_EXIT_CEREMONY);
        }
    } else {
        setState(STATE_EXIT_CEREMONY);
    }
}

void CMenuHologram::drawAll()
{
    if (mHeroModel != NULL) {
        static GSvec pos(0.0f, 0.0f, 0.0f);
        static GSvec rot(0.0f, 3.1415927f, 0.0f);
        static GSvec scl(1.0f, 1.0f, 1.0f);
        GSmodelSetPosition(mHeroModel, &pos);
        GSmodelSetRotation(mHeroModel, &rot);
        GSmodelSetScale(mHeroModel, &scl);
    }
    renderOffscreenData();
    if (!mReliveCeremonyInProgress) {
        if (mModels[MODEL_SCRIPT_BASE].mModel != NULL) {
            long animation = getModelAnimation(mModels[MODEL_SCRIPT_BASE].mModel);
            if ((animation == SCRIPT_ANIMATION_ENTER || animation == SCRIPT_ANIMATION_RELIVE) && GSmodelHasAnimationEnded(mModels[MODEL_SCRIPT_BASE].mModel)) {
                long nextAnimation = animation + 1;
                playModelAnimation(nextAnimation, mModels[MODEL_SCRIPT_BASE].mModel, GS_MODEL_ANIM_LOOP);
            }
        }
        if (mModels[MODEL_SCRIPT_PLATE].mModel != NULL) {
            long animation = getModelAnimation(mModels[MODEL_SCRIPT_PLATE].mModel);
            if ((animation == SCRIPT_ANIMATION_ENTER || animation == SCRIPT_ANIMATION_RELIVE) && GSmodelHasAnimationEnded(mModels[MODEL_SCRIPT_PLATE].mModel)) {
                long nextAnimation = animation + 1;
                playModelAnimation(nextAnimation, mModels[MODEL_SCRIPT_PLATE].mModel, GS_MODEL_ANIM_LOOP);
            }
        }
        if (mStageTurning) {
            float rate = calcAnimationTimeRate0to1(32.0f);
            GSvec rotation(0.0f, 2.0f * (3.1415927f * -rate), 0.0f);
            GSmodelSetRotation(mModels[MODEL_SCRIPT_PLATE].mModel, &rotation);
        }
        CReliveStage *stage = CReliveHall::getPointer()->getStage(mSelectedStageIndex);
        int tempo = stage->getTempo();
        relivehallDraw9StageNumber(mStageIndex);
        relivehallDraw9StageInfo(mSelectedStageIndex);
        bool showCursor = false;
        if (mState.mCurrent != STATE_RELIVE_CEREMONY && mState.mCurrent != STATE_EXIT_CEREMONY) {
            showCursor = true;
        }
        relivehallDrawStageInfoCursor(mSelectedStageIndex, tempo, stage->getDancerQuantity(), stage->getVisitorPointer(), showCursor);
        mInfoWindow.draw();
        localWindowDrawAll();
        menuDrawReliveUpdate();
    }
}

void thisNew()
{
    pMenuHologram = new (GSmemAllocClr(sizeof(CMenuHologram))) CMenuHologram;
}

void thisFree()
{
    if (pMenuHologram != NULL) {
        GSmemFree(pMenuHologram);
        pMenuHologram = NULL;
    }
}

void thisExec(CMenuGlobalState &state)
{
    thisNew();
    pMenuHologram->main(state);
    thisFree();
}

}

void menuHologramExecFromPcBox(CMenuGlobalState &state)
{
    thisExec(state);
}

void menuHologramUnloadDatas()
{
    pMenuHologram->unloadDatas();
}

void menuHologramReloadDatas()
{
    pMenuHologram->reloadDatas(false);
}
