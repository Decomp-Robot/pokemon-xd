#include <app/effectEditor/effectEditor.h>

FloorModuleInterface _floorModuleEffectViewer;

FloorModuleInterface* effectViewerGetFloorModule(void)
{
    return &_floorModuleEffectViewer;
}
