#include <new.h>
#include <types.h>
#include <GSlensFlare.h>
#include <GSvec.h>
#include <GSgfx.h>
#include <GStexture.h>
#include <GScamera.h>
#include <GScolsys2.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSFastCast.h>
#include "../../../ext/msl/include/math.h"

// Header of the asset a lens flare is generated from; its flare elements and
// textures are described by the tables that follow.
struct GSlensFlareData {
    u32 magic;
    u16 textureCount;
    u16 elementCount;
    u32 flags;
    u32 outerRadius;
    u32 fullStrengthRadius;
    u32 glareInnerRadius;
    f32 normalGlareStrength;
    f32 reflectedGlareStrength;
    f32 sizeParameter;
    f32 minimumScaleRadius;
    f32 sampleRadius;
};

struct GSlensFlareElement {
    u8 flags;
    u8 reserved;
    u16 textureSlot;
    f32 divisor;
    f32 baseSize;
    GScolor color;
};

struct GSlensFlareTexture {
    u16 slot;
    u16 reserved;
    union {
        u32 offset;
        GStexture *texture;
    };
};

// One lens flare effect: whether it is drawn, the sun it is generated around
// and the asset it was loaded from.
struct GSlensFlare {
    u8 visible;
    GSvec sunPosition;
    f32 intensity;
    GSlensFlareData *data;
    GSlensFlareElement *elements;
    GStexture *textures[64];

    GSlensFlare() : visible(0), sunPosition(0.0f, 0.0f, 0.0f), data(0), elements(0) {}
};

// Asset header magic of a loaded lens flare resource.
static const u32 LF_RESOURCE_SIGNATURE = 0x7B1EE3F2;

enum GSlensFlareFlags {
    LF_SCALE_RELATIVE_TO_SUN = 0x1,
    LF_ROTATE_ELEMENTS = 0x2,
    LF_REVERSE_ROTATION = 0x4,
    // Set when the flare layer follows the flare's visibility.
    LF_SYNC_LAYER = 0x8,
    LF_TEST_OCCLUSION = 0x10,
    LF_SAMPLE_OCCLUSION = 0x20
};

// Set once the flare module has registered with the renderer; the controls
// below do nothing before that.
u8 _lfInitialized;

// Loaded lens flares, indexed by handle - 1.
enum { LF_MAX_FLARES = 4 };
GSlensFlare *_lfLensFlarePtrs[LF_MAX_FLARES];

// Shows or hides one flare, and keeps the flare layer in step with it.
extern "C" void GSlensFlareSetVisible(u32 handle, bool visible)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    if (_lfLensFlarePtrs[index] == 0) {
        return;
    }
    if (visible == _lfLensFlarePtrs[index]->visible) {
        return;
    }
    _lfLensFlarePtrs[index]->visible = visible;
    if (_lfLensFlarePtrs[index]->data->flags & LF_SYNC_LAYER) {
        GSgfxLayer *layer = GSgfxLayerFindByID(GSGFX_LAYER_SECONDARY_DISTORTION);
        if (layer != 0) {
            layer->SetLayerVisibility(visible);
        }
    }
}

// Places the sun the flare is generated around.
extern "C" void GSlensFlareSetSunPosition(u32 handle, GSvec *position)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    GSlensFlare *flare = _lfLensFlarePtrs[index];
    if (flare == 0) {
        return;
    }
    flare->sunPosition.x = position->x;
    flare->sunPosition.y = position->y;
    flare->sunPosition.z = position->z;
}

// Releases one flare and drops it from the instance table.
extern "C" void GSlensFlareFree(u32 handle)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    if (_lfLensFlarePtrs[index] == 0) {
        return;
    }
    delete _lfLensFlarePtrs[index];
    _lfLensFlarePtrs[index] = 0;
}

extern "C" u32 GSlensFlareLoad(void *resource)
{
    if (!_lfInitialized)
        return 0;
    for (u32 slot = 0; slot < LF_MAX_FLARES; ++slot) {
        if (_lfLensFlarePtrs[slot] != 0)
            continue;
        GSlensFlare *flare = new GSlensFlare;
        if (flare == 0) {
            // Allocation failed; the slot stays free.
            flare = 0;
        } else {
            flare->data = (GSlensFlareData *)resource;
            if (flare->data->magic != LF_RESOURCE_SIGNATURE) {
                delete flare;
                flare = 0;
            } else {
                flare->elements = (GSlensFlareElement *)(flare->data + 1);
                s32 i;
                GSlensFlareTexture *record = (GSlensFlareTexture *)(flare->elements + flare->data->elementCount);
                for (i = 0; i < flare->data->textureCount; ++i) {
                    record->texture = (GStexture *)(record->offset + (u32)resource);
                    GStexture *texture = GStextureLoad(record->texture);
                    GStextureSetFilter(texture, GS_TEXTURE_FILTER_LINEAR, GS_TEXTURE_FILTER_LINEAR, GS_TEXTURE_MIP_FILTER_NONE);
                    if (!texture) {
                        delete flare;
                        flare = 0;
                        goto loaded;
                    }
                    flare->textures[record->slot] = texture;
                    ++record;
                }
                flare->intensity = 1.0f;
            }
        }
loaded:
        _lfLensFlarePtrs[slot] = flare;
        return _lfLensFlarePtrs[slot] ? slot + 1 : 0;
    }
    return 0;
}

void _lensFlareRender(GSgfxLayer *layer, GSgfxLayerPass pass);

static const u8 LF_RENDER_PRIORITY = 128;

extern "C" void GSlensFlareInit()
{
    memset(_lfLensFlarePtrs, 0, sizeof(_lfLensFlarePtrs));
    if (!_lfInitialized) {
        GSgfxRenderModule module = { 0, 0, _lensFlareRender, 0, 0, 0, GSGFX_RENDER_MODULE_SIGNATURE };
        GSgfxRenderModuleRegister(LF_RENDER_PRIORITY, &module, 0);
        _lfInitialized = 1;
    }
}

enum GSlensFlareElementFlags {
    LF_ELEMENT_SUN = 0x1,
    LF_ELEMENT_BLUE_CHANNEL = 0x2
};

static const f32 LF_SCREEN_WIDTH = 640.0f;
static const f32 LF_SCREEN_HEIGHT = 480.0f;
static const f32 LF_PROJECTION_DEPTH = 30000.0f;
static const f32 LF_SCALE_REFERENCE_RADIUS = 800.0f;

// Sample the sun and four rays along the camera up and right axes.
static const u16 LF_SAMPLES_PER_DIRECTION = 5;
static const u16 LF_CENTER_SAMPLE = 4 * LF_SAMPLES_PER_DIRECTION;
static const u16 LF_SAMPLE_COUNT = LF_CENTER_SAMPLE + 1;
static const f32 LF_CENTER_WEIGHT = 2.0f;

static inline GSvec LensScale(const GSvec &value, f32 scale)
{
    GSvec result;
    PSVECScale((const Vec *)&value, (Vec *)&result, scale);
    return result;
}
static inline GSvec LensAdd(const GSvec &a, const GSvec &b)
{
    GSvec result;
    PSVECAdd((const Vec *)&a, (const Vec *)&b, (Vec *)&result);
    return result;
}
static inline GSvec LensSubtract(const GSvec &a, const GSvec &b)
{
    GSvec result;
    PSVECSubtract((const Vec *)&a, (const Vec *)&b, (Vec *)&result);
    return result;
}
static inline f32 LensMagnitude(const GSvec &value)
{
    return PSVECMag((const Vec *)&value);
}
static inline f32 LensColorComponent(u8 component)
{
    f32 result;
    OSu8tof32(&component, &result);
    return result;
}
static inline u8 LensColorByte(f32 component)
{
    u8 result;
    OSf32tou8(&component, &result);
    return result;
}
static inline f32 LensSampleIndex(u16 index)
{
    f32 result;
    OSu16tof32(&index, &result);
    return result;
}
static inline void LensTranslate(GSgfxMatrixStack *stack, f32 x, f32 y, f32 z)
{
    PSMTXTransApply(stack->mCurrentEntry->mMatrix.sdk(), stack->mCurrentEntry->mMatrix.sdk(), x, y, z);
    stack->mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
}
static inline void LensRotate(GSgfxMatrixStack *stack, f32 x, f32 y, f32 z)
{
    GSmtx rotation;
    PSMTXRotRad(rotation.sdk(), 'X', x);
    PSMTXConcat(stack->mCurrentEntry->mMatrix.sdk(), rotation.sdk(), stack->mCurrentEntry->mMatrix.sdk());
    PSMTXRotRad(rotation.sdk(), 'Y', y);
    PSMTXConcat(stack->mCurrentEntry->mMatrix.sdk(), rotation.sdk(), stack->mCurrentEntry->mMatrix.sdk());
    PSMTXRotRad(rotation.sdk(), 'Z', z);
    PSMTXConcat(stack->mCurrentEntry->mMatrix.sdk(), rotation.sdk(), stack->mCurrentEntry->mMatrix.sdk());
    stack->mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
}

// Emits one corner of the screen-space glare quad together with its colour.
static inline void DrawGlareVertex(f32 x, f32 y, s32 color, const f32 &intensity, f32 visibility, f32 zero, f32 gain, f32 rounding)
{
    GXPosition3f32(x, y, zero);
    f32 alpha = gain * intensity;
    alpha = alpha * visibility + rounding;
    GXColor4u8(color, color, color, LensColorByte(alpha));
}

void _lensFlareRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    GSgfxLayerID layerID = layer->mLayerID;
    bool cameraReady = false;
    bool renderStateReady = false;
    GSvec projectedPosition, screenOffset, elementOffset, cameraUp, cameraRight, cameraPosition;
    f32 screenCenterX, screenCenterY;
    if (pass != GSGFX_LAYER_PASS_XLU ||
        (layerID != GSGFX_LAYER_FILTER && layerID != GSGFX_LAYER_SECONDARY_DISTORTION))
        return;
    for (u16 flareIndex = 0; flareIndex < LF_MAX_FLARES; ++flareIndex) {
        GSlensFlare *flare = _lfLensFlarePtrs[flareIndex];
        if (!flare || !flare->visible)
            continue;
        GSlensFlareData *data = flare->data;
        f32 left, top, right, size, strength, distance, sampleStep, visibility, sunSize, scaleRadius, sizeParameter;
        if (layer->mRenderMode == GSGFX_RENDER_MODE_BLUE_CHANNEL && !(data->flags & LF_SYNC_LAYER))
            continue;
        if (!cameraReady) {
            GScamera *camera = GScameraGetActiveCamera();
            if (!camera)
                return;
            GScameraLoadActiveCameraMatrix(GSGFX_LAYER_INVALID);
            f32 (*cameraMatrix)[4] = GScameraGetMatrixPtr(camera);
            GScameraGetPosition(camera, &cameraPosition);
            f32 upY = cameraMatrix[1][1], upZ = cameraMatrix[2][1];
            cameraUp.x = cameraMatrix[0][1];
            cameraUp.y = upY;
            cameraUp.z = upZ;
            screenCenterX = LF_SCREEN_WIDTH * 0.5f;
            screenCenterY = LF_SCREEN_HEIGHT * 0.5f;
            f32 rightY = cameraMatrix[1][0], rightZ = cameraMatrix[2][0];
            cameraRight.x = cameraMatrix[0][0];
            cameraRight.y = rightY;
            cameraRight.z = rightZ;
            GSgfxProjectSetupParameters();
            cameraReady = true;
        }
        GSgfxProjectResult projection = GSgfxProjectFast(&flare->sunPosition, &projectedPosition);
        if (projection == GSGFX_PROJECT_RESULT_UNAVAILABLE)
            continue;
        screenOffset.x = projectedPosition.x - screenCenterX;
        screenOffset.y = projectedPosition.y - screenCenterY;
        screenOffset.z = 0.0f;
        f32 radius = LensMagnitude(screenOffset);
        if (radius > (f32)data->outerRadius)
            continue;
        visibility = 1.0f;
        if (data->flags & LF_TEST_OCCLUSION) {
            if (data->flags & LF_SAMPLE_OCCLUSION) {
                GSvec samples[LF_SAMPLE_COUNT];
                bool blocked[LF_SAMPLE_COUNT];
                sampleStep = (1.0f / LF_SAMPLES_PER_DIRECTION) * data->sampleRadius;
                for (u16 sample = 0; sample < LF_SAMPLES_PER_DIRECTION; ++sample) {
                    distance = sampleStep * LensSampleIndex(sample);
                    samples[sample] = LensSubtract(flare->sunPosition, LensScale(cameraUp, distance));
                    samples[sample + LF_SAMPLES_PER_DIRECTION] = LensAdd(flare->sunPosition, LensScale(cameraUp, distance));
                    samples[sample + 2 * LF_SAMPLES_PER_DIRECTION] = LensSubtract(flare->sunPosition, LensScale(cameraRight, distance));
                    samples[sample + 3 * LF_SAMPLES_PER_DIRECTION] = LensAdd(flare->sunPosition, LensScale(cameraRight, distance));
                }
                samples[LF_CENTER_SAMPLE] = flare->sunPosition;
                if (GScolsys2SunArray(blocked, &cameraPosition, samples, LF_SAMPLE_COUNT) == true) {
                    visibility = blocked[LF_CENTER_SAMPLE] ? 0.0f : LF_CENTER_WEIGHT;
                    for (u16 sample = 0; sample < LF_CENTER_SAMPLE; ++sample)
                        visibility += blocked[sample] ? 0.0f : 1.0f;
                    visibility /= LF_CENTER_SAMPLE + LF_CENTER_WEIGHT;
                }
            } else if (GScolsys2Sun(&flare->sunPosition, &cameraPosition) == true) {
                continue;
            }
        }
        if (!renderStateReady) {
            GScameraLoadActiveCameraMatrix(layerID);
            f32 projectionMatrix[4][4];
            GSmtx44Ortho(projectionMatrix, 0.0f, LF_SCREEN_HEIGHT, 0.0f, LF_SCREEN_WIDTH, 0.0f, -LF_PROJECTION_DEPTH);
            layer->orthographicProjection(projectionMatrix);
            layer->mMatrixStack->identity();
            layer->projectionDepthRange(0.0f, LF_PROJECTION_DEPTH);
            layer->pixelMode(true, true, false);
            layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
            layer->cullMode(GSGFX_CULL_BACK);
            layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
            layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_ONE, true);
            renderStateReady = true;
        }
        if (projection == GSGFX_PROJECT_RESULT_SUCCESS) {
            sunSize = 1.0f;
            GSlensFlareElement *sunElement = flare->elements;
            s32 sunElementCount = data->elementCount;
            while (sunElementCount != 0) {
                --sunElementCount;
                if (sunElement->flags & LF_ELEMENT_SUN) {
                    sunSize = sunElement->baseSize;
                    break;
                }
                ++sunElement;
            }
            scaleRadius = radius;
            if (scaleRadius < data->minimumScaleRadius)
                scaleRadius = data->minimumScaleRadius;
            if (radius < (f32)data->fullStrengthRadius) {
                strength = 1.0f;
            } else {
                u32 fadeRange = data->outerRadius - data->fullStrengthRadius;
                strength = ((f32)fadeRange - (radius - (f32)data->fullStrengthRadius)) / (f32)fadeRange;
                if (strength < 0.0f) strength = 0.0f;
                else if (strength > 1.0f) strength = 1.0f;
            }
            u32 relativeScale = data->flags & LF_SCALE_RELATIVE_TO_SUN;
            sizeParameter = data->sizeParameter;
            layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR_TEXCOORD));
            GSlensFlareElement *element = flare->elements;
            s32 elementCount = data->elementCount;
            while (elementCount-- != 0) {
                if (layer->mRenderMode == GSGFX_RENDER_MODE_BLUE_CHANNEL && !(element->flags & LF_ELEMENT_BLUE_CHANNEL)) {
                    ++element;
                    continue;
                }
                GStexture *texture = flare->textures[element->textureSlot];
                layer->mMatrixStack->identity();
                size = element->baseSize;
                if (element->flags & LF_ELEMENT_SUN) {
                    LensTranslate(layer->mMatrixStack, projectedPosition.x, projectedPosition.y, 0.0f);
                } else {
                    PSVECScale((const Vec *)&screenOffset, (Vec *)&elementOffset, 1.0f / element->divisor);
                    if (relativeScale) {
                        f32 displacement;
                        if (element->divisor > 0.0f && element->divisor < 1.0f)
                            displacement = 1.0f / element->divisor - 1.0f;
                        else
                            displacement = 1.0f - 1.0f / element->divisor;
                        displacement *= LF_SCALE_REFERENCE_RADIUS / scaleRadius;
                        displacement *= sizeParameter / sunSize - 1.0f;
                        size *= 1.0f + displacement;
                    } else {
                        size *= 1.0f + ((sizeParameter - 1.0f) * LensMagnitude(elementOffset)) / LF_SCALE_REFERENCE_RADIUS;
                    }
                    LensTranslate(layer->mMatrixStack, elementOffset.x + screenCenterX, elementOffset.y + screenCenterY, 0.0f);
                }
                left = -(size * (GStextureGetXsize(texture) >> 1));
                top = -(size * (GStextureGetYsize(texture) >> 1));
                right = size * (GStextureGetXsize(texture) >> 1);
                f32 bottom = size * (GStextureGetYsize(texture) >> 1);
                if (data->flags & LF_ROTATE_ELEMENTS) {
                    if (data->flags & LF_REVERSE_ROTATION) {
                        f32 angle = (f32)atan2(screenOffset.y, screenOffset.x);
                        LensRotate(layer->mMatrixStack, 0.0f, 0.0f, -angle);
                    } else {
                        f32 angle = (f32)atan2(screenOffset.y, screenOffset.x);
                        LensRotate(layer->mMatrixStack, 0.0f, 0.0f, angle);
                    }
                }
                f32 redValue = strength * LensColorComponent(element->color.channels.r);
                u8 red = LensColorByte(redValue);
                f32 greenValue = strength * LensColorComponent(element->color.channels.g);
                u8 green = LensColorByte(greenValue);
                f32 blueValue = strength * LensColorComponent(element->color.channels.b);
                u8 blue = LensColorByte(blueValue);
                f32 alphaValue = visibility * (flare->intensity * (strength * LensColorComponent(element->color.channels.a))) + 0.5f;
                u8 alpha = LensColorByte(alphaValue);
                layer->texSelect(GX_TEXMAP0, texture);
                layer->beginSurface(4);
                f32 zero = 0.0f, one = 1.0f;
                GXPosition3f32(left, top, zero); GXColor4u8(red, green, blue, alpha); GXTexCoord2f32(zero, zero);
                GXPosition3f32(right, top, zero); GXColor4u8(red, green, blue, alpha); GXTexCoord2f32(one, zero);
                GXPosition3f32(left, bottom, zero); GXColor4u8(red, green, blue, alpha); GXTexCoord2f32(zero, one);
                GXPosition3f32(right, bottom, zero); GXColor4u8(red, green, blue, alpha); GXTexCoord2f32(one, one);
                layer->endSurface();
                ++element;
            }

            if (layer->mRenderMode != GSGFX_RENDER_MODE_NORMAL)
                continue;
            if (!(radius < (f32)data->fullStrengthRadius))
                continue;

            f32 glareStrength;
            if (radius < (f32)data->glareInnerRadius) {
                glareStrength = data->normalGlareStrength;
            } else {
                u32 glareRange = data->fullStrengthRadius - data->glareInnerRadius;
                glareStrength = data->normalGlareStrength * (((f32)glareRange - (radius - (f32)data->glareInnerRadius)) / (f32)glareRange);
                if (glareStrength < 0.0f) glareStrength = 0.0f;
                else if (glareStrength > 1.0f) glareStrength = 1.0f;
            }
            s32 glareColor = (s32)(255.0f * glareStrength);
            layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR));
            layer->mMatrixStack->identity();
            layer->beginSurface(4);
            f32 zero = 0.0f, gain = 255.0f, rounding = 0.5f, width = LF_SCREEN_WIDTH, height = LF_SCREEN_HEIGHT;
            DrawGlareVertex(zero, zero, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(width, zero, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(zero, height, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(width, height, glareColor, flare->intensity, visibility, zero, gain, rounding);
            s32 surfaces = 1;
            while (surfaces-- > 0)
                layer->endSurface();

        } else {
            if (layer->mRenderMode != GSGFX_RENDER_MODE_NORMAL)
                continue;
            PSVECSubtract((const Vec *)&cameraPosition, (const Vec *)&flare->sunPosition, (Vec *)&screenOffset);
            PSVECAdd((const Vec *)&cameraPosition, (const Vec *)&screenOffset, (Vec *)&screenOffset);
            if (GSgfxProjectFast(&screenOffset, &projectedPosition) != GSGFX_PROJECT_RESULT_SUCCESS)
                continue;
            screenOffset.x = projectedPosition.x - screenCenterX;
            screenOffset.y = projectedPosition.y - screenCenterY;
            screenOffset.z = 0.0f;
            f32 reflectedRadius = LensMagnitude(screenOffset);
            if (!(reflectedRadius > 0.00001f))
                continue;
            if (!(reflectedRadius < (f32)data->outerRadius))
                continue;
            if (!(reflectedRadius < (f32)data->fullStrengthRadius))
                continue;

            f32 glareStrength;
            if (reflectedRadius < (f32)data->glareInnerRadius) {
                glareStrength = data->reflectedGlareStrength;
            } else {
                u32 glareRange = data->fullStrengthRadius - data->glareInnerRadius;
                glareStrength = data->reflectedGlareStrength * (((f32)glareRange - (reflectedRadius - (f32)data->glareInnerRadius)) / (f32)glareRange);
                if (glareStrength < 0.0f) glareStrength = 0.0f;
                else if (glareStrength > 1.0f) glareStrength = 1.0f;
            }
            s32 glareColor = (s32)(255.0f * glareStrength);
            layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR));
            layer->mMatrixStack->identity();
            layer->beginSurface(4);
            f32 zero = 0.0f, gain = 255.0f, rounding = 0.5f, width = LF_SCREEN_WIDTH, height = LF_SCREEN_HEIGHT;
            DrawGlareVertex(zero, zero, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(width, zero, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(zero, height, glareColor, flare->intensity, visibility, zero, gain, rounding);
            DrawGlareVertex(width, height, glareColor, flare->intensity, visibility, zero, gain, rounding);
            s32 surfaces = 1;
            while (surfaces-- > 0)
                layer->endSurface();
        }
    }
}
