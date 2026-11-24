#include "trk.h"
#include <string.h>

typedef struct FilePositionRequest {
    u32 length;
    u8 command;
    u8 reserved[3];
    u32 fileHandle;
    u32 filePosition;
    u8 mode;
    u8 padding[kMessageHeaderSize - 17];
} FilePositionRequest;

typedef struct FileCloseRequest {
    u32 length;
    u8 command;
    u8 reserved[3];
    u32 fileHandle;
    u8 padding[kMessageHeaderSize - 12];
} FileCloseRequest;

typedef struct FileOpenRequest {
    u32 length;
    u8 command;
    u8 reserved[3];
    u8 flags;
    u8 reserved2[3];
    u16 pathLength;
    u8 padding[kMessageHeaderSize - 14];
} FileOpenRequest;

typedef struct FileAccessRequest {
    u32 length;
    u8 command;
    u8 reserved[3];
    u32 fileHandle;
    u16 count;
    u8 padding[kMessageHeaderSize - 14];
} FileAccessRequest;

typedef struct FileReply {
    u32 length;
    u8 command;
    u8 reserved[3];
    u32 fileHandle;
    u8 reserved2[4];
    u32 ioResult;
    u16 transferredCount;
    u8 reserved3[2];
    u32 filePosition;
    u8 padding[kMessageHeaderSize - 28];
} FileReply;

enum {
    kTRKReplyPollLimit = 79999980,
    kTRKRequestRetryCount = 3
};

enum {
    kTRKFileTransferSize = 2048,
    kTRKFileWriteReplySize = 5,
    kTRKFileReadReplySize = 5,
    kTRKFileOpenReplySize = 7,
    kTRKFilePositionReplySize = 3,
    kTRKFileCloseReplySize = 3
};

DSError HandlePositionFileSupportRequest(u32 fileHandle, u32 *position, u8 mode,
                                        DSError *ioResult)
{
    MessageBufferID replyID;
    MessageBufferID requestID;
    TRKBuffer *buffer;
    TRKBuffer *reply;
    DSError err;
    FilePositionRequest request;

    memset(&request, 0, sizeof(FilePositionRequest));
    request.command = kTRKFilePosition;
    request.length = kMessageHeaderSize;
    request.fileHandle = fileHandle;
    request.filePosition = *position;
    request.mode = mode;
    err = TRKGetFreeBuffer(&requestID, &buffer);
    if (err == DS_NoError) {
        err = TRKAppendBuffer_ui8(buffer, (u8 *)&request, kMessageHeaderSize);
    }
    if (err == DS_NoError) {
        *ioResult = DS_NoError;
        *position = -1;
        err = TRKRequestSend(buffer, &replyID, kTRKFilePositionReplySize,
                             kTRKRequestRetryCount, FALSE);
        if (err == DS_NoError) {
            if ((reply = TRKGetBuffer(replyID)) != NULL) {
                *ioResult = ((FileReply *)reply->data)->ioResult;
                *position = ((FileReply *)reply->data)->filePosition;
            }
        }
        TRKReleaseBuffer(replyID);
    }
    TRKReleaseBuffer(requestID);
    return err;
}

DSError HandleCloseFileSupportRequest(u32 fileHandle, DSError *ioResult)
{
    MessageBufferID replyID;
    MessageBufferID requestID;
    TRKBuffer *buffer;
    DSError err;
    TRKBuffer *reply;
    FileCloseRequest request;

    memset(&request, 0, sizeof(FileCloseRequest));
    request.command = kTRKFileClose;
    request.length = kMessageHeaderSize;
    request.fileHandle = fileHandle;
    err = TRKGetFreeBuffer(&requestID, &buffer);
    if (err == DS_NoError) {
        err = TRKAppendBuffer_ui8(buffer, (u8 *)&request, kMessageHeaderSize);
    }
    if (err == DS_NoError) {
        *ioResult = DS_NoError;
        err = TRKRequestSend(buffer, &replyID, kTRKFileCloseReplySize,
                             kTRKRequestRetryCount, FALSE);
        if (err == DS_NoError) {
            reply = TRKGetBuffer(replyID);
        }
        if (err == DS_NoError) {
            *ioResult = ((FileReply *)reply->data)->ioResult;
        }
        TRKReleaseBuffer(replyID);
    }
    TRKReleaseBuffer(requestID);
    return err;
}

DSError HandleOpenFileSupportRequest(const char *path, u8 flags, u32 *fileId,
                                    DSError *ioResult)
{
    MessageBufferID replyID;
    MessageBufferID requestID;
    TRKBuffer *buffer;
    DSError err;
    TRKBuffer *reply;
    FileOpenRequest request;

    memset(&request, 0, sizeof(FileOpenRequest));
    *fileId = 0;
    request.command = kTRKFileOpen;
    request.length = strlen(path) + kMessageHeaderSize + 1;
    request.flags = flags;
    request.pathLength = strlen(path) + 1;
    TRKGetFreeBuffer(&requestID, &buffer);
    err = TRKAppendBuffer_ui8(buffer, (u8 *)&request, kMessageHeaderSize);
    if (err == DS_NoError) {
        err = TRKAppendBuffer_ui8(buffer, (const u8 *)path, strlen(path) + 1);
    }
    if (err == DS_NoError) {
        *ioResult = DS_NoError;
        err = TRKRequestSend(buffer, &replyID, kTRKFileOpenReplySize,
                             kTRKRequestRetryCount, FALSE);
        if (err == DS_NoError) {
            reply = TRKGetBuffer(replyID);
        }
        *ioResult = ((FileReply *)reply->data)->ioResult;
        *fileId = ((FileReply *)reply->data)->fileHandle;
        TRKReleaseBuffer(replyID);
    }
    TRKReleaseBuffer(requestID);
    return err;
}

DSError TRKRequestSend(TRKBuffer *request, MessageBufferID *replyID,
                       u32 minReplySize, u32 retryCount, BOOL useTimeout)
{
    DSError err = DS_NoError;
    TRKBuffer *reply;
    u32 count;
    s32 retry;
    u8 command;
    DSError error;
    BOOL badReply = TRUE;

    *replyID = -1;
    retry = retryCount + 1;
    while (retry != 0 && *replyID == -1 && err == DS_NoError) {
        MWTRACE(TRK_TRACE_LEVEL_DETAIL, "Calling MessageSend\n");
        err = TRKMessageSend(request);
        if (err == DS_NoError) {
            if (useTimeout) {
                count = 0;
            }
            while (1) {
                do {
                    *replyID = TRKTestForPacket();
                } while (-1 == *replyID &&
                         (!useTimeout || ++count < kTRKReplyPollLimit));
                if (-1 != *replyID) {
                    badReply = FALSE;
                    reply = TRKGetBuffer(*replyID);
                    TRKSetBufferPosition(reply, 0);
                    OutputData(reply->data, reply->length);
                    command = reply->data[kDSCommandOffset];
                    MWTRACE(TRK_TRACE_LEVEL_DETAIL,
                            "msg_command : 0x%02x hdr->cmdID 0x%02x\n", command,
                            command);
                    if (command < kDSReplyACK) {
                        TRKProcessInput(*replyID);
                        *replyID = -1;
                        continue;
                    }
                }
                break;
            }
            if (-1 != *replyID) {
                if (reply->length < kMessageHeaderSize) {
                    badReply = TRUE;
                }
                if (err == DS_NoError && !badReply) {
                    error = reply->data[kDSReplyErrorOffset];
                    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "msg_error : 0x%02x\n",
                            error);
                }
                if (err == DS_NoError && !badReply &&
                    ((int)command != kDSReplyACK || error != DS_NoError)) {
                    MWTRACE(TRK_TRACE_LEVEL_ERROR,
                            "RequestSend : Bad ack or non ack received "
                            "msg_command : 0x%02x msg_error 0x%02x\n",
                            command, error);
                    badReply = TRUE;
                }
                if (err != DS_NoError || badReply) {
                    TRKReleaseBuffer(*replyID);
                    *replyID = -1;
                }
            }
        }
        retry--;
    }
    if (*replyID == -1) {
        err = DS_NoResponse;
    }
    return err;
}

DSError TRKSuppAccessFile(u32 fileHandle, u8 *data, u32 *length,
                          DSError *ioResult, BOOL needReply, BOOL read)
{
    DSError err;
    MessageBufferID replyID;
    TRKBuffer *reply;
    MessageBufferID requestID;
    TRKBuffer *buffer;
    u32 count;
    u32 offset;
    u8 fileResult;
    u32 transferred;
    BOOL done;
    FileAccessRequest request;

    if (data == NULL || *length == 0) {
        return DS_InvalidArgument;
    }
    done = FALSE;
    *ioResult = DS_NoError;
    offset = 0;
    err = DS_NoError;
    while (!done && offset < *length && err == DS_NoError &&
           *ioResult == DS_NoError) {
        memset(&request, 0, sizeof(FileAccessRequest));
        if (*length - offset > kTRKFileTransferSize) {
            count = kTRKFileTransferSize;
        } else {
            count = *length - offset;
        }
        request.command = read ? kTRKFileRead : kTRKFileWrite;
        request.length = read ? kMessageHeaderSize : count + kMessageHeaderSize;
        request.fileHandle = fileHandle;
        request.count = count;
        TRKGetFreeBuffer(&requestID, &buffer);
        err = TRKAppendBuffer_ui8(buffer, (u8 *)&request, kMessageHeaderSize);
        if (!read && err == DS_NoError) {
            err = TRKAppendBuffer_ui8(buffer, data + offset, count);
        }
        if (err == DS_NoError) {
            if (needReply) {
                err = TRKRequestSend(buffer, &replyID,
                                     read ? kTRKFileReadReplySize
                                          : kTRKFileWriteReplySize,
                                     kTRKRequestRetryCount,
                                     !(read && fileHandle == 0));
                if (err == DS_NoError) {
                    reply = TRKGetBuffer(replyID);
                }
                fileResult = ((FileReply *)reply->data)->ioResult;
                transferred = ((FileReply *)reply->data)->transferredCount;
                if (read && err == DS_NoError && transferred <= count) {
                    TRKSetBufferPosition(reply, kMessageHeaderSize);
                    err = TRKReadBuffer_ui8(reply, data + offset, transferred);
                    if (err == DS_MessageBufferReadError) {
                        err = DS_NoError;
                    }
                }
                if (transferred != count) {
                    count = transferred;
                    done = TRUE;
                }
                *ioResult = fileResult;
                TRKReleaseBuffer(replyID);
            } else {
                err = TRKMessageSend(buffer);
            }
        }
        TRKReleaseBuffer(requestID);
        offset += count;
    }
    *length = offset;
    return err;
}
