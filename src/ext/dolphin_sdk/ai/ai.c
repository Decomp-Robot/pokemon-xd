#include <dolphin/ai.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/hw_regs.h>

#include "__gx.h"

#define AI_REG_CONTROL 0
#define AI_REG_VOLUME 1
#define AI_REG_SAMPLE_COUNTER 2
#define AI_REG_STREAM_TRIGGER 3

#define AI_CONTROL_STREAM_INTERRUPT 0x0008
#define AI_CONTROL_STREAM_PLAY 0x0001
#define AI_CONTROL_SAMPLE_COUNT_RESET 0x0020
#define AI_CONTROL_DSP_RATE_32KHZ 0x0040
#define AI_CONTROL_DSP_RATE_MASK AI_CONTROL_DSP_RATE_32KHZ
#define DSP_STATUS_AI_INTERRUPT   0x0008
#define DSP_STATUS_ARAM_INTERRUPT 0x0020
#define DSP_STATUS_DSP_INTERRUPT 0x0080

#ifdef DEBUG
const char* __AIVersion = "<< Dolphin SDK - AI\tdebug build: Apr  5 2004 03:56:18 (0x2301) >>";
#else
const char* __AIVersion = "<< Dolphin SDK - AI\trelease build: Apr  5 2004 04:15:02 (0x2301) >>";
#endif

static AISCallback __AIS_Callback;
static AIDCallback __AID_Callback;
static u8* __CallbackStack;
static u8* __OldStack;
static BOOL __AI_init_flag;
static BOOL __AID_Active;
static OSTime bound_32KHz;
static OSTime bound_48KHz;
static OSTime min_wait;
static OSTime max_wait;
static OSTime buffer;

static void __AI_set_stream_sample_rate(u32 rate);
static void __AIDHandler(__OSInterrupt interrupt, OSContext* context);
static void __AISHandler(__OSInterrupt interrupt, OSContext* context);
static void __AICallbackStackSwitch(void* cb);
static void __AI_SRC_INIT(void);

AIDCallback AIRegisterDMACallback(AIDCallback callback) {
    AIDCallback old_callback;
    BOOL old;

    old_callback = __AID_Callback;
    old = OSDisableInterrupts();
    __AID_Callback = callback;
    OSRestoreInterrupts(old);
    return old_callback;
}

void AIInitDMA(u32 start_addr, u32 length) {
    BOOL old;

    old = OSDisableInterrupts();
    __DSPRegs[DSP_DMA_START_HI] = (__DSPRegs[DSP_DMA_START_HI] & 0xFFFFFC00) | (start_addr >> 16);
    __DSPRegs[DSP_DMA_START_LO] = (__DSPRegs[DSP_DMA_START_LO] & 0xFFFF001F) | (start_addr & 0xFFFF);
    ASSERTMSGLINE(316, (length & 0x1F) == 0, "AIStartDMA: length must be multiple of 32 bytes");
    __DSPRegs[DSP_DMA_CONTROL_LEN] = (__DSPRegs[DSP_DMA_CONTROL_LEN] & 0xFFFF8000) | ((length >> 5) & 0xFFFF);
    OSRestoreInterrupts(old);
}


void AIStartDMA(void) {
    __DSPRegs[DSP_DMA_CONTROL_LEN] = __DSPRegs[DSP_DMA_CONTROL_LEN] | DSP_DMA_START_FLAG;
}

void AIStopDMA(void) {
    __DSPRegs[DSP_DMA_CONTROL_LEN] = __DSPRegs[DSP_DMA_CONTROL_LEN] & ~DSP_DMA_START_FLAG;
}


void AIResetStreamSampleCount(void) {
    __AIRegs[AI_REG_CONTROL] = (__AIRegs[AI_REG_CONTROL] & ~AI_CONTROL_SAMPLE_COUNT_RESET) | AI_CONTROL_SAMPLE_COUNT_RESET;
}

void AISetStreamTrigger(u32 trigger) {
    __AIRegs[AI_REG_STREAM_TRIGGER] = trigger;
}


void AISetStreamPlayState(u32 state) {
    BOOL old;
    u8 vol_left;
    u8 vol_right;

    if (state != AIGetStreamPlayState()) {
        if (AIGetStreamSampleRate() == AI_SAMPLERATE_32KHZ && state == AI_STREAM_START) {
            vol_left = AIGetStreamVolRight();
            vol_right = AIGetStreamVolLeft();
            AISetStreamVolRight(0);
            AISetStreamVolLeft(0);
            old = OSDisableInterrupts();
            __AI_SRC_INIT();
            MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 5, 1);
            MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 0, AI_STREAM_START);
            OSRestoreInterrupts(old);
            AISetStreamVolLeft(vol_left);
            AISetStreamVolRight(vol_right);
            return;
        }
        MASKED_SET_REG_FIELD(653, __AIRegs[AI_REG_CONTROL], 1, 0, state);
    }
}

u32 AIGetStreamPlayState(void) {
    return __AIRegs[AI_REG_CONTROL] & AI_CONTROL_STREAM_PLAY;
}

void AISetDSPSampleRate(u32 rate) {
    BOOL old;
    u32 play_state;
    u32 afr_state;
    u8 vol_left;
    u8 vol_right;

    if (rate != AIGetDSPSampleRate()) {
        __AIRegs[AI_REG_CONTROL] &= ~AI_CONTROL_DSP_RATE_MASK;
        if (rate == AI_SAMPLERATE_32KHZ) {
            vol_left = AIGetStreamVolLeft();
            vol_right = AIGetStreamVolRight();
            play_state = AIGetStreamPlayState();
            afr_state = AIGetStreamSampleRate();
            AISetStreamVolLeft(0U);
            AISetStreamVolRight(0U);
            old = OSDisableInterrupts();
            __AI_SRC_INIT();
            MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 5, 1);
            MASKED_SET_REG_FIELD(743, __AIRegs[AI_REG_CONTROL], 1, 1, afr_state);
            MASKED_SET_REG_FIELD(744, __AIRegs[AI_REG_CONTROL], 1, 0, play_state);
            __AIRegs[AI_REG_CONTROL] |= AI_CONTROL_DSP_RATE_32KHZ;
            OSRestoreInterrupts(old);
            AISetStreamVolLeft(vol_left);
            AISetStreamVolRight(vol_right);
        }
    }
}

u32 AIGetDSPSampleRate(void) {
    return GET_REG_FIELD(__AIRegs[AI_REG_CONTROL], 1, 6) ^ 1;
}


static void __AI_set_stream_sample_rate(u32 rate) {
    BOOL old;
    u32 play_state;
    u8 vol_left;
    u8 vol_right;
    u32 dsp_src_state;

    if (rate != AIGetStreamSampleRate()) {
        play_state = AIGetStreamPlayState();
        vol_left = AIGetStreamVolLeft();
        vol_right = AIGetStreamVolRight();
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        dsp_src_state = __AIRegs[AI_REG_CONTROL] & AI_CONTROL_DSP_RATE_MASK;
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 6, 0);
        old = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[AI_REG_CONTROL] |= dsp_src_state;
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 5, 1);
        MASKED_SET_REG_FIELD(887, __AIRegs[AI_REG_CONTROL], 1, 1, rate);
        OSRestoreInterrupts(old);
        AISetStreamPlayState(play_state);
        AISetStreamVolLeft(vol_left);
        AISetStreamVolRight(vol_right);
    }
}

u32 AIGetStreamSampleRate(void) {
    return GET_REG_FIELD(__AIRegs[AI_REG_CONTROL], 1, 1);
}

void AISetStreamVolLeft(u8 vol) {
    MASKED_SET_REG_FIELD(945, __AIRegs[AI_REG_VOLUME], 8, 0, vol);
}

u8 AIGetStreamVolLeft(void) {
    return GET_REG_FIELD(__AIRegs[AI_REG_VOLUME], 8, 0);
}

void AISetStreamVolRight(u8 vol) {
    MASKED_SET_REG_FIELD(986, __AIRegs[AI_REG_VOLUME], 8, 8, vol);
}

u8 AIGetStreamVolRight(void)
{
    return (__AIRegs[AI_REG_VOLUME] & (0xFF << 8)) >> 8;
}

void AIInit(u8* stack) {
    if (__AI_init_flag != TRUE) {
        OSRegisterVersion(__AIVersion);

        bound_32KHz = OSNanosecondsToTicks(31524);
        bound_48KHz = OSNanosecondsToTicks(42024);
        min_wait = OSNanosecondsToTicks(42000);
        max_wait = OSNanosecondsToTicks(63000);
        buffer = OSNanosecondsToTicks(3000);
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        AISetStreamTrigger(0);
        AIResetStreamSampleCount();
        __AI_set_stream_sample_rate(AI_SAMPLERATE_48KHZ);
        AISetDSPSampleRate(AI_SAMPLERATE_32KHZ);
#if DEBUG
        OSReport("AIInit(): DSP is 32KHz\n");
#endif
        __AIS_Callback = NULL;
        __AID_Callback = NULL;
        __CallbackStack = stack;
        if (stack) {
            ASSERTMSGLINE(1107, ((u32)stack & 7) == 0, "AIInit: stack must be 8-byte aligned");
        }
        __OSSetInterruptHandler(__OS_INTERRUPT_DSP_AI, __AIDHandler);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_DSP_AI);
        __OSSetInterruptHandler(__OS_INTERRUPT_AI_AI, __AISHandler);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_AI_AI);
        __AI_init_flag = TRUE;
    }
}

static void __AISHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;

    __AIRegs[AI_REG_CONTROL] |= AI_CONTROL_STREAM_INTERRUPT;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AIS_Callback) {
        __AIS_Callback(__AIRegs[AI_REG_SAMPLE_COUNTER]);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static void __AIDHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;
    u16 tmp;

    tmp = __DSPRegs[DSP_CONTROL_STATUS];
    tmp = (tmp & ~(DSP_STATUS_ARAM_INTERRUPT | DSP_STATUS_DSP_INTERRUPT)) | DSP_STATUS_AI_INTERRUPT;
    __DSPRegs[DSP_CONTROL_STATUS] = tmp;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AID_Callback && !__AID_Active) {
        __AID_Active = TRUE;
        if (__CallbackStack) {
            __AICallbackStackSwitch(__AID_Callback);
        } else {
            __AID_Callback();
        }
        __AID_Active = FALSE;
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static asm void __AICallbackStackSwitch(register void* cb) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    mr r31, r3
    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    stw r1, 0x0(r5)
    lis r5, __CallbackStack@ha
    addi r5, r5, __CallbackStack@l
    lwz r1, 0x0(r5)
    subi r1, r1, 0x8
    mtlr r31
    blrl
    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    lwz r1, 0x0(r5)
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

static void __AI_SRC_INIT(void) {
    OSTime rising_32khz = 0;
    OSTime rising_48khz = 0;
    OSTime diff = 0;
    OSTime t1 = 0;
    OSTime temp;
    u32 temp0;
    u32 temp1;
    u32 done = 0;
    u32 volume = 0;
    u32 Init_Cnt = 0;
    u32 walking = 0;

    walking = 0;
    Init_Cnt = 0;
    temp = 0;

    while (!done) {
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 5, 1);
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 1, AI_SAMPLERATE_32KHZ);
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 0, AI_STREAM_START);
        temp0 = __AIRegs[AI_REG_SAMPLE_COUNTER];
        while (temp0 == __AIRegs[AI_REG_SAMPLE_COUNTER]) {
        }
        rising_32khz = OSGetTime();
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 1, AI_SAMPLERATE_48KHZ);
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 0, AI_STREAM_START);
        temp1 = __AIRegs[AI_REG_SAMPLE_COUNTER];
        while (temp1 == __AIRegs[AI_REG_SAMPLE_COUNTER]) {
        }
        rising_48khz = OSGetTime();
        diff = rising_48khz - rising_32khz;
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 1, AI_SAMPLERATE_32KHZ);
        MASKED_SET_REG_FIELD(0, __AIRegs[AI_REG_CONTROL], 1, 0, AI_STREAM_STOP);
        if (diff < bound_32KHz - buffer) {
            temp = min_wait;
            done = 1;
            Init_Cnt++;
        } else if (diff >= bound_32KHz + buffer && diff < bound_48KHz - buffer) {
            temp = max_wait;
            done = 1;
            Init_Cnt++;
        } else {
            done = 0;
            walking = 1;
            Init_Cnt++;
        }
    }
    while (rising_48khz + temp > OSGetTime()) {
    }
}

