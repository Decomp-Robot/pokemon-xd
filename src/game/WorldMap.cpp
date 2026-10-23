#include <WorldMap.h>

extern f32 lbl_804E7E90;
extern f32 lbl_804EA704;
extern const f32 lbl_804EBEEC;

/* WorldMap::SetCameraSpeed -- prime the camera blend from the default rate.
 * The call closes the previous move (elapsed time restarts from the blend
 * constant) and reloads the rate and increment from the tuned defaults,
 * so ControlCamera advances the blend parameter along the new move. */
void WorldMap::SetCameraSpeed(f32 speed)
{
    mCamWork->blend = lbl_804EBEEC;
    mCamWork->_10 = lbl_804EA704;
    mCamWork->_0C = lbl_804E7E90;
    mCamWork->_14 = lbl_804EBEEC;
}
