#include <dolphin.h>
#include <dolphin/os.h>
#include <dolphin/gx.h>
#include <dolphin/exi.h>

#include "__os.h"

typedef struct OSFatalParam {
    GXColor fg;
    GXColor bg;
    const char* msg;
} OSFatalParam;

static OSFatalParam FatalParam;
static OSContext FatalContext;

enum {
    VI_PROGRESSIVE_CLOCK_SEL = 1
};

enum {
    EXI_CHANNEL_ZERO_CONTROL_REGISTER = 3,
    EXI_CONTROL_TRANSFER_START = 1,
};

// prototypes
static void Halt();

static void ScreenClear(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv) {
    int i;
    int j;
    u8* ptr;

    ptr = xfb;
    for (i = 0; i < xfbH; i++) {
        for (j = 0; j < xfbW; j += 2) {
            *ptr++ = yuv.r;
            *ptr++ = yuv.g;
            *ptr++ = yuv.r;
            *ptr++ = yuv.b;
        }
    }
}

static void ScreenReport(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv, s32 x, s32 y, s32 leading, const char* string) {
    u8* ptr;
    s32 width;
    u32 i;
    u32 j;
    u32 image[72];
    u32 k;
    u32 l;
    u8 Y;
    u32 pixel;
    s32 col;

next_line:
    if (xfbH - 24 >= y) {
        ptr = (u8*)xfb + ((x + (y * xfbW)) * 2);
        col = x;

        while (*string != 0) {
            if (*string == '\n') {
                string++;
                y += leading;
                goto next_line;
            }

            if (xfbW - 48 < col) {
                y += leading;
                goto next_line;
            }

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);
                image[j + 0] = 0;
                image[j + 8] = 0;
                image[j + 16] = 0;
            }

            string = OSGetFontTexel(string, image, 0, 6, &width);

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);

                for (k = 0; k < 24; k++) {
                    l = j + (k & 0xFFFFFFF8);

                    Y = (image[l] >> ((7 - (k & 7)) * 4)) & 0xF;
                    if (Y != 0) {
                        Y = (((yuv.r * (Y * 239)) / 255) / 15) + 16;
                        pixel = k + (i * xfbW);
                        ptr[pixel * 2] = Y;

                        if ((col + k) & 1) {
                            ptr[(pixel * 2) - 1] = yuv.g;
                            ptr[(pixel * 2) + 1] = yuv.b;
                        } else {
                            ptr[(pixel * 2) - 1] = yuv.b;
                            ptr[(pixel * 2) + 1] = yuv.g;
                        }
                    }
                }
            }

            ptr += width * 2;
            col += width;
        }
    }
}

static void ConfigureVideo(u16 xfbW, u16 xfbH) {
    GXRenderModeObj mode;
    mode.fbWidth = xfbW;
    mode.efbHeight = 480;
    mode.xfbHeight = xfbH;
    mode.viXOrigin = 40;
    mode.viWidth = 640;
    mode.viHeight = xfbH;

    switch (VIGetTvFormat()) {
    case VI_MPAL:
    case VI_NTSC:
        if (__VIRegs[VI_CLOCK_SEL] & VI_PROGRESSIVE_CLOCK_SEL) {
            mode.viTVmode = VI_TVMODE_NTSC_PROG;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_SF;
        } else {
            mode.viTVmode = VI_TVMODE_NTSC_INT;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_DF;
        }
        break;
    case VI_EURGB60:
        mode.viTVmode = VI_TVMODE_EURGB60_INT;
        mode.viYOrigin = 0;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    case VI_PAL:
        mode.viTVmode = VI_TVMODE_PAL_INT;
        mode.viYOrigin = 47;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    }

    VIConfigure(&mode);
    VIConfigurePan(0, 0, 640, 480);
}

static GXColor RGB2YUV(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y = 0.257f * rgb.r + 0.504f * rgb.g + 0.098f * rgb.b + 16.0f + 0.5f;
    Cb = -0.148f * rgb.r - 0.291f * rgb.g + 0.439f * rgb.b + 128.0f + 0.5f;
    Cr = 0.439f * rgb.r - 0.368f * rgb.g - 0.071f * rgb.b + 128.0f + 0.5f;

    yuv.r = (Y > 235.0f) ? 235.0f : (Y < 16.0f) ? 16.0f : Y;
    yuv.g = (Cb > 240.0f) ? 240.0f : (Cb < 16.0f) ? 16.0f : Cb;
    yuv.b = (Cr > 240.0f) ? 240.0f : (Cr < 16.0f) ? 16.0f : Cr;
    yuv.a = 0;

    return yuv;
}

void OSFatal(GXColor fg, GXColor bg, const char* msg) {
    OSBootInfo* bootInfo;
    u32 count;
    OSTime t;

    bootInfo = (OSBootInfo*)OSPhysicalToCached(0);
    OSDisableInterrupts();
    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    VIInit();
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_VI);
    VISetBlack(TRUE);
    VIFlush();
    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);
    OSEnableInterrupts();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 1);

    t = OSGetTime();
    while (!__OSCallResetFunctions(FALSE) && OSGetTime() - t < OSMillisecondsToTicks(1000)) {}

    OSDisableInterrupts();
    __OSCallResetFunctions(TRUE);
    EXISetExiCallback(0, NULL);
    EXISetExiCallback(2, NULL);

    while (!EXILock(0, 1, NULL)) {
        EXISync(0);
        EXIDeselect(0);
        EXIUnlock(0);
    }
    EXIUnlock(0);

    do {} while ((__EXIRegs[EXI_CHANNEL_ZERO_CONTROL_REGISTER] & EXI_CONTROL_TRANSFER_START) ==
                 EXI_CONTROL_TRANSFER_START);

    __OSSetExceptionHandler(__OS_EXCEPTION_DECREMENTER, &OSDefaultExceptionHandler);
    GXAbortFrame();
    OSSetArenaLo((void*)0x81400000);
    OSSetArenaHi(bootInfo->FSTLocation);

    FatalParam.fg = fg;
    FatalParam.bg = bg;
    FatalParam.msg = msg;
    OSSwitchFiber((u32)&Halt, (u32)OSGetArenaHi());
}

static void Halt() {
    u32 count;
    OSFontHeader* fontData;
    void* xfb;
    u32 len;
    OSFatalParam* fp;

    OSEnableInterrupts();
    fp = &FatalParam;
    len = strlen(fp->msg) + 1;
    fp->msg = memmove(OSAllocFromArenaLo(len, DOLPHIN_ALIGNMENT), fp->msg, len);

    fontData = OSAllocFromArenaLo(659460, DOLPHIN_ALIGNMENT);
    OSLoadFont(fontData, OSGetArenaLo());
    
    xfb = OSAllocFromArenaLo(614400, DOLPHIN_ALIGNMENT);
    ScreenClear(xfb, 640, 480, RGB2YUV(fp->bg));
    VISetNextFrameBuffer(xfb);
    ConfigureVideo(640, 480);
    VIFlush();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 2);

    ScreenReport(xfb, 640, 480, RGB2YUV(fp->fg), 48, 100, fontData->leading, fp->msg);
    DCFlushRange(xfb, 614400);
    VISetBlack(FALSE);
    VIFlush();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 1);

    OSDisableInterrupts();
    OSReport("%s\n", fp->msg);
    PPCHalt();
}
