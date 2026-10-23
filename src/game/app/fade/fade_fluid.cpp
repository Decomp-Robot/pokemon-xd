#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXFifo.h>
#include <types.h>
#include <GSgfx.h>
#include <GSmath.h>
#include <GSsprite.h>
#include <GSmem.h>
#include <math.h>
#include <dolphin/os.h>

struct FadeFluidTexCoord {
    f32 u;
    f32 v;
};

struct FadeFluidWork {
    unsigned int dimX;
    unsigned int dimY;
    f32 centerWeight;
    f32 previousWeight;
    f32 neighbourWeight;
    f32 waveSpeed;
    f32 damping;
    f32 gridSpacing;
    f32 screenScaleX;
    f32 screenScaleY;
    GSvec *wave[2];
    GSvec *normal;
    GSvec *tangent;
    FadeFluidTexCoord *texCoord;
};

extern "C" {
FadeFluidWork _fadeFluidWork;
unsigned int _fadeFluidSw;

void fadeFluidDispSecond(GSgfxLayerID layerID, GStexture *texture, f32 fade, f32 time);
void fadeFluidDisp(GStexture *texture, f32 fade, f32 time);
void fadeFluidSetShockSecond(const GSvec *position, f32 power);
void fadeFluidEvaluateSecond(void);
void fadeFluidSetShock(const GSvec *position, f32 power);
void fadeFluidEvaluate(void);
void fadeFluidCalcParms(f32 timeStep);
void fadeFluidQuit(void);
void fadeFluidInit(unsigned int width, unsigned int height, f32 gridSpacing, f32 timeStep,
                   f32 waveSpeed, f32 damping);
}

void _fadeFluidSetShockSub(unsigned long x, unsigned long y, f32 power);
void _fadeFluidDoFree(void *memory);
void *_fadeFluidDoMalloc(unsigned long bytes);

static const GSgfxLayerID FADE_FLUID_LAYER = (GSgfxLayerID)7;

static inline void fadeFluidSetVector(GSvec *vector, f32 x, f32 y, f32 z)
{
    vector->x = x;
    vector->y = y;
    vector->z = z;
}

static inline void fadeFluidCopyVector(GSvec *to, const GSvec *from)
{
    to->x = from->x;
    to->y = from->y;
    to->z = from->z;
}

void fadeFluidDispSecond(GSgfxLayerID layerID, GStexture *texture, f32 fade, f32 time)
{
    int alpha;
    int y;
    GSgfxLayer *layer;
    layer = GSgfxLayerFindByID(layerID);
    GSvec color;
    GSvec opaque;
    GSvec transparent;
    opaque.x = 1.0f;
    opaque.y = 1.0f;
    opaque.z = 1.0f;
    transparent.x = 0.0f;
    transparent.y = 0.0f;
    transparent.z = 0.0f;
    GSlerpGetLinearInterpolationVector(&color, &opaque, &transparent, time);
    alpha = (int)(255 * color.x);
    alpha = (int)((f32)alpha * (1.0f - fade));

    spriteSetEnv(layerID);
    // Blended pass, alpha writes on, no depth or alpha test and no culling:
    // the fluid surface is drawn as a transparent overlay.
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->pixelMode(true, true, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_LESS);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND,
                        GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(
        GX_POS_XYZ, GX_F32, 0, 0,
        1, GX_RGBA8, 1, GX_TEX_ST,
        GX_F32));
    layer->texSelect(0, texture);

    unsigned int pitch = _fadeFluidWork.dimX + 1;
    f32 scaleX = _fadeFluidWork.screenScaleX;
    f32 scaleY = _fadeFluidWork.screenScaleY;
    for (y = 0; y < _fadeFluidWork.dimY; y++) {
        layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
        layer->beginSurface((u16)(pitch * 2));
        unsigned int row;
        GSvec **waveSlot;
        FadeFluidTexCoord *texCoord;
        unsigned int nextRow;
        unsigned int bottom;
        unsigned int x;
        waveSlot = &_fadeFluidWork.wave[_fadeFluidSw];
        row = y * pitch;
        nextRow = (y + 1) * pitch;
        texCoord = _fadeFluidWork.texCoord;
        for (x = 0; x < pitch; x++) {
            unsigned int top = x + row;
            bottom = x + nextRow;
            GSvec *position = &(*waveSlot)[top];
            FadeFluidTexCoord *uvTop = &texCoord[top];
            f32 px;
            f32 py;
            f32 pz;
            px = position->x;
            py = position->y;
            pz = position->z;
            px *= scaleX;
            py *= scaleY;
            pz *= scaleX;
            GXWGFifo.f32 = px;
            GXWGFifo.f32 = py;
            GXWGFifo.f32 = pz;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = (u8)alpha;
            f32 u = uvTop->u;
            f32 v = uvTop->v;
            GXWGFifo.f32 = u;
            GXWGFifo.f32 = v;
            position = &(*waveSlot)[bottom];
            px = position->x * scaleX;
            pz = position->z * scaleX;
            py = position->y * scaleY;
            GXWGFifo.f32 = px;
            GXWGFifo.f32 = py;
            GXWGFifo.f32 = pz;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = (u8)alpha;
            u = texCoord[bottom].u;
            v = texCoord[bottom].v;
            GXWGFifo.f32 = u;
            GXWGFifo.f32 = v;
        }
        layer->endSurface();
    }
}

void fadeFluidDisp(GStexture *texture, f32 fade, f32 time)
{
    int alpha;
    int y;
    GSgfxLayer *layer;
    layer = GSgfxLayerFindByID(FADE_FLUID_LAYER);
    GSvec color;
    GSvec opaque;
    GSvec transparent;
    opaque.x = 1.0f;
    opaque.y = 1.0f;
    opaque.z = 1.0f;
    transparent.x = 0.0f;
    transparent.y = 0.0f;
    transparent.z = 0.0f;
    GSlerpGetLinearInterpolationVector(&color, &opaque, &transparent, time);
    alpha = (int)(255 * color.x);
    alpha = (int)((f32)alpha * (1.0f - fade));

    spriteSetEnv(FADE_FLUID_LAYER);
    // Blended pass, alpha writes on, no depth or alpha test and no culling:
    // the fluid surface is drawn as a transparent overlay.
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->pixelMode(true, true, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_LESS);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND,
                        GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(
        GX_POS_XYZ, GX_F32, 0, 0,
        1, GX_RGBA8, 1, GX_TEX_ST,
        GX_F32));
    layer->texSelect(0, texture);

    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    unsigned int pitch = _fadeFluidWork.dimX + 1;
    f32 scaleX = _fadeFluidWork.screenScaleX;
    f32 scaleY = _fadeFluidWork.screenScaleY;
    for (y = 0; y < _fadeFluidWork.dimY; y++) {
        layer->beginSurface((u16)(pitch * 2));
        unsigned int row;
        GSvec **waveSlot;
        FadeFluidTexCoord *texCoord;
        unsigned int nextRow;
        unsigned int bottom;
        unsigned int x;
        waveSlot = &_fadeFluidWork.wave[_fadeFluidSw];
        row = y * pitch;
        nextRow = (y + 1) * pitch;
        texCoord = _fadeFluidWork.texCoord;
        for (x = 0; x < pitch; x++) {
            unsigned int top = x + row;
            bottom = x + nextRow;
            GSvec *position = &(*waveSlot)[top];
            FadeFluidTexCoord *uvTop = &texCoord[top];
            f32 px;
            f32 py;
            f32 pz;
            px = position->x;
            py = position->y;
            pz = position->z;
            px *= scaleX;
            py *= scaleY;
            pz *= scaleX;
            GXWGFifo.f32 = px;
            GXWGFifo.f32 = py;
            GXWGFifo.f32 = pz;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = (u8)alpha;
            f32 u = uvTop->u;
            f32 v = uvTop->v;
            GXWGFifo.f32 = u;
            GXWGFifo.f32 = v;
            position = &(*waveSlot)[bottom];
            px = position->x * scaleX;
            pz = position->z * scaleX;
            py = position->y * scaleY;
            GXWGFifo.f32 = px;
            GXWGFifo.f32 = py;
            GXWGFifo.f32 = pz;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = 255;
            GXWGFifo.u8 = (u8)alpha;
            u = texCoord[bottom].u;
            v = texCoord[bottom].v;
            GXWGFifo.f32 = u;
            GXWGFifo.f32 = v;
        }
        layer->endSurface();
    }
}

void fadeFluidSetShockSecond(const GSvec *position, f32 power)
{
    f32 spacing;
    f32 sidePower;
    f32 diagonalPower;
    spacing = _fadeFluidWork.gridSpacing;
    unsigned long x = (unsigned long)(position->x / spacing);
    unsigned long y = (unsigned long)(position->y / spacing);
    _fadeFluidSetShockSub(x, y, power);
    sidePower = 0.5f * power;
    diagonalPower = 0.25f * power;
    _fadeFluidSetShockSub(x - 1, y - 1, diagonalPower);
    _fadeFluidSetShockSub(x,     y - 1, sidePower);
    _fadeFluidSetShockSub(x + 1, y - 1, diagonalPower);
    _fadeFluidSetShockSub(x - 1, y,     sidePower);
    _fadeFluidSetShockSub(x + 1, y,     sidePower);
    _fadeFluidSetShockSub(x - 1, y + 1, diagonalPower);
    _fadeFluidSetShockSub(x,     y + 1, sidePower);
    _fadeFluidSetShockSub(x + 1, y + 1, diagonalPower);
}

void fadeFluidEvaluateSecond(void)
{
    GSvec *current;
    unsigned int pitch;
    unsigned int x;
    int y;
    pitch = _fadeFluidWork.dimX + 1;
    f32 centerWeight = _fadeFluidWork.centerWeight;
    f32 previousWeight = _fadeFluidWork.previousWeight;
    f32 neighbourWeight = _fadeFluidWork.neighbourWeight;
    for (y = 1; y < _fadeFluidWork.dimY; y++) {
        current = _fadeFluidWork.wave[_fadeFluidSw] + y * pitch;
        GSvec *previous = _fadeFluidWork.wave[1 - _fadeFluidSw] + y * pitch;
        for (x = 1; x < _fadeFluidWork.dimX; x++) {
            previous[x].z = centerWeight * current[x].z + previousWeight * previous[x].z
                + neighbourWeight * (current[x - pitch].z + (current[x + 1].z + current[x - 1].z + current[x + pitch].z));
        }
    }
    _fadeFluidSw = 1 - _fadeFluidSw;
}

void fadeFluidSetShock(const GSvec *position, f32 power)
{
    f32 spacing = _fadeFluidWork.gridSpacing;
    unsigned long x = (unsigned long)(position->x / spacing);
    unsigned long y = (unsigned long)(position->y / spacing);
    _fadeFluidSetShockSub(x, y, power);
    power *= 0.5f;
    _fadeFluidSetShockSub(x - 1, y - 1, power);
    _fadeFluidSetShockSub(x,     y - 1, power);
    _fadeFluidSetShockSub(x + 1, y - 1, power);
    _fadeFluidSetShockSub(x - 1, y,     power);
    _fadeFluidSetShockSub(x + 1, y,     power);
    _fadeFluidSetShockSub(x - 1, y + 1, power);
    _fadeFluidSetShockSub(x,     y + 1, power);
    _fadeFluidSetShockSub(x + 1, y + 1, power);
}

void _fadeFluidSetShockSub(unsigned long x, unsigned long y, f32 power)
{
    if (x > _fadeFluidWork.dimX) return;
    if (y > _fadeFluidWork.dimY) return;
    {
        unsigned int index = x + y * (_fadeFluidWork.dimX + 1);
        GSvec *point = &_fadeFluidWork.wave[_fadeFluidSw][index];
        point->z -= power;
    }
}

void fadeFluidEvaluate(void)
{
    GSvec *current;
    unsigned int pitch;
    unsigned int x;
    int y;
    pitch = _fadeFluidWork.dimX + 1;
    f32 centerWeight = _fadeFluidWork.centerWeight;
    f32 previousWeight = _fadeFluidWork.previousWeight;
    f32 neighbourWeight = _fadeFluidWork.neighbourWeight;
    for (y = 1; y < _fadeFluidWork.dimY; y++) {
        current = _fadeFluidWork.wave[_fadeFluidSw] + y * pitch;
        GSvec *previous = _fadeFluidWork.wave[1 - _fadeFluidSw] + y * pitch;
        for (x = 1; x < _fadeFluidWork.dimX; x++) {
            previous[x].z = centerWeight * current[x].z + previousWeight * previous[x].z
                + neighbourWeight * (current[x - pitch].z + (current[x + 1].z + current[x - 1].z + current[x + pitch].z));
        }
    }
    _fadeFluidSw = 1 - _fadeFluidSw;

    GSvec *normal;
    GSvec *tangent;
    unsigned int normalX;
    GSvec *normalCurrent;
    for (y = 1; y < _fadeFluidWork.dimY; y++) {
        normalCurrent = _fadeFluidWork.wave[_fadeFluidSw] + y * pitch;
        normal = _fadeFluidWork.normal + y * pitch;
        tangent = _fadeFluidWork.tangent + y * pitch;
        for (normalX = 1; normalX < _fadeFluidWork.dimX; normalX++) {
            normal[normalX].x = normalCurrent[normalX - 1].z - normalCurrent[normalX + 1].z;
            normal[normalX].y = normalCurrent[normalX - pitch].z - normalCurrent[normalX + pitch].z;
            tangent[normalX].z = normalCurrent[normalX + 1].z - normalCurrent[normalX - 1].z;
        }
    }
}

void fadeFluidCalcParms(f32 timeStep)
{
    f32 speedSquared = _fadeFluidWork.waveSpeed * _fadeFluidWork.waveSpeed;
    f32 spacingSquared = _fadeFluidWork.gridSpacing * _fadeFluidWork.gridSpacing;
    f32 propagation = timeStep * (timeStep * speedSquared);
    propagation /= spacingSquared;
    f32 dampTime = _fadeFluidWork.damping * timeStep;
    f32 reciprocal = 1.0f / (2.0f + dampTime);
    _fadeFluidWork.centerWeight = (4.0f - 8.0f * propagation) * reciprocal;
    _fadeFluidWork.previousWeight = reciprocal * (dampTime - 2.0f);
    _fadeFluidWork.neighbourWeight = (2.0f * propagation) * reciprocal;
}

void fadeFluidQuit(void)
{
    if (_fadeFluidWork.wave[0]) {
        _fadeFluidDoFree(_fadeFluidWork.wave[0]);
    }
    if (_fadeFluidWork.wave[1]) {
        _fadeFluidDoFree(_fadeFluidWork.wave[1]);
    }
    if (_fadeFluidWork.normal) {
        _fadeFluidDoFree(_fadeFluidWork.normal);
    }
    if (_fadeFluidWork.tangent) {
        _fadeFluidDoFree(_fadeFluidWork.tangent);
    }
    if (_fadeFluidWork.texCoord) {
        _fadeFluidDoFree(_fadeFluidWork.texCoord);
    }
    _fadeFluidWork.wave[0] = 0;
    _fadeFluidWork.wave[1] = 0;
    _fadeFluidWork.normal = 0;
    _fadeFluidWork.tangent = 0;
    _fadeFluidWork.texCoord = 0;
}

void fadeFluidInit(unsigned int width, unsigned int height, f32 gridSpacing, f32 timeStep,
                   f32 waveSpeed, f32 damping)
{
    unsigned int index;
    int x;
    int y;
    f32 normalSpacing;
    f32 py;
    f32 v;
    unsigned int vertexCount = (width + 1) * (height + 1);
    _fadeFluidWork.dimX = width;
    _fadeFluidWork.dimY = height;
    _fadeFluidWork.screenScaleX = 640.0f / (f32)width;
    _fadeFluidWork.screenScaleY = 480.0f / (f32)height;
    _fadeFluidWork.wave[0] = (GSvec *)_fadeFluidDoMalloc(vertexCount * sizeof(GSvec));
    _fadeFluidWork.wave[1] = (GSvec *)_fadeFluidDoMalloc(vertexCount * sizeof(GSvec));
    _fadeFluidSw = 0;
    _fadeFluidWork.normal = (GSvec *)_fadeFluidDoMalloc(vertexCount * sizeof(GSvec));
    _fadeFluidWork.tangent = (GSvec *)_fadeFluidDoMalloc(vertexCount * sizeof(GSvec));
    _fadeFluidWork.texCoord = (FadeFluidTexCoord *)_fadeFluidDoMalloc(vertexCount * sizeof(FadeFluidTexCoord));

    f32 speedLimit = (gridSpacing / (2.0f * timeStep)) * sqrtf(2.0f + damping);
    if (!(0.0f < waveSpeed && waveSpeed < speedLimit)) {
        OSReport("Warning: fadeFluid c parm. Limit Over!! -> 0.0f < %f < %f\n",
                 waveSpeed, speedLimit);
        waveSpeed = speedLimit - 0.000001f;
    }
    _fadeFluidWork.gridSpacing = gridSpacing;
    _fadeFluidWork.waveSpeed = waveSpeed;
    _fadeFluidWork.damping = damping;
    fadeFluidCalcParms(timeStep);

    normalSpacing = 2.0f * gridSpacing;
    f32 texScaleX = 1.0f / (f32)_fadeFluidWork.dimX;
    f32 texScaleY = 1.0f / (f32)_fadeFluidWork.dimY;
    index = 0;
    for (y = 0; (unsigned int)y <= height; y++) {
        py = gridSpacing * (f32)y;
        v = texScaleY * (f32)y;
        for (x = 0; (unsigned int)x <= width; index++, x++) {
            fadeFluidSetVector(&_fadeFluidWork.wave[0][index], gridSpacing * (f32)x, py, 0.0f);
            fadeFluidCopyVector(&_fadeFluidWork.wave[1][index], &_fadeFluidWork.wave[0][index]);
            fadeFluidSetVector(&_fadeFluidWork.normal[index], 0.0f, 0.0f, normalSpacing);
            fadeFluidSetVector(&_fadeFluidWork.tangent[index], normalSpacing, 0.0f, 0.0f);
            _fadeFluidWork.texCoord[index].u = texScaleX * (f32)x;
            _fadeFluidWork.texCoord[index].v = v;
        }
    }
}

void _fadeFluidDoFree(void *memory)
{
    GSmemFree(memory);
}

void *_fadeFluidDoMalloc(unsigned long bytes)
{
    void *memory;
    long previousMethod = GSmemSetAllocMethodHeap(GSmemGetCurrentHeap(),
                                                3);
    memory = GSmemAllocAlign(bytes, 32);
    (void)GSmemSetAllocMethodHeap(GSmemGetCurrentHeap(), previousMethod);
    return memory;
}
