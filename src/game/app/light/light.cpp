#include <light.h>
#include <app/light/lightEditor.h>
#include <GSlight.h>
#include <GSres.h>
#include <GSthread.h>
#include <GSinput.h>
#include <floorBios.h>
#include <floorManager.h>
#include <app/floor/floorModule.h>
#include <dolphin/pad.h>

static const unsigned char LIGHT_MODULE_PRIORITY = 128;

static FloorModuleInterface _floorModuleLight;
static unsigned long _hModuleLight;
static u8 _lightFlag;
static GSlight* _eventGSlight;
static u8 _lightDebugMode;
static LightEditor* _lightEditor;

GSlight* lightGetLight(int index)
{
    u32 resource;
    long count;
    floorGetLightInfo(&resource, &count);
    if (index < count) {
        return (GSlight*)GSresGetResourceAllGroup(resource | index);
    } else if (index == count) {
        return _eventGSlight;
    }
    return 0;
}

int lightGetLightSize(void)
{
    long count = 0;
    floorGetLightInfo(0, &count);
    if (_eventGSlight) {
        count++;
    }
    return count;
}

u8 lightIsDebugMode(void)
{
    return _lightDebugMode;
}

void lightSetDebugMode(u8 enable)
{
    _lightDebugMode = enable;
    if (enable) {
        GSthreadBlockGroup(GSfloorGetCurrentFloorGroupID());
    } else {
        GSthreadUnblockGroup(GSfloorGetCurrentFloorGroupID());
    }
}

static const char* _lightGetModuleName()
{
    return "light";
}

static void _lightFinalize(unsigned int, FloorEnterMode)
{
    if (_eventGSlight) {
        GSlightFree(_eventGSlight);
        _eventGSlight = 0;
    }
    _lightFlag = 0;
    _lightDebugMode = 0;
}

static void _lightUpdate(unsigned int, FloorEnterMode)
{
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1) & PAD_BUTTON_B) {
        if (_lightDebugMode) {
            GSinputClearButtonData(GS_INPUT_PORT_1, PAD_BUTTON_B);
            _lightDebugMode = 0;
            GSthreadUnblockGroup(GSfloorGetCurrentFloorGroupID());
        }
    }
    if (_lightDebugMode) {
        _lightEditor->editUpdate();
    }
}

static void _lightInitialize(unsigned int, FloorEnterMode)
{
    _lightFlag = 0;
    _lightDebugMode = 0;
    _lightEditor->init();
}

void lightInit(void)
{
    FloorModuleInterface_clear(&_floorModuleLight);
    _floorModuleLight.initialize = _lightInitialize;
    _floorModuleLight.update = _lightUpdate;
    _floorModuleLight.finalize = _lightFinalize;
    _floorModuleLight.getModuleName = _lightGetModuleName;
    _hModuleLight = floorRegisterModule(LIGHT_MODULE_PRIORITY, 0, &_floorModuleLight);
    _lightDebugMode = 0;
    _lightFlag = 0;
    _eventGSlight = 0;
    _lightEditor = new LightEditor;
}
