#include <dolphin.h>
#include <dolphin/si.h>

static void (*SamplingCallback)();
u32 __SISteeringEnableBits;

// UNUSED: the game never calls this; dead-stripped at link.
void __SISteeringEnable(s32 chan) {
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = 0x80000000 >> chan;
    if (!(__SISteeringEnableBits & chanBit)) {
        __SISteeringEnableBits |= chanBit;
        SIGetResponse(chan, &data);

        cmd = 0x300680;
        SISetCommand(chan, cmd);
        SIEnablePolling(__SISteeringEnableBits);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
void __SISteeringDisable(s32 chan) {
    u32 chanBit;

    chanBit = 0x80000000 >> chan;
    SIDisablePolling(chanBit);
    __SISteeringEnableBits &= ~chanBit;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
static void SamplingHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;

    if (SamplingCallback != 0) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        SamplingCallback();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
void (* SISetSteeringSamplingCallback(void (*callback)()))() {
    void (*prev)() = SamplingCallback;
    SamplingCallback = callback;

    if (callback != 0) {
        SIRegisterPollingHandler(SamplingHandler);
    } else {
        SIUnregisterPollingHandler(SamplingHandler);
    }

    return prev;
}

// UNUSED: the game never calls this; dead-stripped at link.
void SIControlSteering(s32 chan, u32 control, s32 level) {
    BOOL enabled;
    u32 chanBit;
    u32 command;

    control &= 0x600;

    if (level <= -0x80) {
        command = 0;
    } else if (level >= 0x80) {
        command = 0x100;
    } else {
        command = level + 0x80;
    }

    command |= control;
    command &= 0x7FF;

    enabled = OSDisableInterrupts();
    chanBit = 0x80000000 >> chan;
    if (__SISteeringEnableBits & chanBit) {
        command |= 0x300000;
        SISetCommand(chan, command);
        SITransferCommands();
    }

    OSRestoreInterrupts(enabled);
}
