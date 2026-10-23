#include <WorldMap.h>
extern s16 lbl_80428080[];
extern u8 lbl_804EA6FC;
extern const f32 lbl_804EBEEC;
extern const f32 lbl_804EBF2C;
extern const f32 lbl_804EBF30;
extern const f32 lbl_804EBF34;
extern const f32 lbl_804EBEF8;
void WorldMap::TargetDebug(long index)
{
    s32 prev = mCamWork->_00;
    s16 id = lbl_80428080[index];
    mCamWork->_04 = prev;
    mCamWork->_00 = id;
    if (lbl_804EA6FC == 0) return;
    if (mCamWork->_04 == mCamWork->_00) return;
    SetCameraNextWork(&mIcons[id].pos, lbl_804EBF2C, lbl_804EBF30, lbl_804EBEEC, lbl_804EBF34);
    SetCameraSpeed(lbl_804EBEF8);
}
