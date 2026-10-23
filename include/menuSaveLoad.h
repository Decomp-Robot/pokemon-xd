#ifndef MENUSAVELOAD_H
#define MENUSAVELOAD_H

#include "types.h"
#include "winSprite.h"

// Save report shared by the save-load window and the PDA.
class menuSaveload {
public:
    unsigned long mPlaytime; // Saved playtime or elapsed time during the save prompt.
    menuSaveload();
    ~menuSaveload();
    void SaveParameterSet();
    void Ctrl(tagWINDOW_WORK *window);
    void _menuClose(long menu);
};

#endif
