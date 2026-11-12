#ifndef APP_EFFECTS_FILTER_H
#define APP_EFFECTS_FILTER_H

#include <GSeffect.h>
#include <dolphin/gx/GXStruct.h>

struct GSfilter;
struct _GSmodel;

class filterEffect : public GSeffect {
public:
    enum { SCREEN_FILTER = 0 };

    virtual bool _start();
    virtual void _stop();
    virtual bool _update(f32 delta);
    virtual ~filterEffect();
    void UpdateModulation();
    void InitModulation();
    void StopModulation();

    union {
        GSfilter *mFilter;
        _GSmodel *mModel;
    };
    GScolor mSavedColor;
    bool mOverridePE;
    bool mCombineModulation;
    bool mRestoreModulation;
    u8 mReserved;
    u8 mMode;
    GXColor mColor;
};

#endif
