#include "trk.h"

static const char sTRKNubWelcomeMessage[] = "MetroTRK for GAMECUBE v2.6";
static const char sTRKInitializeNubMessage[] = "Initialize NUB\n";

void TRKNubWelcome(void)
{
    TRK_board_display(sTRKNubWelcomeMessage);
}


DSError TRKTerminateNub(void)
{
    TRKTerminateSerialHandler();
    return DS_NoError;
}


static inline DSError TRKInitializeEndian(void)
{
    s32 result = DS_NoError;
    u32 test;
    u8 *byteAlias = (u8 *)&test;

    gTRKBigEndian = TRUE;
    byteAlias[0] = 0x12;
    byteAlias[1] = 0x34;
    byteAlias[2] = 0x56;
    byteAlias[3] = 0x78;

    if (test == 0x12345678) {
        gTRKBigEndian = TRUE;
    } else if (test == 0x78563412) {
        gTRKBigEndian = FALSE;
    } else {
        result = TRUE;
    }

    return result;
}


DSError TRKInitializeNub(void)
{
    s32 result = DS_NoError;
    s32 uartResult;

    result = TRKInitializeEndian();

    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKInitializeNubMessage);

    if (result == DS_NoError) {
        usr_put_initialize();
    }
    if (result == DS_NoError) {
        result = TRKInitializeEventQueue();
    }
    if (result == DS_NoError) {
        result = TRKInitializeMessageBuffers();
    }
    if (result == DS_NoError) {
        result = TRKInitializeDispatcher();
    }

    InitializeProgramEndTrap();

    if (result == DS_NoError) {
        result = TRKInitializeSerialHandler();
    }
    if (result == DS_NoError) {
        result = TRKInitializeTarget();
    }
    if (result == DS_NoError) {
        uartResult = TRKInitializeIntDrivenUART(kTRKHostBaudRate, 1, 0, &gTRKInputPendingPtr);
        TRKTargetSetInputPendingPtr(gTRKInputPendingPtr);
        if (uartResult != DS_NoError) {
            result = uartResult;
        }
    }

    return result;
}
