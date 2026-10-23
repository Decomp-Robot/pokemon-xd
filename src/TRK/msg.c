#include "trk.h"

/* Traced with the result of the UART write. */
const char sTRKMessageSend[] = "MessageSend : cc_write returned %ld\n";

DSError TRKMessageSend(TRKBuffer *message)
{
    DSError result;

    result = TRKWriteUARTN(message->data, message->length);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, sTRKMessageSend, result);

    return DS_NoError;
}
