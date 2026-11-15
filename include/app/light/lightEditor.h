#ifndef APP_LIGHT_LIGHTEDITOR_H
#define APP_LIGHT_LIGHTEDITOR_H

#include <GSgfx.h>

struct GSlight;

class LightEditor {
public:
    LightEditor();
    void init();
    void editUpdate();

private:
    void selectNextPeople();
    void editDirection(GSlight *light);
    void editPosition(GSlight *light);
    void editType(GSlight *light);
    void editColor(GSlight *light);
    void editCommand();
    void dispLightModel(GSlight *light, bool selected);
    void dispInfo(GSlight *light, long x, long y, bool selected);
    void disp();
    void dispCommand(GSgfxLayerID layer, long y);

    s8 mLightIndex;
    s8 mEditMode;
    s8 mEditItem;
    s8 mPositionMode;
    s8 mPeopleIndex;
    f32 mBlinkPhase;
};

#endif
