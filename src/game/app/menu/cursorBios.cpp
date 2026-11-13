#include <cursorBios.h>

static CURSOR _cursor[16];

void cursorBiosInit(void)
{
    for (long i = 0; i < 16; i++) {
        _cursor[i].x = 0;
        _cursor[i].y = 0;
    }
}

CURSOR cursorBiosSetPos(int cursorID, CURSOR position)
{
    CURSOR previous = {-1, -1};

    if ((u16)cursorID >= 16) {
        return previous;
    }

    previous = _cursor[(u16)cursorID];
    _cursor[(u16)cursorID] = position;
    return previous;
}

CURSOR cursorBiosGetPos(int cursorID)
{
    CURSOR position = {-1, -1};

    if ((u16)cursorID >= 16) {
        return position;
    }

    position = _cursor[(u16)cursorID];
    return position;
}
