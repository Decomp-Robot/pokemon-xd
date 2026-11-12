#include "trk.h"

DSError TRKDoNotifyStopped(MessageCommandID command)
{
    MessageBufferID replyID;
    MessageBufferID requestID;
    TRKBuffer *buffer;
    DSError err;

    err = TRKGetFreeBuffer(&requestID, &buffer);
    if (err == DS_NoError) {
        if (err == DS_NoError) {
            if (command == kDSNotifyStopped) {
                TRKTargetAddStopInfo(buffer);
            } else {
                TRKTargetAddExceptionInfo(buffer);
            }
        }
        err = TRKRequestSend(buffer, &replyID, 2, 3, TRUE);
        if (err == DS_NoError) {
            TRKReleaseBuffer(replyID);
        }
        TRKReleaseBuffer(requestID);
    }
    return err;
}
