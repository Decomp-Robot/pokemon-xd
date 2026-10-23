#ifndef MENUSAVELOAD_H
#define MENUSAVELOAD_H

#include "types.h"
#include "winSprite.h"

class menuSaveload {
public:
    menuSaveload();
    ~menuSaveload();
    void Main();
    bool ComfirmOverwriting();
    void SaveParameterSet();
    void Ctrl(tagWINDOW_WORK *window);
    void _menuClose(long menu);

    unsigned long mPlayTime;
};

#endif
