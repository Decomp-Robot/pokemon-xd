#include "trk.h"

void usr_put_initialize(void)
{
}


BOOL usr_puts_serial(const char *msg)
{
    BOOL status = FALSE;
    BOOL connectionStatus;
    char buf[2];
    char c;

    // Each character is written with the connected flag cleared so the
    // report does not leave as a nub message, and the flag is restored right
    // after.  The loop condition reports a status the body never raises, so
    // the string is always run to its terminator.
    while (!status && (c = *msg++) != '\0') {
        connectionStatus = TRKIsConnected();

        buf[0] = c;
        buf[1] = '\0';

        SetTRKConnected(FALSE);
        OSReport(buf);

        SetTRKConnected(connectionStatus);
        status = FALSE;
    }

    return status;
}
