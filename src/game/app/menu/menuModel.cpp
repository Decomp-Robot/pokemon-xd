#include <menuModel.h>
#include <relglobal.h>
#include <GSgfx.h>
#include <GSthread.h>
#include <GSmodel.h>
#include <GScamera.h>
#include <GSlight.h>
#include <GSbound.h>
#include <GSpart.h>
#include <GSmtx.h>
#include <GSAPI/GSvec/GSvec.h>
#include <Pokemon.h>
#include <fightSeq.h>
#include <memo.h>
#include <sequence.h>
#include <wazaSequence.h>
#include <floor.h>
#include <menuFace.h>
#include <GSlog.h>
#include <math.h>
#include <string.h>


static GSgfxLayerID _layerID = GSGFX_LAYER_DEFAULT;
static f32 _menuModelScaleMultiValue = 1.0f;
static f32 _camFov = 35.0f;
static GXColor _lightColor = { 128, 128, 128, 255 };
static s32 _waitframe = 3;
static f32 _disp_scale = 0.7f;
static u8 _litonoff[3] = { 1, 1, 1 };
s32 _menuModelRef;
static u8 _menuModelFitViewport;
GSvec _testpos(0.0f, 0.0f, 0.0f);
GSvec _testPos(0.0f, 0.0f, 0.0f);
GSvec _testPos2D(0.0f, 0.0f, 0.0f);
GSvec _lightOffset(0.0f, 0.0f, 0.0f);

void _menuModelRenderOff(MENU_MODEL *model);
void _menuModelSetupViewport2(MENU_MODEL *model, MENU_MODEL *other);
void _menuModelSetupViewport(MENU_MODEL *model);
s32 _menuModelAdjustCamera(MENU_MODEL *model, bool reset);
s32 _menuModelAdjustCamera2(MENU_MODEL *model, MENU_MODEL *other);
s32 _menuModelLoadThread(MENU_MODEL *model);
s32 _menuModelTerm(MENU_MODEL *model);
bool _menuModelCmpLoadInfo(MENU_MODEL_LOAD *a, MENU_MODEL_LOAD *b);
void _menuModelMakeLoadInfo(MENU_MODEL_LOAD *info, Pokemon *pokemon,
                             u16 wazaID, bool checkRare);

enum MENU_MODEL_STATE {
    MENU_MODEL_STATE_NONE,
    MENU_MODEL_STATE_LOADING,
    MENU_MODEL_STATE_READY
};
enum MENU_MODEL_WAZA {
    MENU_MODEL_WAZA_DARK = 37,
    MENU_MODEL_WAZA_HYPER_MODE = 135
};
enum MENU_MODEL_LIGHT {
    MENU_MODEL_LIGHT_AMBIENT,
    MENU_MODEL_LIGHT_INFINITE,
    MENU_MODEL_LIGHT_POINT
};
static const u8 MENU_MODEL_WAZA_CATEGORY = 4;
static const u16 MENU_MODEL_PKX_DARK_LUGIA = 417;


void _menuModelMakeLoadInfo(MENU_MODEL_LOAD *info, Pokemon *pokemon,
                             u16 wazaID, bool checkRare)
{
    info->isPokemon = 1;
    if (pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_SEALED, 0) != 0) {
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE) == 1) {
            info->id.pokemon.wazaID = MENU_MODEL_WAZA_HYPER_MODE;
        } else {
            info->id.pokemon.wazaID = MENU_MODEL_WAZA_DARK;
        }
    } else {
        info->id.pokemon.wazaID = wazaID;
    }
    menuModelGetPkxRndFromPokemonForSeqenceLoad(
        info->pokemonRandom, info->id.pokemon.pkxID, pokemon);
    if (checkRare) {
        info->isRare = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SHINY, 0) != 0;
    } else {
        info->isRare = 0;
    }
    info->animIndex = menuModelGetPokemonAnimationIndexByPointer(pokemon);
}

bool _menuModelCmpLoadInfo(MENU_MODEL_LOAD *a, MENU_MODEL_LOAD *b)
{
    if (a->isPokemon == b->isPokemon) {
        if (a->isPokemon) {
            if (a->id.pokemon.pkxID == b->id.pokemon.pkxID &&
                a->id.pokemon.wazaID == b->id.pokemon.wazaID &&
                a->pokemonRandom == b->pokemonRandom &&
                a->isRare == b->isRare) return true;
        } else if (a->id.objectID == b->id.objectID) return true;
    }
    return false;
}

void _menuModelSetupViewport(MENU_MODEL *model)
{
    f32 x;
    f32 y;
    f32 width;
    f32 height;
    f32 originalWidth;
    f32 originalHeight;
    f32 xOffset;
    f32 yOffset;

    if (model->camera == NULL) return;
    x = model->x;
    y = model->y;
    width = model->width;
    height = model->height;
    originalWidth = width;
    originalHeight = height;
    if (_menuModelFitViewport) {
        if (width > height) width = 1.3333334f * height;
        else height = width / 1.3333334f;
    } else {
        if (width < height) width = 1.3333334f * height;
        else height = width / 1.3333334f;
    }
    xOffset = 0.5f * (originalWidth - width);
    yOffset = 0.5f * (originalHeight - height);
    x += xOffset;
    y += yOffset;
    if (model->clip) {
        GScameraSetScissor(model->camera, 1, (u16)model->x, (u16)model->y,
                           (u16)model->width, (u16)model->height);
    } else {
        GScameraSetScissor(model->camera, 0, 0, 0, 640, 480);
    }
    if (x < 0.0f) x = 0.0f;
    if (y < 0.0f) y = 0.0f;
    GScameraSetViewport(model->camera, 1, x, y,
                        x + width * 0.5f, y + height * 0.5f);
    if (model->visibilityDelay < _waitframe) model->visibilityDelay++;
    if (model->visibilityDelay >= _waitframe) {
        if (model->current.isPokemon) {
            Sequence_SetVisibility(model->object.sequence, true);
            Sequence_SetCamera(model->object.sequence, model->camera);
        } else {
            GSmodelSetVisibility(model->object.model, true);
            GSmodelSetRenderCamera(model->object.model, model->camera);
        }
    }
}

s32 _menuModelAdjustCamera(MENU_MODEL *model, bool reset)
{
    static GSvec upVec(0.0f, 1.0f, 0.0f);
    static GSvec target(0.0f, 0.0f, 0.0f);
    _GSmodel *gsmodel;
    GSpart *part;
    GSbound *bound;
    f32 inverseMatrix[3][4];
    f32 rotationX[3][4];
    f32 rotationY[3][4];
    GSvec cameraPosition;
    GSvec centerPosition;
    GSvec modelPosition;
    GSvec lightPosition;
    GSvec centerOffset;
    f32 fov;
    f32 aspect;
    f32 nearPlane;
    f32 farPlane;
    f32 modelScale;
    f32 animationAngle;
    GXColor lightColor;
    f32 sizeScale;
    f32 maxSize;
    f32 distance;
    f32 aspectScale;
    f32 lightDistance;

    if (model == NULL) return 0;
    if (model->current.isPokemon) {
        gsmodel = Sequence_GetGSmodel((u32)model->object.sequence);
        switch (Sequence_GetModelSize((u32)model->object.sequence)) {
        case MODEL_SIZE_CLASS_SMALLEST: sizeScale = 0.8f; break;
        case MODEL_SIZE_CLASS_SMALL: sizeScale = 0.9f; break;
        case MODEL_SIZE_CLASS_MEDIUM: sizeScale = 1.0f; break;
        case MODEL_SIZE_CLASS_LARGE: sizeScale = 1.1f; break;
        case MODEL_SIZE_CLASS_LARGER: sizeScale = 1.15f; break;
        case MODEL_SIZE_CLASS_LARGEST: sizeScale = 1.2f; break;
        default: sizeScale = 1.0f; break;
        }
        memoGetScaleAngle(model->current.id.pokemon.pkxID, &modelScale,
                          &animationAngle);
    } else {
        gsmodel = model->object.model;
        sizeScale = 1.0f;
        modelScale = 1.0f;
    }
    if (gsmodel == NULL) {
        model->state = MENU_MODEL_STATE_NONE;
        return 0;
    }
    part = GSmodelGetPart(gsmodel, GSmodelCenterNull(gsmodel));
    if (part != NULL) {
        GSpartGetTransform(part, &centerPosition, NULL, NULL);
        GSpartFree(part);
    }
    if (model->boundsValid == 0) {
        bound = GSmodelGetBound(gsmodel);
        GSboundGetSize(bound, &model->boundsSize, false, false);
        GSboundGetMinMax(bound, &model->boundsMin, &model->boundsMax,
                         0, 0, 0, 0);
        model->boundsValid = 1;
    }
    modelPosition.x = 0.0f;
    modelPosition.z = 0.0f;
    modelPosition.y = -(model->boundsMin.y + model->boundsMax.y) / 2.0f;
    PSVECAdd(&modelPosition, &model->mOffset, &modelPosition);
    GSmodelSetPosition(gsmodel, &modelPosition);
    if (model->camera == NULL) return 0;
    GScameraGetPerspective(model->camera, &fov, &aspect, &nearPlane, &farPlane);
    aspect = 1.3333334f;
    fov = _camFov;
    if (model->width > model->height) {
        aspectScale = 1.0f;
    } else {
        aspectScale = 1.0f;
    }
    if (model->boundsSize.x > model->boundsSize.y) {
        maxSize = model->boundsSize.x;
    } else {
        maxSize = model->boundsSize.y;
    }
    distance = 0.8f * (maxSize / sizeScale) /
               (f32)tan(0.017453292f * fov / 2.0f);
    distance = distance * modelScale / aspectScale / model->zoom;
    cameraPosition.x = 0.0f;
    cameraPosition.y = 0.0f;
    cameraPosition.z = distance;
    PSMTXRotRad(rotationX, 'X', model->mAngle.x);
    PSMTXRotRad(rotationY, 'Y', -model->mAngle.y);
    PSMTXConcat(rotationY, rotationX, rotationX);
    PSMTXMultVec(rotationX, &cameraPosition, &cameraPosition);
    farPlane = 4000.0f;
    GScameraSetPosition(model->camera, &cameraPosition);
    GScameraSetPerspective(model->camera, fov, aspect, nearPlane, farPlane);
    GScameraLookAt(model->camera, &upVec, &target);
    C_MTXLookAt(model->viewMatrix, (const Vec *)&cameraPosition,
                (const Vec *)&upVec, (const Point3d *)&target);
    PSMTXInverse(model->viewMatrix, inverseMatrix);
    PSVECSubtract(&modelPosition, &centerPosition, &centerOffset);
    lightDistance = PSVECDistance(&lightPosition, &centerPosition);
    memcpy(&lightColor, &_lightColor, sizeof(lightColor));
    memcpy(&lightPosition, &cameraPosition, sizeof(lightPosition));
    lightPosition = lightPosition + _lightOffset;
    centerPosition = centerPosition + _lightOffset;
    GSlightSetType(model->light[MENU_MODEL_LIGHT_POINT], GSLIGHT_TYPE_POINT);
    GSlightSetColor(model->light[MENU_MODEL_LIGHT_POINT], lightColor);
    GSlightSetPosition(model->light[MENU_MODEL_LIGHT_POINT], &lightPosition);
    GSlightSetTarget(model->light[MENU_MODEL_LIGHT_POINT], &centerPosition);
    return 1;
}

void _menuModelSetupViewport2(MENU_MODEL *model, MENU_MODEL *other)
{
    f32 x;
    f32 y;
    f32 width;
    f32 height;
    f32 originalWidth;
    f32 originalHeight;
    f32 xOffset;
    f32 yOffset;

    if (model->camera == NULL) return;
    x = model->x;
    y = model->y;
    width = model->width;
    height = model->height;
    originalWidth = width;
    originalHeight = height;
    if (width > height) width = 1.3333334f * height;
    else height = width / 1.3333334f;
    xOffset = 0.5f * (originalWidth - width);
    yOffset = 0.5f * (originalHeight - height);
    x += xOffset;
    y += yOffset;
    if (model->clip) {
        GScameraSetScissor(model->camera, 1, (s32)x, (s32)y,
                           (s32)width, (s32)height);
    } else {
        GScameraSetScissor(model->camera, 0, 0, 0, 640, 480);
    }
    GScameraSetViewport(model->camera, 1, x, y,
                        x + width * 0.5f, y + height * 0.5f);
    if (model->visibilityDelay < _waitframe) model->visibilityDelay++;
    if (model->visibilityDelay >= _waitframe) {
        if (model->current.isPokemon) {
            Sequence_SetVisibility(model->object.sequence, true);
            Sequence_SetCamera(model->object.sequence, model->camera);
        } else {
            GSmodelSetVisibility(model->object.model, true);
            GSmodelSetRenderCamera(model->object.model, model->camera);
        }
        if (other->current.isPokemon) {
            Sequence_SetVisibility(other->object.sequence, true);
            Sequence_SetCamera(other->object.sequence, model->camera);
        } else {
            GSmodelSetVisibility(other->object.model, true);
            GSmodelSetRenderCamera(other->object.model, model->camera);
        }
    } else {
        _menuModelRenderOff(model);
        _menuModelRenderOff(other);
    }
}

s32 _menuModelAdjustCamera2(MENU_MODEL *model, MENU_MODEL *other)
{
    static GSvec upVec(0.0f, 1.0f, 0.0f);
    static GSvec target(0.0f, 0.0f, 0.0f);
    _GSmodel *gsmodel;
    _GSmodel *otherModel;
    f32 rotationX[3][4];
    f32 rotationY[3][4];
    GSvec cameraPosition;
    GSvec lightTarget;
    GSvec position;
    GSvec otherPosition;
    GSvec lightPosition;
    f32 fov;
    f32 aspect;
    f32 nearPlane;
    f32 farPlane;
    f32 modelScale;
    f32 animationAngle;
    GXColor lightColor;
    GSvec extent;
    f32 sizeScale;
    f32 center;
    f32 distance;
    f32 specialScale;
    f32 maxExtent;

    if (model == NULL) return 0;
    if (other == NULL) return 0;
    if (model->current.isPokemon) {
        gsmodel = Sequence_GetGSmodel((u32)model->object.sequence);
        switch (Sequence_GetModelSize((u32)model->object.sequence)) {
        case MODEL_SIZE_CLASS_SMALLEST: sizeScale = 0.8f; break;
        case MODEL_SIZE_CLASS_SMALL: sizeScale = 0.9f; break;
        case MODEL_SIZE_CLASS_MEDIUM: sizeScale = 1.0f; break;
        case MODEL_SIZE_CLASS_LARGE: sizeScale = 1.1f; break;
        case MODEL_SIZE_CLASS_LARGER: sizeScale = 1.15f; break;
        case MODEL_SIZE_CLASS_LARGEST: sizeScale = 1.2f; break;
        default: sizeScale = 1.0f; break;
        }
        memoGetScaleAngle(model->current.id.pokemon.pkxID, &modelScale,
                          &animationAngle);
    } else {
        gsmodel = model->object.model;
        sizeScale = 1.0f;
        modelScale = 1.0f;
    }
    if (other != NULL) otherModel = other->object.model;
    if (gsmodel == NULL) {
        model->state = MENU_MODEL_STATE_NONE;
        return 0;
    }
    if (otherModel == NULL) return 0;
    if (model->boundsValid == 0) {
        GSbound *bound = GSmodelGetBound(gsmodel);
        if (bound == NULL) return 0;
        GSboundGetSize(bound, &model->boundsSize, false, false);
        GSboundGetMinMax(bound, &model->boundsMin, &model->boundsMax,
                         0, 0, 0, 0);
        model->boundsValid = 1;
    }
    if (other->boundsValid == 0) {
        GSbound *bound = GSmodelGetBound(otherModel);
        if (bound == NULL) return 0;
        GSboundGetSize(bound, &other->boundsSize, false, false);
        GSboundGetMinMax(bound, &other->boundsMin, &other->boundsMax,
                         0, 0, 0, 0);
        other->boundsValid = 1;
    }

    extent.x = model->boundsSize.x + other->boundsSize.x;
    extent.y = model->boundsSize.y >= other->boundsSize.y
                 ? model->boundsSize.y : other->boundsSize.y;
    center = -other->boundsSize.x + extent.x / 2.0f;

    GSmodelGetPosition(gsmodel, &position);
    position.y = 0.0f;
    position.x = -(model->boundsSize.x / 2.0f) + center;
    position.z = 0.0f;
    PSVECAdd(&position, &model->mOffset, &position);
    GSmodelSetPosition(gsmodel, &position);
    GSmodelGetPosition(otherModel, &otherPosition);
    otherPosition.y = 0.0f;
    otherPosition.x = other->boundsSize.x / 2.0f + center;
    otherPosition.z = 0.0f;
    PSVECAdd(&otherPosition, &other->mOffset, &otherPosition);
    GSmodelSetPosition(otherModel, &otherPosition);

    if (model->camera == NULL) return 0;
    GScameraGetPerspective(model->camera, &fov, &aspect, &nearPlane, &farPlane);
    aspect = 1.3333334f;
    fov = _camFov;
    switch (model->current.id.pokemon.pkxID) {
    case POKEMON_SPECIES_RAPIDASH:
    case POKEMON_SPECIES_GYARADOS:
    case POKEMON_SPECIES_ARTICUNO:
    case POKEMON_SPECIES_MEWTWO:
    case POKEMON_SPECIES_STEELIX:
    case POKEMON_SPECIES_RAIKOU:
    case POKEMON_SPECIES_HOUOU:
        specialScale = _disp_scale;
        break;
    default:
        specialScale = 1.0f;
        break;
    }
    maxExtent = extent.x > extent.y ? extent.x : extent.y;
    distance = 0.8f * (maxExtent / sizeScale) / (f32)tan(0.017453292f * fov / 2.0f);
    distance = distance * modelScale / specialScale / model->zoom;
    target.y = 0.75f * extent.y;
    cameraPosition.x = 0.0f;
    cameraPosition.y = 0.0f;
    cameraPosition.z = distance;
    PSMTXRotRad(rotationX, 'X', model->mCameraRotation.x);
    PSMTXRotRad(rotationY, 'Y', -model->mCameraRotation.y);
    PSMTXConcat(rotationY, rotationX, rotationX);
    PSMTXMultVec(rotationX, &cameraPosition, &cameraPosition);
    GScameraSetPosition(model->camera, &cameraPosition);
    GScameraSetPerspective(model->camera, fov, aspect, nearPlane, farPlane);
    GScameraLookAt(model->camera, &upVec, &target);
    C_MTXLookAt(model->viewMatrix, (const Vec *)&cameraPosition,
                (const Vec *)&upVec, (const Point3d *)&target);
    memcpy(&lightColor, &_lightColor, sizeof(lightColor));
    memcpy(&lightPosition, &cameraPosition, sizeof(lightPosition));
    lightPosition.x += _lightOffset.x;
    lightPosition.y += _lightOffset.y;
    lightPosition.z += _lightOffset.z;
    GSlightSetType(model->light[MENU_MODEL_LIGHT_POINT], GSLIGHT_TYPE_POINT);
    GSlightSetColor(model->light[MENU_MODEL_LIGHT_POINT], lightColor);
    GSlightSetPosition(model->light[MENU_MODEL_LIGHT_POINT], &lightPosition);
    GSlightSetTarget(model->light[MENU_MODEL_LIGHT_POINT], &lightTarget);
    return 1;
}

s32 _menuModelLoadThread(MENU_MODEL *model)
{
    _GSmodel *gsmodel;
    if (model == NULL) return 0;
    model->unknownFloat = 0.0f;
    if (!model->loadRequest) return 0;
    while (menuFaceCheckLoading()) {
        GSthreadSwitch();
    }
    wazaSequenceSysSetVisibilityAll(false);
    _menuModelRenderOff(model);
    if (model->current.id.pokemon.wazaID != 0) menuModelWazaFree(model);
    do {
        if (model->object.model != model->nextObject.model) {
            menuModelFreeNext(model);
        }
        model->state = MENU_MODEL_STATE_LOADING;
        model->loadRequest = 0;
        model->current = model->next;

        if (model->current.isPokemon) {
            model->nextObject.sequence = Sequence_LoadPokemon(
                model->current.id.pokemon.pkxID, model->current.pokemonRandom,
                model->current.isRare);
            if (model->nextObject.sequence == NULL) {
                model->state = MENU_MODEL_STATE_NONE;
                return 0;
            }
            gsmodel = Sequence_GetGSmodel((u32)model->nextObject.sequence);
            if (gsmodel == NULL) {
                model->state = MENU_MODEL_STATE_NONE;
                return 0;
            }
            if (model->current.id.pokemon.wazaID != 0) {
                if ((model->nextWaza = Sequence_LoadWaza(
                         model->nextObject.sequence, model->current.id.pokemon.wazaID,
                         MENU_MODEL_WAZA_CATEGORY, 0)) != NULL) {
                    WazaSequence_Start(model->nextWaza, 0);
                }
            }
            Sequence_DoAnimation(model->nextObject.sequence,
                                 model->current.animIndex);
        } else {
            model->nextObject.model = floorOpenObject(model->current.id.objectID);
            if (model->nextObject.model == NULL) return 0;
            if (GSmodelCanAnimate(model->nextObject.model)) {
                GSmodelSetAnimIndex(model->nextObject.model, model->current.motionIndex);
                GSmodelSetAnimRate(model->nextObject.model, 0.5f);
                GSmodelStartAnimation(model->nextObject.model);
            }
            gsmodel = model->nextObject.model;
        }
        GSmodelSetVisibility(gsmodel, false);
        GSmodelSetAnimType(gsmodel, GS_MODEL_ANIM_LOOP);
        GSmodelSetRenderLayer(gsmodel, 1, &_layerID);
        GSmodelSetBoundCheck(gsmodel, false);
    } while (model->loadRequest == 1);

    menuModelFree(model);
    model->state = MENU_MODEL_STATE_LOADING;
    model->visibilityDelay = 0;
    model->object.model = model->nextObject.model;
    model->waza = model->nextWaza;
    _menuModelAdjustCamera(model, false);
    model->boundsValid = 0;
    model->nextObject.model = NULL;
    model->nextWaza = NULL;
    model->state = MENU_MODEL_STATE_READY;
    model->thread = NULL;
    return 1;
}

s32 menuModelSetSize(MENU_MODEL *model, s32 x, s32 y, s32 width, s32 height)
{
    if (model == NULL) return 0;
    model->x = x;
    model->y = y;
    model->width = width;
    model->height = height;
    return 1;
}

s32 menuModelInit(MENU_MODEL *model, s32 width, s32 height)
{
    GSvec pos(15.0f, 15.0f, 15.0f);
    GSvec rot(0.0f, 0.0f, 0.0f);
    GSvec target(0.0f, 0.0f, 0.0f);
    GXColor white = { 255, 255, 255, 255 };
    GXColor ambient = { 128, 128, 128, 255 };
    int i;
    GSlight *light;
    if (model == NULL) return 0;
    memset(model, 0, sizeof(MENU_MODEL));
    model->initialized = 1;
    model->renderEnable = 1;
    model->unknownFloat = 0.0f;
    if (_menuModelRef >= 4) {
        GSlogWrite("[%s] 参照回数上限オーバー\n", __FUNCTION__);
        return 0;
    }
    model->width = width;
    model->height = height;
    model->x = 0;
    model->y = 0;
    model->unknownFlag = 1;
    model->zoom = 1.0f;
    model->clip = 1;
    model->mAngle.z = model->mAngle.y = model->mAngle.x = 0.0f;
    model->mOffset.z = model->mOffset.y = model->mOffset.x = 0.0f;
    model->camera = GScameraCreate();
    if (model->camera == NULL) {
        GSlogWrite("[%s] カメラの確保に失敗\n", __FUNCTION__);
        return 0;
    }
    GScameraSetPosition(model->camera, &pos);
    GScameraSetRotation(model->camera, &rot);
    for (i = 0; i < 3; i++) {
        light = GSlightCreate();
        if (light != NULL) {
            switch (i) {
            case MENU_MODEL_LIGHT_AMBIENT:
                GSlightSetType(light, GSLIGHT_TYPE_AMBIENT);
                GSlightSetColor(light, ambient);
                break;
            case MENU_MODEL_LIGHT_INFINITE:
                GSlightSetType(light, GSLIGHT_TYPE_INFINITE);
                GSlightSetColor(light, white);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
                break;
            case MENU_MODEL_LIGHT_POINT:
                pos.z = pos.y = pos.x = 10.0f;
                GSlightSetType(light, GSLIGHT_TYPE_POINT);
                GSlightSetColor(light, white);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
                break;
            }
            GSlightSetActive(light, 0);
            model->light[i] = light;
        }
    }
    model->mCameraRotation.z = model->mCameraRotation.y = model->mCameraRotation.x = 0.0f;
    _menuModelRef++;
    return 1;
}

s32 _menuModelTerm(MENU_MODEL *model)
{
    int i;
    u8 hasNext;
    if (model == NULL) return 0;
    if (!model->initialized) return 0;
    if (_menuModelRef <= 0) {
        GSlogWrite("[%s] 必要以上に開放しようとしました\n", __FUNCTION__);
        return 0;
    }
    model->loadRequest = 0;
    model->initialized = 0;
    if (GSthreadGetCurrentThread() != NULL) {
        while (true) {
            if (model->thread == NULL) break;
            if (GSthreadIsRunning(model->thread) == false) {
                GSthreadClose(model->thread);
                break;
            }
            GSthreadSwitch();
        }
    } else if (model->thread != NULL) {
        GSthreadClose(model->thread);
        model->thread = NULL;
    }
    hasNext = 0;
    if (model->object.model != model->nextObject.model) hasNext = 1;
    menuModelFree(model);
    if (hasNext) menuModelFreeNext(model);
    for (i = 0; i < 3; i++) {
        if (model->light[i] != NULL) {
            GSlightFree(model->light[i]);
            model->light[i] = NULL;
        }
    }
    if (model->camera != NULL) {
        GScameraFree(model->camera);
        model->camera = NULL;
    }
    _menuModelRef--;
    return 1;
}

void menuModelTerm(MENU_MODEL *model)
{
    _menuModelTerm(model);
}

bool menuModelCompareObject(MENU_MODEL *model, u32 resourceID)
{
    if (model == NULL) return 0;
    MENU_MODEL_LOAD loadInfo;
    loadInfo.isPokemon = 0;
    loadInfo.id.objectID = resourceID;
    loadInfo.motionIndex = 0;
    if (model->loadRequest) {
        return _menuModelCmpLoadInfo(&model->next, &loadInfo);
    }
    return _menuModelCmpLoadInfo(&model->current, &loadInfo);
}

bool menuModelComparePokemon(MENU_MODEL *model, Pokemon *pokemon)
{
    if (model == NULL) return 0;
    MENU_MODEL_LOAD loadInfo;
    _menuModelMakeLoadInfo(&loadInfo, pokemon, 0, true);
    if (model->loadRequest) {
        return _menuModelCmpLoadInfo(&model->next, &loadInfo);
    }
    return _menuModelCmpLoadInfo(&model->current, &loadInfo);
}

s32 menuModelLoadObject(MENU_MODEL *model, u32 resourceID)
{
    if (model == NULL) return 0;
    if (menuModelCompareObject(model, resourceID)) return 0;
    switch (model->state) {
    case MENU_MODEL_STATE_NONE:
        model->state = MENU_MODEL_STATE_LOADING;
        break;
    case MENU_MODEL_STATE_LOADING:
    case MENU_MODEL_STATE_READY:
        break;
    }
    model->next.isPokemon = 0;
    model->next.id.objectID = resourceID;
    model->loadRequest = 1;
    if (model->thread != NULL) {
        if (GSthreadIsRunning(model->thread)) return 1;
        GSthreadClose(model->thread);
    }
    model->thread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 16384, 1, 1,
                                   (GSthreadEntry)_menuModelLoadThread);
    if (model->thread != NULL) GSthreadSetArgs(model->thread, 1, model);
    return 0;
}

s32 menuModelLoad(MENU_MODEL *model, Pokemon *pokemon)
{
    if (model == NULL) return 0;
    if (menuModelComparePokemon(model, pokemon)) return 0;
    switch (model->state) {
    case MENU_MODEL_STATE_NONE:
        model->state = MENU_MODEL_STATE_LOADING;
        break;
    case MENU_MODEL_STATE_LOADING:
    case MENU_MODEL_STATE_READY:
        break;
    }
    _menuModelMakeLoadInfo(&model->next, pokemon, 0, true);
    model->loadRequest = 1;
    if (model->thread != NULL) {
        if (GSthreadIsRunning(model->thread)) return 1;
        GSthreadClose(model->thread);
    }
    wazaSequenceSysSetVisibilityAll(false);
    model->thread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 16384, 1, 1,
                                   (GSthreadEntry)_menuModelLoadThread);
    if (model->thread != NULL) GSthreadSetArgs(model->thread, 1, model);
    return 1;
}

s32 menuModelLoadPokemon(MENU_MODEL *model, Pokemon *pokemon)
{
    if (model == NULL) return 0;
    if (menuModelComparePokemon(model, pokemon)) return 0;
    switch (model->state) {
    case MENU_MODEL_STATE_NONE:
        model->state = MENU_MODEL_STATE_LOADING;
        break;
    case MENU_MODEL_STATE_LOADING:
    case MENU_MODEL_STATE_READY:
        break;
    }
    _menuModelMakeLoadInfo(&model->next, pokemon, 0, false);
    model->loadRequest = 1;
    if (model->thread != NULL) {
        if (GSthreadIsRunning(model->thread)) return 1;
        GSthreadClose(model->thread);
    }
    wazaSequenceSysSetVisibilityAll(false);
    model->thread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 16384, 1, 1,
                                   (GSthreadEntry)_menuModelLoadThread);
    if (model->thread != NULL) GSthreadSetArgs(model->thread, 1, model);
    return 1;
}

s32 menuModelFree(MENU_MODEL *model)
{
    if (model == NULL) return 0;
    model->loadRequest = 0;
    if (model->current.isPokemon) {
        if (model->object.sequence != NULL) {
            Sequence_SetVisibility(model->object.sequence, false);
            Sequence_Free(model->object.sequence);
            model->object.sequence = NULL;
        }
    } else if (model->object.model != NULL) {
        GSmodelFree(model->object.model);
        model->object.model = NULL;
    }
    model->state = MENU_MODEL_STATE_NONE;
    return 1;
}

void menuModelWazaFree(MENU_MODEL *model)
{
    if (model != NULL && model->current.isPokemon && model->waza != NULL) {
        WazaSequence_Free(model->waza);
        model->waza = NULL;
    }
}

s32 menuModelFreeNext(MENU_MODEL *model)
{
    if (model == NULL) return 0;
    model->loadRequest = 0;
    if (model->next.isPokemon) {
        if (model->nextObject.sequence != NULL) {
            Sequence_SetVisibility(model->nextObject.sequence, false);
            Sequence_Free(model->nextObject.sequence);
            model->nextObject.sequence = NULL;
        }
    } else if (model->nextObject.model != NULL) {
        GSmodelFree(model->nextObject.model);
        model->nextObject.model = NULL;
    }
    model->state = MENU_MODEL_STATE_NONE;
    return 1;
}

bool menuModelCheck(MENU_MODEL *model, u8 wait)
{
    if (model == NULL) return false;
    if (wait) {
        while (true) {
            if (model->thread == NULL && model->state != MENU_MODEL_STATE_LOADING) break;
            GSthreadSwitch();
        }
        return false;
    }
    if (model->thread == NULL) return false;
    return model->state == MENU_MODEL_STATE_LOADING;
}

void menuModelRenderOffAll(MENU_MODEL *model)
{
    _menuModelRenderOff(model);
}

void _menuModelRenderOff(MENU_MODEL *model)
{
    if (model->current.isPokemon) {
        if (model->object.sequence != NULL) {
            Sequence_SetVisibility(model->object.sequence, false);
        }
        if (model->waza != NULL && wazaSequenceSysIsValidWaza(model->waza) == true &&
            WazaSequence_IsActive(model->waza) == true) {
            WazaSequence_Stop(model->waza);
        }
    } else if (model->object.model != NULL) {
        GSmodelSetVisibility(model->object.model, false);
    }
}

GStexture *menuModelRender(MENU_MODEL *model)
{
    _GSmodel *gsmodel;
    int i;
    static u8 _menuModelRenderDisabled;
    if (_menuModelRenderDisabled) return 0;
    if (!model->renderEnable) return 0;
    if (model == NULL) return 0;
    if (model->state != MENU_MODEL_STATE_READY) return 0;
    if (model->current.isPokemon) {
        if (model->object.sequence == NULL) return 0;
        gsmodel = Sequence_GetGSmodel((u32)model->object.sequence);
    } else {
        gsmodel = model->object.model;
    }
    if (gsmodel == NULL) return 0;
    if (model->current.isPokemon) Sequence_SetCamera(model->object.sequence, model->camera);
    else GSmodelSetRenderCamera(model->object.model, model->camera);
    _menuModelSetupViewport(model);
    for (i = 0; i < 3; i++) {
        if (model->light[i] != NULL) GSlightSetActive(model->light[i], _litonoff[i]);
    }
    if (model->current.isPokemon) {
        if (model->current.isPokemon && model->waza != NULL
            && wazaSequenceSysIsValidWaza(model->waza) == true
            && WazaSequence_IsActive(model->waza) == false) {
            WazaSequence_Start(model->waza, 0);
            Sequence_DoAnimation(model->object.sequence, model->current.animIndex);
        }
    }
    return 0;
}

s32 menuModelRender2(MENU_MODEL *model, MENU_MODEL *other)
{
    _GSmodel *gsmodel;
    _GSmodel *gsmodel2;
    int i;
    if (model == NULL) return 0;
    if (other == NULL) return 0;
    if (model->state != MENU_MODEL_STATE_READY) return 0;
    if (other->state != MENU_MODEL_STATE_READY) return 0;
    if (model->current.isPokemon) gsmodel = Sequence_GetGSmodel((u32)model->object.sequence);
    else gsmodel = model->object.model;
    if (other->current.isPokemon) gsmodel2 = Sequence_GetGSmodel((u32)other->object.sequence);
    else gsmodel2 = other->object.model;
    if (gsmodel == NULL) return 0;
    if (gsmodel2 == NULL) return 0;
    if (model->current.isPokemon) Sequence_SetCamera(model->object.sequence, model->camera);
    else GSmodelSetRenderCamera(model->object.model, model->camera);
    if (other->current.isPokemon) Sequence_SetCamera(other->object.sequence, model->camera);
    else GSmodelSetRenderCamera(other->object.model, model->camera);
    _menuModelSetupViewport2(model, other);
    for (i = 0; i < 3; i++) {
        if (model->light[i] != NULL) GSlightSetActive(model->light[i], _litonoff[i]);
    }
    return 0;
}

s32 menuModelSetMotion(MENU_MODEL *model, s32 motion)
{
    if (model == NULL) return 0;
    if (model->loadRequest) {
        if (model->next.isPokemon == 0) model->next.motionIndex = motion;
    } else if (model->current.isPokemon == 0) {
        model->current.motionIndex = motion;
        if (GSmodelCanAnimate(model->object.model)) {
            GSmodelSetAnimIndex(model->object.model, model->current.motionIndex);
            GSmodelSetAnimRate(model->object.model, 0.5f);
            GSmodelStartAnimation(model->object.model);
        }
    }
    return 1;
}

void menuModelAdjustCamera(MENU_MODEL *model)
{
    _menuModelAdjustCamera(model, true);
}

void menuModelAdjustCamera2(MENU_MODEL *model, MENU_MODEL *other)
{
    _menuModelAdjustCamera2(model, other);
}

void menuModelSetAngle(MENU_MODEL *model, f32 *angle)
{
    model->mAngle.x = angle[0];
    model->mAngle.y = angle[1];
    model->mAngle.z = angle[2];
}

void menuModelSetOffset(MENU_MODEL *model, f32 *offset)
{
    model->mOffset.x = offset[0];
    model->mOffset.y = offset[1];
    model->mOffset.z = offset[2];
}

void menuModelSetZoom(MENU_MODEL *model, f32 zoom)
{
    model->zoom = zoom;
}

void menuModelSetClip(MENU_MODEL *model, u8 clip)
{
    model->clip = clip;
}

_GSmodel *menuModelGetModelPtr(MENU_MODEL *model)
{
    if (model->current.isPokemon) {
        return Sequence_GetGSmodel((u32)model->object.sequence);
    }
    return model->object.model;
}

f32 *menuModelGetCameraRotation(MENU_MODEL *model)
{
    return &model->mCameraRotation.x;
}

void menuModelSetCameraRotation(MENU_MODEL *model, f32 *rotation)
{
    model->mCameraRotation.x = rotation[0];
    model->mCameraRotation.y = rotation[1];
    model->mCameraRotation.z = rotation[2];
}

u8 menuModelGetPokemonAnimationIndexByPointer(Pokemon *pokemon)
{
    return menuModelGetPokemonAnimationIndexByID(pokemon->getPokemonDataId());
}

u8 menuModelGetPokemonAnimationIndexByID(u16 species)
{
    u32 count = *pokemon_menu_animation_number;
    PokemonMenuAnimationEntry *entry = pokemon_menu_animation;
    for (u32 i = 0; i < count; i++) {
        if (entry->species == species) {
            return entry->animationIndex - 1;
        }
        entry++;
    }
    return 0;
}

f32 menuModelGetScale(u8 inputSelector, u16 inputID, f32 scale)
{
    return scale * _menuModelScaleMultiValue;
}

void menuModelSetScale(f32 scale)
{
    _menuModelScaleMultiValue = scale;
}

void menuModelReSetScale(void)
{
    _menuModelScaleMultiValue = 1.0f;
}

void menuModelGetPkxRndFromPokemonForSeqenceLoad(u32 &random,
                                                 u16 &pkxID,
                                                 Pokemon *pokemon)
{
    u16 speciesIndex = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
    pkxID = pokemonGetStatus(NULL, speciesIndex, POKEMON_QUERY_PKX_DATA_ID, 0);
    random = pokemon->getRnd();

    switch (pkxID) {
    case POKEMON_SPECIES_ANNON:
        random = pokemonGetAnnonKatati(random);
        break;
    case POKEMON_SPECIES_DEOXYS:
        random = 0;
        break;
    case POKEMON_SPECIES_RUGIA:
        if (pokemon->isDarkPokemon()) {
            pkxID = MENU_MODEL_PKX_DARK_LUGIA;
        }
        random = 0;
        break;
    }
}
