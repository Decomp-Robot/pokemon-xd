#include <types.h>
#include <place.h>
#include <GSflag.h>
#include <underTakerBios.h>

// Sentinel: the radar prints no place name for this message id.
enum { UNDER_TAKER_NO_PLACE_MESSAGE = 1 };

// Message ID of the place he is on, or the no-place message if none matches.
s32 underTakerBios::getPlaceMsg(void)
{
    u32 place = GSflagGet(gPlaceWork->mMiraboPlaceFlag);
    PLACE_WORK *work = gPlaceWork;

    if (work->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace == place) {
        return work->mPoint[PLACE_POINT_PYRITE_TOWN].mMessage;
    }
    if (work->mPoint[PLACE_POINT_REALGAM_TOWER].mPlace == place) {
        return work->mPoint[PLACE_POINT_REALGAM_TOWER].mMessage;
    }
    if (work->mPoint[PLACE_POINT_FIRST_SPOT].mPlace == place) {
        return work->mPoint[PLACE_POINT_FIRST_SPOT].mMessage;
    }
    if (work->mPoint[PLACE_POINT_SECOND_SPOT].mPlace == place) {
        return work->mPoint[PLACE_POINT_SECOND_SPOT].mMessage;
    }
    if (work->mPoint[PLACE_POINT_THIRD_SPOT].mPlace == place) {
        return work->mPoint[PLACE_POINT_THIRD_SPOT].mMessage;
    }
    if (work->mPoint[PLACE_POINT_OUTSKIRT_STAND].mPlace == place) {
        return work->mPoint[PLACE_POINT_OUTSKIRT_STAND].mMessage;
    }
    if (work->mPoint[PLACE_POINT_GATEON_PORT].mPlace == place) {
        return work->mPoint[PLACE_POINT_GATEON_PORT].mMessage;
    }
    return UNDER_TAKER_NO_PLACE_MESSAGE;
}
