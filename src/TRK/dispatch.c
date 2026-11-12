#include "trk.h"

DSError TRKDispatchMessage(TRKBuffer *message)
{
    DSError error = DS_DispatchError;

    TRKSetBufferPosition(message, 0);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "Dispatch command 0x%08x\n", message->data[kDSCommandOffset]);

    switch (message->data[kDSCommandOffset]) {
    case kDSConnect: error = TRKDoConnect(message); break;
    case kDSDisconnect: error = TRKDoDisconnect(message); break;
    case kDSReset: error = TRKDoReset(message); break;
    case kDSOverride: error = TRKDoOverride(message); break;
    case kDSVersions: error = TRKDoVersions(message); break;
    case kDSSupportMask: error = TRKDoSupportMask(message); break;
    case kDSReadMemory: error = TRKDoReadMemory(message); break;
    case kDSWriteMemory: error = TRKDoWriteMemory(message); break;
    case kDSReadRegisters: error = TRKDoReadRegisters(message); break;
    case kDSWriteRegisters: error = TRKDoWriteRegisters(message); break;
    case kDSContinue: error = TRKDoContinue(message); break;
    case kDSStep: error = TRKDoStep(message); break;
    case kDSStop: error = TRKDoStop(message); break;
    case kDSSetOption: error = TRKDoSetOption(message); break;
    }

    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "Dispatch complete err = %ld\n", error);
    return error;
}

DSError TRKInitializeDispatcher(void)
{
    return DS_NoError;
}
