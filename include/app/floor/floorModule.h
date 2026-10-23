#ifndef INCLUDE_APP_FLOOR_FLOORMODULE_H
#define INCLUDE_APP_FLOOR_FLOORMODULE_H

#include <GSAPI/GSthread/GSthread.h>

enum FloorStateMode {
    FLOOR_STATE_MODE_NONE = 0,
    FLOOR_STATE_MODE_FIELD = 1,
    FLOOR_STATE_MODE_FIGHT = 2,
    FLOOR_STATE_MODE_SAVE = 3
};
enum FloorEnterMode { FLOOR_ENTER_NORMAL = 0 };
struct GSthread;
struct FloorModuleInterface {
    void (*create)(unsigned int, FloorEnterMode);
    void (*initialize)(unsigned int, FloorEnterMode);
    void (*update)(unsigned int, FloorEnterMode);
    void (*updateThread)(GSthread*);
    void (*finalize)(unsigned int, FloorEnterMode);
    void (*free)(unsigned int, FloorEnterMode);
    const char* (*getModuleName)();
    void (*makeStateData)(FloorStateMode, void*, unsigned long);
    void (*restoreStateData)(FloorStateMode, void*, unsigned long);
    unsigned long (*getStateSize)(FloorStateMode);
    unsigned long (*getUID)();
};
extern "C" {
void FloorModuleInterface_clear(FloorModuleInterface* interface);
unsigned long floorRegisterModule(unsigned char type, unsigned long groupID, FloorModuleInterface* interface);
}

#endif
