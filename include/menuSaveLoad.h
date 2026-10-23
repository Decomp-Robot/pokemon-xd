#ifndef MENUSAVELOAD_H
#define MENUSAVELOAD_H

#include "types.h"
#include "winSprite.h"

// Save/report flow shared by the save-load window (menu id 0x64) and the PDA.
// menuSaveLoadCtrl drives the window steps; the report object it hands around
// carries the save target the message printer reads back.
class menuSaveload {
public:
    unsigned long mTarget; // 0x00: value the report passes to msgctrlSetValue.
    void SaveParameterSet();
    void Ctrl(tagWINDOW_WORK *window);
};

// FlashMenu is the report object menuReport creates, runs the save flow on and
// destroys again; its destructor releases the object memory.
class FlashMenu : public menuSaveload {
public:
    ~FlashMenu();
};

class GSstreamFileOutput : public FlashMenu {
public:
    GSstreamFileOutput();
};

#endif
