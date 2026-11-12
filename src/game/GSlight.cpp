#include <types.h>
#include <hsd/aobj.h>
#include <hsd/lobj.h>
#include <GSlight.h>
#include <GSmem.h>
#include <GSgfx.h>

GSlight *_lightStructList;
u32 _lightStructCount;
static f32 _frameCount;

void lightGetFrameCount(_HSD_AObj *aobj);


static inline void lightSetAnimationFrame(GSlight *light, f32 frame)
{
    if (light->mHasAnimation) {
        light->mAnimFrame = frame;
        HSD_ForeachAnim(light->mLObj, HSD_ANIM_LIGHT_OBJECT, HSD_ANIM_ALL,
                        (void (*)())HSD_AObjSetRate, HSD_ANIM_CALLBACK_FLOAT_ARG, 0.0);
        HSD_LObjReqAnim(light->mLObj, light->mAnimFrame);
        HSD_LObjAnim(light->mLObj);
        HSD_ForeachAnim(light->mLObj, HSD_ANIM_LIGHT_OBJECT, HSD_ANIM_ALL,
                        (void (*)())HSD_AObjSetRate, HSD_ANIM_CALLBACK_FLOAT_ARG, light->mAnimRate);
    }
}

static inline void lightSetAnimRate(GSlight *light, f32 rate)
{
    if (light->mHasAnimation) {
        if (GSgfxVideoGetVsyncRate() == 50) {
            rate *= 1.2f;
        }
        light->mAnimRate = rate;
        HSD_ForeachAnim(light->mLObj, HSD_ANIM_LIGHT_OBJECT, HSD_ANIM_ALL,
                        (void (*)())HSD_AObjSetRate, HSD_ANIM_CALLBACK_FLOAT_ARG, light->mAnimRate);
    }
}

static inline void lightSetAnimIndex(GSlight *light, unsigned int index)
{
    if (light->mHasAnimation) {
        HSD_LObjRemoveAnimAll(light->mLObj);
        if (index <= light->mAnimCount) {
            light->mAnimIndex = index;
            HSD_LObjAddAnimAll(light->mLObj,
                               light->mDesc->mAnimations[light->mAnimIndex]);
            HSD_LObjReqAnimAll(light->mLObj, 0.0f);
            _frameCount = 0.0f;
            HSD_ForeachAnim(light->mLObj, HSD_ANIM_LIGHT_OBJECT, HSD_ANIM_ALL,
                            (void (*)())lightGetFrameCount, HSD_ANIM_CALLBACK_NO_ARG);
            light->mAnimFrameCount = _frameCount;
            lightSetAnimationFrame(light, 0.0f);
        }
    }
}

static inline int lightGetType(_HSD_LObj *lobj)
{
    switch (HSD_LObjGetFlags(lobj) & HSD_LOBJ_TYPE_MASK) {
    case GSLIGHT_TYPE_AMBIENT:
        return GSLIGHT_TYPE_AMBIENT;
    case GSLIGHT_TYPE_INFINITE:
        return GSLIGHT_TYPE_INFINITE;
    case GSLIGHT_TYPE_POINT:
        return GSLIGHT_TYPE_POINT;
    case GSLIGHT_TYPE_SPOT:
        return GSLIGHT_TYPE_SPOT;
    default:
        return GSLIGHT_TYPE_INVALID;
    }
}

static inline int lightObjectUnref(_HSD_LObj *object)
{
    int noRef = object->referenceCount == (u16)-1;
    if (noRef) return noRef;
    return object->referenceCount-- == 0;
}

static inline void lightObjectRemove(_HSD_LObj *object)
{
    if (object && lightObjectUnref(object)) {
        if (object) {
            object->classInfo->release(object);
            object->classInfo->destroy(object);
        }
    }
}

static inline GSlight *lightFindFree(void)
{
    GSlight *light = _lightStructList;
    for (u32 i = 0; i < _lightStructCount; ++i, ++light) {
        if (light->mValid == 0) return light;
    }
    return 0;
}

extern "C" void GSlightDeleteCurrentAll(void)
{
    return HSD_LObjDeleteCurrentAll(0);
}

static inline int lightFindFirstActive(int index, GSlight *light)
{
    for (; index < _lightStructCount; ++light, ++index) {
        if (light->mValid == 1 && light->mActive != 0) {
            return index;
        }
    }
    return -1;
}

extern "C" void GSlightSetupLights(_HSD_CObj *camera)
{
    GSlight *light;
    int first;
    u32 i;

    HSD_LObjDeleteCurrentAll(0);
    first = lightFindFirstActive(0, _lightStructList);
    if (first != -1) {
        light = &_lightStructList[first];
        for (i = first + 1; i < _lightStructCount; ++i) {
            GSlight *next = &_lightStructList[i];
            if (next->mValid != 1) continue;
            if (next->mActive == 0) continue;
            light->mLObj->next = next->mLObj;
            light = next;
        }
        light->mLObj->next = 0;
        HSD_LObjAddCurrentAll(_lightStructList[first].mLObj);
    }
    HSD_LObjSetupInit(camera);
}

void _lightPostUpdate(f32 deltaTime)
{
    for (u32 i = 0; i < _lightStructCount; ++i) {
        GSlight *light = &_lightStructList[i];
        if (light->mValid == 1 && light->mAnimating == 1) {
            f32 lastFrame;
            f32 step;
            s32 type;
            step = light->mAnimRate * deltaTime;
            lastFrame = light->mAnimFrameCount - 1.0f;
            type = light->mAnimType;
            if (light->mAnimDirection == -1) {
                light->mAnimFrame -= step;
            } else if (light->mAnimDirection == 1) {
                light->mAnimFrame += step;
            }
            switch (type) {
            case GSLIGHT_ANIM_ONCE:
                if (light->mAnimFrame >= lastFrame - 0.1f) {
                    light->mAnimEnded = 1;
                    light->mAnimDirection = 0;
                    light->mAnimFrame = lastFrame - 0.1f;
                }
                break;
            case GSLIGHT_ANIM_LOOP:
                if (light->mAnimFrame >= lastFrame) {
                    light->mAnimFrame -= lastFrame;
                }
                break;
            case GSLIGHT_ANIM_PINGPONG:
                if (light->mAnimFrame >= lastFrame) {
                    light->mAnimDirection = -1;
                } else if (light->mAnimFrame <= 0.0f) {
                    light->mAnimDirection = 1;
                }
                break;
            }
        }
    }
}

void _lightPreUpdate(f32 deltaTime)
{
    #pragma unused(deltaTime)
    for (u32 i = 0; i < _lightStructCount; ++i) {
        GSlight *light = &_lightStructList[i];
        if (light->mValid == 1 && light->mAnimating == 1) {
            HSD_LObjReqAnimAll(light->mLObj, light->mAnimFrame);
            HSD_LObjAnimAll(light->mLObj);
        }
    }
}

extern "C" void GSlightPopState(GSlight *light, GSlightState *state)
{
    int type;

    state->mActive = light->mActive;
    HSD_LObjSetPosition(light->mLObj, (Vec *)&state->mPosition);
    type = lightGetType(light->mLObj);
    switch (type) {
    case GSLIGHT_TYPE_POINT:
    case GSLIGHT_TYPE_SPOT:
        HSD_LObjSetInterest(light->mLObj, (Vec *)&state->mInterest);
        break;
    }
    lightSetAnimIndex(light, state->mAnimIndex);
    lightSetAnimationFrame(light, state->mAnimFrame);
    lightSetAnimRate(light, state->mAnimRate);
    light->mAnimType = state->mAnimType;
    light->mAnimEnded = state->mAnimEnded;
    light->mAnimDirection = state->mAnimDirection;
    if (light->mAnimating && light->mHasAnimation) {
        light->mAnimating = 1;
        light->mAnimEnded = 0;
        light->mAnimDirection = 1;
    }
}

extern "C" void GSlightPushState(GSlight *light, GSlightState *state)
{
    state->mActive = light->mActive;
    state->mAnimating = light->mAnimating;
    HSD_LObjGetPosition(light->mLObj, (Vec *)&state->mPosition);
    HSD_LObjGetInterest(light->mLObj, (Vec *)&state->mInterest);
    state->mAnimIndex = light->mAnimIndex;
    state->mAnimFrame = light->mAnimFrame;
    state->mAnimRate = light->mAnimRate;
    state->mAnimType = light->mAnimType;
    state->mAnimEnded = light->mAnimEnded;
    state->mAnimDirection = light->mAnimDirection;
}

extern "C" u8 GSlightHasAnimationEnded(GSlight *light)
{
    return light->mAnimEnded;
}

extern "C" void GSlightStopAnimation(GSlight *light)
{
    light->mAnimating = 0;
}

extern "C" void GSlightStartAnimation(GSlight *light)
{
    if (light->mHasAnimation) {
        light->mAnimating = 1;
        light->mAnimEnded = 0;
        light->mAnimDirection = 1;
    }
}

extern "C" void GSlightSetAnimType(GSlight *light, int type)
{
    light->mAnimType = type;
}

extern "C" void GSlightSetAnimFrame(GSlight *light, f32 frame)
{
    lightSetAnimationFrame(light, frame);
}

extern "C" void GSlightSetAnimRate(GSlight *light, f32 rate)
{
    if (light->mHasAnimation) {
        if (GSgfxVideoGetVsyncRate() == 50) {
            rate *= 1.2f;
        }
        light->mAnimRate = rate;
        HSD_ForeachAnim(light->mLObj, HSD_ANIM_LIGHT_OBJECT, HSD_ANIM_ALL,
                        (void (*)())HSD_AObjSetRate, HSD_ANIM_CALLBACK_FLOAT_ARG, light->mAnimRate);
    }
}

extern "C" void GSlightSetAnimIndex(GSlight *light, unsigned int index)
{
    lightSetAnimIndex(light, index);
}

extern "C" u8 GSlightCanAnimate(GSlight *light)
{
    return light->mHasAnimation;
}

extern "C" u8 GSlightGetActive(GSlight *light)
{
    return light->mActive;
}

extern "C" void GSlightGetTarget(GSlight *light, GSvec *target)
{
    HSD_LObjGetInterest(light->mLObj, (Vec *)target);
}

extern "C" u8 GSlightGetPosition(GSlight *light, GSvec *position)
{
    return HSD_LObjGetPosition(light->mLObj, (Vec *)position) != 0;
}

extern "C" int GSlightGetType(GSlight *light)
{
    return lightGetType(light->mLObj);
}

extern "C" void GSlightGetColor(GSlight *light, GXColor *color)
{
    if (color) {
        HSD_LObjGetColor(light->mLObj, color);
    }
}

extern "C" void GSlightSetActive(GSlight *light, int active)
{
    light->mActive = active;
}

extern "C" void GSlightSetTarget(GSlight *light, GSvec *target)
{
    HSD_LObjSetInterest(light->mLObj, (Vec *)target);
}

extern "C" void GSlightSetPosition(GSlight *light, GSvec *position)
{
    HSD_LObjSetPosition(light->mLObj, (Vec *)position);
}

extern "C" void GSlightSetColor(GSlight *light, GXColor color)
{
    GXColor temp;
    temp.r = color.r;
    temp.g = color.g;
    temp.b = color.b;
    temp.a = color.a;
    HSD_LObjSetColor(light->mLObj, temp);
}

extern "C" void GSlightSetType(GSlight *light, int type)
{
    HSD_LObjClearFlags(light->mLObj, HSD_LOBJ_TYPE_MASK);
    switch (type) {
    case GSLIGHT_TYPE_AMBIENT:
        HSD_LObjSetFlags(light->mLObj, GSLIGHT_TYPE_AMBIENT);
        break;
    case GSLIGHT_TYPE_INFINITE:
        HSD_LObjSetFlags(light->mLObj, GSLIGHT_TYPE_INFINITE);
        break;
    case GSLIGHT_TYPE_POINT:
        HSD_LObjSetFlags(light->mLObj, GSLIGHT_TYPE_POINT);
        break;
    case GSLIGHT_TYPE_SPOT:
        HSD_LObjSetFlags(light->mLObj, GSLIGHT_TYPE_SPOT);
        break;
    }
    light->mType = type;
}

extern "C" void GSlightFree(GSlight *light)
{
    lightObjectRemove(light->mLObj);
    light->mActive = 0;
    light->mValid = 0;
}

extern "C" GSlight *GSlightLoad(void *lightData)
{
    GSlightDesc *desc = (GSlightDesc *)lightData;
    GSlight *light = lightFindFree();
    if (light == 0) return 0;
    memset(light, 0, sizeof(*light));
    light->mDesc = desc;
    light->mLObj = HSD_LObjLoadDesc(light->mDesc->mLight);
    light->mValid = 1;
    light->mActive = 0;
    light->mAnimating = 0;
    if (light->mDesc->mAnimations) {
        light->mHasAnimation = 1;
        light->mAnimRate = 1.0f;
        light->mAnimType = GSLIGHT_ANIM_LOOP;
        light->mAnimEnded = 0;
        light->mAnimCount = 0;
        while (light->mDesc->mAnimations[light->mAnimCount]) ++light->mAnimCount;
        lightSetAnimIndex(light, 0);
    } else {
        light->mHasAnimation = 0;
    }
    return light;
}

extern "C" GSlight *GSlightCreate(void)
{
    GSlight *light = lightFindFree();
    if (light == 0) return 0;
    memset(light, 0, sizeof(*light));
    light->mPositionDesc.className = 0;
    light->mPositionDesc.position.x = 0.0f;
    light->mPositionDesc.position.y = 0.0f;
    light->mPositionDesc.position.z = 0.0f;
    light->mPositionDesc.constraints = 0;
    light->mInterestDesc.className = 0;
    light->mInterestDesc.position.x = 0.0f;
    light->mInterestDesc.position.y = 0.0f;
    light->mInterestDesc.position.z = 0.0f;
    light->mInterestDesc.constraints = 0;
    light->mAttenuation = 16.0f;
    _HSD_LObjDesc *desc = &light->mLightDesc;
    desc->className = 0;
    desc->next = 0;
    desc->flags = HSD_LOBJ_DIFFUSE;
    desc->attenuationFlags = 0;
    desc->color.r = 128;
    desc->color.g = 128;
    desc->color.b = 128;
    desc->color.a = 0;
    desc->position = &light->mPositionDesc;
    desc->interest = &light->mInterestDesc;
    desc->attenuation = &light->mAttenuation;
    light->mLObj = HSD_LObjLoadDesc(desc);
    light->mValid = 1;
    light->mActive = 0;
    return light;
}

extern "C" void GSlightInit(u32 count)
{
    GSgfxRenderModule module = {0, _lightPreUpdate, 0, 0, _lightPostUpdate, 0,
                                GSGFX_RENDER_MODULE_SIGNATURE};
    _lightStructCount = count;
    _lightStructList = (GSlight *)GSmemAlloc(count * sizeof(GSlight));
    if (_lightStructList) {
        for (u32 i = 0; i < _lightStructCount; ++i) _lightStructList[i].mValid = 0;
        GSgfxRenderModuleRegister(4, &module, 0);
    }
}

void lightGetFrameCount(_HSD_AObj *aobj)
{
    _frameCount = 1.0f + HSD_AObjGetEndFrame(aobj);
}
