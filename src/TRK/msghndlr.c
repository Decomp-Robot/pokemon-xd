#include "trk.h"
#include <string.h>

typedef struct CommandReply {
    u32 length;
    u8 commandID;
    u8 reserved[3];
    u8 replyError;
    u8 padding[kMessageHeaderSize - 9];
} CommandReply;

enum {
    kDSReplyACK = 128,
    kDSReplyNoError = 0,
    kDSReplyError = 1,
    kDSReplyPacketSizeError = 2,
    kDSReplyCWDSError = 3,
    kDSReplyUnsupportedCommandError = 16,
    kDSReplyParameterError,
    kDSReplyUnsupportedOptionError,
    kDSReplyInvalidMemoryRange,
    kDSReplyInvalidRegisterRange,
    kDSReplyCWDSException,
    kDSReplyNotStopped,
    kDSReplyOsError = 32,
    kDSReplyInvalidProcessId,
    kDSReplyInvalidThreadId
};

enum {
    kTRKLastDefaultRegister = 36,
    kTRKLastFPRegister = 33,
    kTRKLastExtended1Register = 96,
    kTRKLastExtended2Register = 31,
    kDSReadRegistersReplySize = 1128,
    kTRKMemoryTransferSize = 2048,
    kTRKARAMAlignment = 32
};

BOOL IsTRKConnected;

static inline void TRKSendReply(u8 err)
{
    CommandReply reply;
    memset(&reply, 0, sizeof(CommandReply));
    reply.commandID = kDSReplyACK;
    reply.length = sizeof(CommandReply);
    reply.replyError = err;
    TRKWriteUARTN(&reply, sizeof(CommandReply));
}
static inline DSError TRKSendACK(TRKBuffer* message)
{
    DSError err;
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "SendACK : Calling MessageSend\n");
    err = TRKMessageSend(message);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "MessageSend err : %ld\n", err);
    return err;
}

DSError TRKDoSetOption(TRKBuffer* message)
{
    u8 option = message->data[kDSOptionsOffset];
    u8 value = message->data[kDSOptionValueOffset];
    if (option == kDSOptionSerialIO) {
        usr_puts_serial("\nMetroTRK Option : SerialIO - ");
        if (value) usr_puts_serial("Enable\n");
        else usr_puts_serial("Disable\n");
        TRKSetUseSerialIO(value);
    }
    TRKSendReply(kDSReplyNoError);
    return DS_NoError;
}

DSError TRKDoStop(TRKBuffer* message)
{
    u8 replyError;
    switch (TRKTargetStop()) {
    case DS_NoError: replyError = kDSReplyNoError; break;
    case DS_InvalidProcessId: replyError = kDSReplyInvalidProcessId; break;
    case DS_InvalidThreadId: replyError = kDSReplyInvalidThreadId; break;
    case DS_OsError: replyError = kDSReplyOsError; break;
    default: replyError = kDSReplyError; break;
    }
    TRKSendReply(replyError);
    return DS_NoError;
}

DSError TRKDoStep(TRKBuffer* message)
{
    DSError result;
    u8 mode;
    u8 count;
    u32 rangeStart;
    u32 rangeEnd;
    u32 pc;

    TRKSetBufferPosition(message, 0);
    mode = message->data[kDSOptionsOffset];
    rangeStart = *(u32*)(message->data + kDSAddressOffset);
    rangeEnd = *(u32*)(message->data + kDSRangeEndOffset);
    switch (mode) {
    case kDSStepIntoCount:
    case kDSStepOverCount:
        count = message->data[kDSStepCountOffset];
        if (count >= 1) break;
        TRKSendReply(kDSReplyParameterError);
        return DS_NoError;
    case kDSStepIntoRange:
    case kDSStepOverRange:
        pc = TRKTargetGetPC();
        if (pc >= rangeStart && pc <= rangeEnd) break;
        TRKSendReply(kDSReplyParameterError);
        return DS_NoError;
    default:
        TRKSendReply(kDSReplyUnsupportedOptionError);
        return DS_NoError;
    }
    if (!TRKTargetStopped()) { TRKSendReply(kDSReplyNotStopped); return DS_NoError; }
    TRKSendReply(kDSReplyNoError);
    result = DS_NoError;
    switch (mode) {
    case kDSStepIntoCount:
    case kDSStepOverCount:
        result = TRKTargetSingleStep(count, mode == kDSStepOverCount);
        break;
    case kDSStepIntoRange:
    case kDSStepOverRange:
        result = TRKTargetStepOutOfRange(rangeStart, rangeEnd, mode == kDSStepOverRange);
        break;
    }
    return result;
}

DSError TRKDoContinue(TRKBuffer* message)
{
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "DoContinue\n");
    if (!TRKTargetStopped()) { TRKSendReply(kDSReplyNotStopped); return DS_NoError; }
    TRKSendReply(kDSReplyNoError);
    return TRKTargetContinue();
}

DSError TRKDoFlushCache(TRKBuffer* message)
{
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "DoFlushCache unimplemented!!!\n");
    return DS_NoError;
}

DSError TRKDoWriteRegisters(TRKBuffer* message)
{
    DSError error;
    unsigned long registersLength;
    CommandReply reply;
    u8 replyError;
    u8 options = message->data[kDSOptionsOffset];
    u16 firstRegister = *(u16*)(message->data + kDSFirstRegisterOffset);
    u16 lastRegister = *(u16*)(message->data + kDSLastRegisterOffset);

    TRKSetBufferPosition(message, 0);
    if (firstRegister > lastRegister) { TRKSendReply(kDSReplyInvalidRegisterRange); return DS_NoError; }
    TRKSetBufferPosition(message, sizeof(CommandReply));
    switch (options) {
    case kDSRegistersDefault: error = TRKTargetAccessDefault(firstRegister, lastRegister, message, &registersLength, 0); break;
    case kDSRegistersFP: error = TRKTargetAccessFP(firstRegister, lastRegister, message, &registersLength, 0); break;
    case kDSRegistersExtended1: error = TRKTargetAccessExtended1(firstRegister, lastRegister, message, &registersLength, 0); break;
    case kDSRegistersExtended2: error = TRKTargetAccessExtended2(firstRegister, lastRegister, message, &registersLength, 0); break;
    default: error = DS_UnsupportedError; break;
    }
    TRKResetBuffer(message, 0);
    if (error == DS_NoError) {
        memset(&reply, 0, sizeof(CommandReply));
        reply.length = sizeof(CommandReply);
        reply.commandID = kDSReplyACK;
        reply.replyError = error;
        error = TRKAppendBuffer(message, &reply, sizeof(CommandReply));
    }
    if (error != DS_NoError) {
        switch (error) {
        case DS_UnsupportedError: replyError = kDSReplyUnsupportedOptionError; break;
        case DS_InvalidRegister: replyError = kDSReplyInvalidRegisterRange; break;
        case DS_MessageBufferReadError: replyError = kDSReplyPacketSizeError; break;
        case DS_CWDSException: replyError = kDSReplyCWDSException; break;
        case DS_InvalidProcessId: replyError = kDSReplyInvalidProcessId; break;
        case DS_InvalidThreadId: replyError = kDSReplyInvalidThreadId; break;
        case DS_OsError: replyError = kDSReplyOsError; break;
        default: replyError = kDSReplyCWDSError; break;
        }
        TRKSendReply(replyError);
        return DS_NoError;
    }
    return TRKSendACK(message);
}

DSError TRKDoReadRegisters(TRKBuffer* message)
{
    DSError error;
    unsigned long registersLength;
    CommandReply reply;
    u8 replyError;
    u16 firstRegister = *(u16*)(message->data + kDSFirstRegisterOffset);
    u16 lastRegister = *(u16*)(message->data + kDSLastRegisterOffset);

    if (firstRegister > lastRegister) { TRKSendReply(kDSReplyInvalidRegisterRange); return DS_NoError; }
    reply.commandID = kDSReplyACK;
    reply.length = kDSReadRegistersReplySize;
    TRKResetBuffer(message, 0);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    TRKAppendBuffer_ui8(message, (u8*)&reply, sizeof(CommandReply));
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    error = TRKTargetAccessDefault(0, kTRKLastDefaultRegister, message, &registersLength, 1);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Error reading  default regs 0x%08x\n", error);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (error == DS_NoError) error = TRKTargetAccessFP(0, kTRKLastFPRegister, message, &registersLength, 1);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Error FP regs 0x%08x\n", error);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (error == DS_NoError) error = TRKTargetAccessExtended1(0, kTRKLastExtended1Register, message, &registersLength, 1);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Error extended1 regs 0x%08x\n", error);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (error == DS_NoError) error = TRKTargetAccessExtended2(0, kTRKLastExtended2Register, message, &registersLength, 1);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Error extended2 regs 0x%08x\n", error);
    MWTRACE(TRK_TRACE_LEVEL_DEBUG, "DoReadRegisters : Buffer length 0x%08x\n", message->length);
    if (error != DS_NoError) {
        switch (error) {
        case DS_UnsupportedError: replyError = kDSReplyUnsupportedOptionError; break;
        case DS_InvalidRegister: replyError = kDSReplyInvalidRegisterRange; break;
        case DS_CWDSException: replyError = kDSReplyCWDSException; break;
        case DS_InvalidProcessId: replyError = kDSReplyInvalidProcessId; break;
        case DS_InvalidThreadId: replyError = kDSReplyInvalidThreadId; break;
        case DS_OsError: replyError = kDSReplyOsError; break;
        default: replyError = kDSReplyCWDSError; break;
        }
        TRKSendReply(replyError);
        return DS_NoError;
    }
    return TRKSendACK(message);
}

DSError TRKDoWriteMemory(TRKBuffer* message)
{
    DSError error;
    u8 replyError;
    u8 transferData[kTRKMemoryTransferSize + kTRKARAMAlignment] __attribute__((aligned(kTRKARAMAlignment)));
    CommandReply reply;
    unsigned long transferLength;
    u16 length;
    u32 start;
    u8 options;
    start = *(u32*)(message->data + kDSAddressOffset);
    length = *(u16*)(message->data + kDSLengthOffset);
    options = message->data[kDSOptionsOffset];
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "WriteMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", message->data[kDSCommandOffset], start, length, options);
    if (options & kDSMemoryExtended) { TRKSendReply(kDSReplyUnsupportedOptionError); return DS_NoError; }
    transferLength = length;
    TRKSetBufferPosition(message, sizeof(CommandReply));
    if (options & kDSMemoryARAM) {
        TRKReadBuffer(message, transferData + (start & (kTRKARAMAlignment - 1)), transferLength);
        error = TRKTargetAccessARAM(transferData, start, &transferLength, 0);
    } else {
        TRKReadBuffer(message, transferData, transferLength);
        error = TRKTargetAccessMemory(transferData, start, &transferLength, (options & kDSMemoryUserView) == 0, 0);
    }
    TRKResetBuffer(message, 0);
    if (error == DS_NoError) {
        memset(&reply, 0, sizeof(CommandReply));
        reply.length = sizeof(CommandReply);
        reply.commandID = kDSReplyACK;
        reply.replyError = error;
        error = TRKAppendBuffer(message, &reply, sizeof(CommandReply));
    }
    if (error != DS_NoError) {
        switch (error) {
        case DS_CWDSException: replyError = kDSReplyCWDSException; break;
        case DS_InvalidMemory: replyError = kDSReplyInvalidMemoryRange; break;
        case DS_InvalidProcessId: replyError = kDSReplyInvalidProcessId; break;
        case DS_InvalidThreadId: replyError = kDSReplyInvalidThreadId; break;
        case DS_OsError: replyError = kDSReplyOsError; break;
        default: replyError = kDSReplyCWDSError; break;
        }
        TRKSendReply(replyError);
        return DS_NoError;
    }
    return TRKSendACK(message);
}

DSError TRKDoReadMemory(TRKBuffer* message)
{
    DSError error;
    u8 transferData[kTRKMemoryTransferSize + kTRKARAMAlignment] __attribute__((aligned(kTRKARAMAlignment)));
    CommandReply reply;
    unsigned long transferLength;
    u8 replyError;
    u16 length;
    u32 start;
    u8 options;
    start = *(u32*)(message->data + kDSAddressOffset);
    length = *(u16*)(message->data + kDSLengthOffset);
    options = message->data[kDSOptionsOffset];
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "ReadMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", message->data[kDSCommandOffset], start, length, options);
    if (options & kDSMemoryExtended) { TRKSendReply(kDSReplyUnsupportedOptionError); return DS_NoError; }
    transferLength = length;
    if (options & kDSMemoryARAM) {
        error = TRKTargetAccessARAM(transferData, start, &transferLength, 1);
    } else {
        error = TRKTargetAccessMemory(transferData, start, &transferLength, (options & kDSMemoryUserView) == 0, 1);
    }
    TRKResetBuffer(message, 0);
    if (error == DS_NoError) {
        memset(&reply, 0, sizeof(CommandReply));
        reply.replyError = error;
        reply.length = transferLength + sizeof(CommandReply);
        reply.commandID = kDSReplyACK;
        TRKAppendBuffer(message, &reply, sizeof(CommandReply));
        if (options & kDSMemoryARAM) error = TRKAppendBuffer(message, transferData + (start & (kTRKARAMAlignment - 1)), transferLength);
        else error = TRKAppendBuffer(message, transferData, transferLength);
    }
    if (error != DS_NoError) {
        switch (error) {
        case DS_CWDSException: replyError = kDSReplyCWDSException; break;
        case DS_InvalidMemory: replyError = kDSReplyInvalidMemoryRange; break;
        case DS_InvalidProcessId: replyError = kDSReplyInvalidProcessId; break;
        case DS_InvalidThreadId: replyError = kDSReplyInvalidThreadId; break;
        case DS_OsError: replyError = kDSReplyOsError; break;
        default: replyError = kDSReplyCWDSError; break;
        }
        TRKSendReply(replyError);
        return DS_NoError;
    }
    return TRKSendACK(message);
}

DSError TRKDoSupportMask(TRKBuffer* message) { return DS_NoError; }

DSError TRKDoVersions(TRKBuffer* message) { return DS_NoError; }

DSError TRKDoOverride(TRKBuffer* message) { TRKSendReply(kDSReplyNoError); __TRK_copy_vectors(); return DS_NoError; }

DSError TRKDoReset(TRKBuffer* message) { TRKSendReply(kDSReplyNoError); __TRK_reset(); return DS_NoError; }

DSError TRKDoDisconnect(TRKBuffer* message)
{
    TRKEvent event;
    IsTRKConnected = FALSE;
    TRKSendReply(kDSReplyNoError);
    TRKConstructEvent(&event, NUBEVENT_Shutdown);
    TRKPostEvent(&event);
    return DS_NoError;
}

DSError TRKDoConnect(TRKBuffer* message) { IsTRKConnected = TRUE; TRKSendReply(kDSReplyNoError); return DS_NoError; }

void SetTRKConnected(BOOL connected) { IsTRKConnected = connected; }

BOOL GetTRKConnected(void) { return IsTRKConnected; }

void OutputData(u8* data, int length)
{
    int i;

    for (i = 0; i < length; i++) {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, "%02x ", data[i]);
        if (i % 16 == 15)
            MWTRACE(TRK_TRACE_LEVEL_ERROR, "\n");
    }
    MWTRACE(TRK_TRACE_LEVEL_ERROR, "\n");
}
