#include <window.h>
#include <string.h>

extern "C" {

u16 windowGetKeyTrg(void)
{
    u16 key = windowGetKeyInfo()->mKeyTrg;
    key |= (windowGetKeyInfo()->mKeyOn & (WINDOW_KEY_UP | WINDOW_KEY_DOWN | WINDOW_KEY_LEFT | WINDOW_KEY_RIGHT))
        ? (u16)0 : windowGetKeyInfo2()->mKeyTrg;
    return key;
}

tagWINDOW_KEY* windowGetPortKeyInfo(u8 portMask)
{
    static tagWINDOW_KEY key;
    u8 portBits[4] = { 0x1, 0x2, 0x4, 0x8 };
    int port;
    tagWINDOW_KEY* portKey;

    memset(&key, 0, sizeof(key));
    for (port = 0; port < 4; port++) {
        portKey = &winSysWork.mPortKeys[port];
        if (portMask & portBits[port]) {
            key.mKeyOn |= portKey->mKeyOn;
            key.mKeyOnPrev |= portKey->mKeyOnPrev;
            key.mKeyTrg |= portKey->mKeyTrg;
            key.mKeyRepeat |= portKey->mKeyRepeat;
            key.mKeyRepeatActive |= portKey->mKeyRepeatActive;
        }
    }
    return &key;
}

tagWINDOW_KEY* windowGetPortKeyInfo2(u8 portMask)
{
    static tagWINDOW_KEY key;
    u8 portBits[4] = { 0x1, 0x2, 0x4, 0x8 };
    int port;
    tagWINDOW_KEY* portKey;

    memset(&key, 0, sizeof(key));
    for (port = 0; port < 4; port++) {
        portKey = &winSysWork2.mPortKeys[port];
        if (portMask & portBits[port]) {
            key.mKeyOn |= portKey->mKeyOn;
            key.mKeyOnPrev |= portKey->mKeyOnPrev;
            key.mKeyTrg |= portKey->mKeyTrg;
            key.mKeyRepeat |= portKey->mKeyRepeat;
            key.mKeyRepeatActive |= portKey->mKeyRepeatActive;
        }
    }
    return &key;
}

}
