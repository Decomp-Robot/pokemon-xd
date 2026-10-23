#include <types.h>

s32 dbgMenuGetGsinputMsgId(s32 id)
{
    s32 msg;

    if (id == 0) {
        msg = 0xED04;
    } else if (id == 1) {
        msg = 0xED05;
    } else if (id == 2) {
        msg = 0xED06;
    } else if (id == 3) {
        msg = 0xED07;
    } else if (id == 4) {
        msg = 0xED08;
    } else {
        msg = 0xED04;
    }
    return msg;
}
