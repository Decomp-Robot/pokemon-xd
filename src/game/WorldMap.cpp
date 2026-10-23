#include <WorldMap.h>

extern f32 lbl_804E7E90;
extern f32 lbl_804EA704;
extern const f32 lbl_804EBEEC;  /* 0.0f */
extern const f32 lbl_804EBEE8;  /* 2*pi */
extern const f32 lbl_804EBF20;  /* pi */
extern const f32 lbl_804EBF24;  /* 0.25f */

extern "C" long getRibbonCnt(WorldMap* map);

/* Index of the pad slot whose window contains angle, or -1 when the angle
 * falls outside every slot. The eight slots are spread evenly over the
 * turn (pi/4 apart), so the slot at angle 0 wraps around the end. */
s8 WorldMap::GetPadIndex3(f32 angle, long index, f32 width)
{
    bool inWindow = false;
    s8 padIndex = -1;
    f32 wrapEnd = lbl_804EBEE8 - width;
    f32 slotAngle = lbl_804EBF20 * (f32)index;
    f32 slotCenter = slotAngle * lbl_804EBF24;

    if (lbl_804EBEEC == (f32)index) {
        if (angle >= wrapEnd || angle < width) {
            inWindow = true;
        }
    } else if (angle >= slotCenter - width && angle < slotCenter + width) {
        inWindow = true;
    }
    if (inWindow) {
        padIndex = index;
    }
    return padIndex;
}

/* The loop re-reads the ribbon count each pass; the body left no code. */
void WorldMap::InitModel2D(f32 param)
{
    long i;

    CalcModel2D(param, true);
    for (i = 0; i < getRibbonCnt(this); i++) {
    }
}

/* Prime the camera blend from the default rate. */
void WorldMap::SetCameraSpeed(f32 speed)
{
    mCamWork->blend = lbl_804EBEEC;
    mCamWork->_10 = lbl_804EA704;
    mCamWork->_0C = lbl_804E7E90;
    mCamWork->_14 = lbl_804EBEEC;
}
