#include <GSgfx.h>

enum { GSGFX_BACKFB_CAPTURE_COUNT = 4 };

typedef u8 (*GSgfxBackFBCallback)(GStexture *, u32, void *);

struct GSgfxBackFBCapture {
    u8 mActive;
    GStexture *mTexture;
    GSgfxBackFBCallback mCallback;
    void *mParam;
    u32 mFrameCount;
};

GSgfxBackFBCapture gfxBackFBList[GSGFX_BACKFB_CAPTURE_COUNT];
u8 gfxBackFBActive;

static inline GSgfxBackFBCapture *GSgfxBackFBFindCapture(GStexture *texture)
{
    GSgfxBackFBCapture *capture = gfxBackFBList;
    for (u32 i = 0; i < GSGFX_BACKFB_CAPTURE_COUNT; i++, capture++) {
        if (capture->mActive == 1 && capture->mTexture == texture) {
            return capture;
        }
    }
    return 0;
}

static inline GSgfxBackFBCapture *GSgfxBackFBFindFree(void)
{
    GSgfxBackFBCapture *capture = gfxBackFBList;
    for (u32 i = 0; i < GSGFX_BACKFB_CAPTURE_COUNT; i++, capture++) {
        if (capture->mActive == 0) {
            return capture;
        }
    }
    return 0;
}

static inline void GSgfxBackFBUpdateActive(void)
{
    gfxBackFBActive = 0;
    for (u32 i = 0; i < GSGFX_BACKFB_CAPTURE_COUNT; i++) {
        if (gfxBackFBList[i].mActive == 1) {
            gfxBackFBActive = 1;
            break;
        }
    }
}

extern "C" s32 GSgfxEndBackFBCapture(GStexture *texture)
{
    GSgfxBackFBCapture *capture = GSgfxBackFBFindCapture(texture);
    if (capture == 0) return 0;

    GStextureUnlockImage(capture->mTexture);
    capture->mActive = 0;
    GSgfxBackFBUpdateActive();
    return 1;
}

extern "C" s32 GSgfxBeginBackFBCapture(GStexture *texture, GSgfxBackFBCallback callback, void *param)
{
    switch (GStextureGetFormat(texture)) {
    case GS_TEXTURE_FORMAT_I4:
    case GS_TEXTURE_FORMAT_IA4:
    case GS_TEXTURE_FORMAT_I8:
    case GS_TEXTURE_FORMAT_IA8:
    case GS_TEXTURE_FORMAT_RGB565:
    case GS_TEXTURE_FORMAT_32BIT:
    case GS_TEXTURE_FORMAT_RGB5A3:
        break;
    default:
        return 0;
    }

    if (GSgfxBackFBFindCapture(texture) != 0) {
        return 0;
    }

    GSgfxBackFBCapture *capture = GSgfxBackFBFindFree();
    if (capture == 0) {
        return 0;
    }

    capture->mActive = 1;
    capture->mTexture = texture;
    capture->mCallback = callback;
    capture->mParam = param;
    capture->mFrameCount = 0;
    GStextureSetWrap(capture->mTexture, GS_TEXTURE_WRAP_CLAMP, GS_TEXTURE_WRAP_CLAMP);
    GStextureSetFilter(capture->mTexture, GS_TEXTURE_FILTER_LINEAR, GS_TEXTURE_FILTER_LINEAR, GS_TEXTURE_MIP_FILTER_NONE);
    GStextureLockImage(capture->mTexture, 0);
    gfxBackFBActive = 1;
    return 1;
}

void GSgfxBackFBDoFrame(GSgfxLayer *layer)
{
    if (gfxBackFBActive == 0) return;

    u32 pass0Size;
    u32 pass1Size;
    GSgfxBackFBCapture *capture;
    u32 i;
    GSgfxLayerPass pass;

    layer->GetFifoSize(&pass0Size, &pass1Size);
    if (pass1Size != 0) pass = GSGFX_LAYER_PASS_XLU;
    else pass = GSGFX_LAYER_PASS_OPAQUE;
    capture = gfxBackFBList;
    for (i = 0; i < GSGFX_BACKFB_CAPTURE_COUNT; i++, capture++) {
        if (capture->mActive != 1) continue;

        layer->textureCopy(pass, capture->mTexture, true, 0, false, false, false);
        if (capture->mCallback != 0) {
            if (capture->mCallback(capture->mTexture, capture->mFrameCount, capture->mParam) == 0) {
                GSgfxEndBackFBCapture(capture->mTexture);
            }
        }
        capture->mFrameCount++;
    }
}

void GSgfxBackFBInit(void)
{
    for (u32 i = 0; i < GSGFX_BACKFB_CAPTURE_COUNT; i++) {
        gfxBackFBList[i].mActive = 0;
    }
    gfxBackFBActive = 0;
}
