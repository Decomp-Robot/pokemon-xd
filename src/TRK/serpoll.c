#include "trk.h"

/* Message layout sizes, traced once as the serial handler starts up. */
const char sTRKPacketHeaderMessage[] = "TRK_Packet_Header \t    %ld bytes\n";
const char sTRKReadMemoryMessage[] = "TRK_CMD_ReadMemory     %ld bytes\n";
const char sTRKWriteMemoryMessage[] = "TRK_CMD_WriteMemory    %ld bytes\n";
const char sTRKConnectMessage[] = "TRK_CMD_Connect \t    %ld bytes\n";
const char sTRKReplyAckMessage[] = "TRK_CMD_ReplyAck\t    %ld bytes\n";
const char sTRKReadRegistersMessage[] = "TRK_CMD_ReadRegisters\t%ld bytes\n";

/* Traced by TRKTestForPacket as it frames a packet. */
const char sTRKFreeBufferMessage[] = "TestForPacket : FreeBuffer is  %ld\n";
const char sTRKReadingPayloadMessage[] = "Reading payload %ld bytes\n";
const char sTRKInvalidHeaderSizeMessage[] = "TestForPacket : Invalid size of packet hdr.size\n";
const char sTRKInvalidPacketMessage[] = "TestForPacket : Invalid size of packet\n";
const char sTRKReturningMessage[] = "TestForPacket returning %ld\n";

DSError TRKTerminateSerialHandler(void)
{
    return DS_NoError;
}


DSError TRKInitializeSerialHandler(void)
{
    TRKSerialHandlerState * const handler = &gTRKSerialHandlerState;

    handler->msgBufID = -1;
    handler->receiveState = kWaitFlag;
    handler->isEscape = FALSE;

    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKPacketHeaderMessage, kMessageHeaderSize);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKReadMemoryMessage, kMessageHeaderSize);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKWriteMemoryMessage, kMessageHeaderSize);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKConnectMessage, kMessageHeaderSize);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKReplyAckMessage, kMessageHeaderSize);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKReadRegistersMessage, kMessageHeaderSize);

    return DS_NoError;
}


void TRKProcessInput(MessageBufferID bufferID)
{
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    event.msgBufID = bufferID;
    gTRKSerialHandlerState.msgBufID = -1;
    TRKPostEvent(&event);
}


void TRKGetInput(void)
{
    MessageBufferID id = TRKTestForPacket();

    if (id != -1) {
        TRKGetBuffer(id);
        TRKProcessInput(id);
    }
}


MessageBufferID TRKTestForPacket(void)
{
    MessageBufferID bufID;
    TRKBuffer *buf;
    MessageBufferID result;
    u8 packetBuf[kMessageHeaderSize];
    u8 payloadBuf[kMessageBufferSize];

    if (TRKPollUART() <= 0) {
        return -1;
    }

    result = TRKGetFreeBuffer(&bufID, &buf);

    MWTRACE(TRK_TRACE_LEVEL_DEBUG, sTRKFreeBufferMessage, result);

    TRKSetBufferPosition(buf, 0);
    if (TRKReadUARTN(packetBuf, kMessageHeaderSize) == DS_NoError) {
        int readSize;

        TRKAppendBuffer_ui8(buf, packetBuf, kMessageHeaderSize);
        readSize = ((u32 *)packetBuf)[0] - kMessageHeaderSize;
        result = bufID;
        if (readSize > 0) {
            MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKReadingPayloadMessage, readSize);
            if (TRKReadUARTN(payloadBuf, ((u32 *)packetBuf)[0] - kMessageHeaderSize) == DS_NoError) {
                TRKAppendBuffer_ui8(buf, payloadBuf, ((u32 *)packetBuf)[0]);
            } else {
                MWTRACE(TRK_TRACE_LEVEL_ERROR, sTRKInvalidHeaderSizeMessage);
                TRKReleaseBuffer(result);
                result = -1;
            }
        }
    } else {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, sTRKInvalidPacketMessage);
        TRKReleaseBuffer(result);
        result = -1;
    }

    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKReturningMessage, result);
    return result;
}
