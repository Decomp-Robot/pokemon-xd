#include <types.h>
#include <winSprite.h>
#include <app/menu/menuNintendoWarning.h>

// Face and shoulder buttons accept the warning screen.
#define NINTENDO_WARNING_KEY_MASK 0xFF0

// While the warning is waiting for input, a button press accepts it.
void _menuNintendoWarningButtonNormal(tagWINDOW_WORK *window) {
    if (window != 0) {
        if (windowGetKeyInfo()->mKeyTrg & NINTENDO_WARNING_KEY_MASK) {
            window->mAccept = 1;
        }
    }
}

// Window callback of the warning screen.
extern "C" void menuNintendoWarningButton(tagWINDOW_WORK *window) {
    if (_warningSeq == WARNING_SEQ_ACCEPTING) {
        _menuNintendoWarningButtonNormal(window);
    } else if (_warningSeq == WARNING_SEQ_AUTO_ACCEPT) {
        window->mAccept = 1;
    }
}
