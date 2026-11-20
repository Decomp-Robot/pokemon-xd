#include <types.h>
#include <heroMove.h>
#include <hero.h>
#include <place.h>
#include <esaba.h>
#include <GSflag.h>
#include <GSthread.h>
#include <hsdRandom.h>
#include <gameSetup.h>
#include <floorManager.h>
#include <WorldMap.h>
#include <winMsg.h>
#include <darkPokemon.h>
#include <undertaker.h>

#include <menu.h>

void _appearMirabo(void);

// Floors of the first two places; he skips a place while the hero is on it.
enum UnderTakerPlaceFloor {
    UNDER_TAKER_FIRST_PLACE_FLOOR = 113,
    UNDER_TAKER_SECOND_PLACE_FLOOR = 59
};

// Story flag required before the second place opens to the roaming.
#define UNDER_TAKER_SECOND_PLACE_FLAG 2257

// Messages he plays when he appears and when he leaves.
enum UnderTakerMessage {
    UNDER_TAKER_MSG_APPEAR = 16004,
    UNDER_TAKER_MSG_LEAVE = 16005
};

// Place record at the given index, NULL past the last one.
static inline PLACE_POINT *_underTakerGetPoint(int index)
{
    if (index >= PLACE_POINT_MAX) {
        return NULL;
    }
    return &gPlaceWork->mPoint[index];
}

// First place: its world-map landmark has to be known and the hero must not be
// standing on its floor already.
static inline bool _underTakerFirstPlaceOpen(void)
{
    u32 floor = floorGetCurrent();
    if (GSflagTest(worldmapXD_data[WORLD_MAP_DATA_INDEX_PYRITE_TOWN].mFlagID) && floor != UNDER_TAKER_FIRST_PLACE_FLOOR) {
        return true;
    }
    return false;
}

// Second place, which needs its own story flag on top of the landmark.
static inline bool _underTakerSecondPlaceOpen(void)
{
    u32 floor = floorGetCurrent();
    u32 mapFlag = worldmapXD_data[WORLD_MAP_DATA_INDEX_REALGAM_TOWER].mFlagID;
    if (GSflagTest(UNDER_TAKER_SECOND_PLACE_FLAG) && GSflagTest(mapFlag) &&
        floor != UNDER_TAKER_SECOND_PLACE_FLOOR) {
        return true;
    }
    return false;
}

// Poke Spot no pokemon is waiting in; writes 0 when every spot is taken.
static inline void _underTakerGetEmptySpot(u32 *place)
{
    int spot[ESABA_THIRD_SPOT_INDEX + 1];
    EsabaSpotArea placeID[ESABA_THIRD_SPOT_INDEX + 1] = {
        ESABA_FIRST_SPOT_AREA, ESABA_SECOND_SPOT_AREA, ESABA_THIRD_SPOT_AREA
    };
    int spotNum = 0;
    u16 roll;
    int i;

    for (i = ESABA_FIRST_SPOT_INDEX; i <= ESABA_THIRD_SPOT_INDEX; i++) {
        if (esabaGetPokemonID(i) == 0) {
            spot[spotNum++] = i;
        }
    }
    if (spotNum == 0) {
        *place = 0;
        return;
    }
    roll = (u16)HSD_Rand() % spotNum;
    *place = placeID[spot[roll]];
}

// Story progress inside the window in which he stays away.
static inline bool _underTakerInProgress(void)
{
    u32 progress = GSflagGet(ESABA_PROGRESS_FLAG);
    if (progress >= flag_etc[gPlaceWork->mProgressEntryMin].progressValue &&
        progress <= flag_etc[gPlaceWork->mProgressEntryMax].progressValue) {
        return true;
    }
    return false;
}

// Put him on the place and play his appearance message.
static inline void _underTakerAppearPlace(u32 place)
{
    GSflagSet(gPlaceWork->mMiraboPlaceFlag, place);
    GSflagSet(gPlaceWork->mStayFlag, 0);
    u16 random = HSD_Rand();
    GSflagSet(gPlaceWork->mRandomFlag, (u8)(random % 2));
    heroMoveInitEvent();
    heroMoveSetColision(1);
    GSthreadSwitch();
    menuIntUnderTakerMenuOpen();
    winMsgOpen(WINMSG_WINDOW_FIELD, UNDER_TAKER_MSG_APPEAR, true, true);
    winMsgClose(true);
    menuIntUnderTakerMenuClose();
    heroMoveTermEvent();
}

// Clear the place he is on and announce that he left.
static inline void _underTakerLeavePlace(void)
{
    GSflagSet(gPlaceWork->mMiraboPlaceFlag, 0);
    GSflagSet(gPlaceWork->mStepFlag, 0);
    GSflagSet(gPlaceWork->mPlaceFlag, 0);
    menuIntUnderTakerMenuClose();
    heroMoveInitEvent();
    heroMoveSetColision(1);
    winMsgOpen(WINMSG_WINDOW_FIELD, UNDER_TAKER_MSG_LEAVE, true, true);
    winMsgClose(true);
    heroMoveTermEvent();
}

void underTakerMenuResore(void)
{
}

// Place he is on, 0 for none.
int underTakerGetMiraboPlace(void)
{
    return GSflagGet(gPlaceWork->mMiraboPlaceFlag);
}

// Footstep callback of the roaming: counts the steps that decide whether he
// appears, stays or leaves.
void underTakerCallBackFunc(long context, FootStepCounter counter, long steps)
{
    u32 level;
    u32 step;
    u16 down;
    u16 up;
    u16 range;
    u16 roll;
    u32 stay;

    if (!GSflagTest(gPlaceWork->mStartFlag)) {
        return;
    }
    if (GSflagTest(gPlaceWork->mStopFlag)) {
        return;
    }
    if (!darkpokemonCheckUnderTaker()) {
        return;
    }
    if (GSflagGet(gPlaceWork->mMiraboPlaceFlag) == 0) {
        step = (u16)GSflagGet(gPlaceWork->mStepFlag);
        step += (u16)steps;
        if ((u16)step >= gPlaceWork->mStepMax) {
            up = gPlaceWork->mUpRate;
            down = up + gPlaceWork->mDownRate;
            range = gPlaceWork->mRateRange;
            roll = (u16)HSD_Rand() % range;
            if (roll < up) {
                level = (u16)GSflagGet(gPlaceWork->mPlaceFlag) + 1;
                if ((u16)level >= gPlaceWork->mLevelMax - 1) {
                    if (_underTakerInProgress()) {
                        level--;
                    } else {
                        _appearMirabo();
                    }
                }
                GSflagSet(gPlaceWork->mPlaceFlag, (u16)level);
            } else if (roll < down) {
                level = (u16)GSflagGet(gPlaceWork->mPlaceFlag);
                if (level != 0) {
                    level--;
                }
                GSflagSet(gPlaceWork->mPlaceFlag, (u16)level);
            }
            step = (step - gPlaceWork->mStepMax) & 0xFFFF;
        }
        GSflagSet(gPlaceWork->mStepFlag, (u16)step);
    } else if (_underTakerInProgress()) {
        _underTakerLeavePlace();
    } else {
        stay = (u16)GSflagGet(gPlaceWork->mStayFlag);
        stay += (u16)steps;
        if ((u16)stay >= gPlaceWork->mStayMax) {
            _underTakerLeavePlace();
        }
        GSflagSet(gPlaceWork->mStayFlag, (u16)stay);
    }
}

// Clear the place when he has to disappear on a script event.
void underTakerMiraboDisAppear(void)
{
    GSflagSet(gPlaceWork->mMiraboPlaceFlag, 0);
    GSflagSet(gPlaceWork->mStepFlag, 0);
    GSflagSet(gPlaceWork->mPlaceFlag, 0);
    menuIntUnderTakerMenuClose();
}

// Weighted appearance: roll one of the places and check it is legal now.
void _appearMirabo(void)
{
    PLACE_POINT *point;
    u32 place;
    u32 total = 0;
    u16 roll;
    u16 sum;
    u32 floor;
    int i;

    i = 0;
    for (;;) {
        point = _underTakerGetPoint(i);
        if (point == NULL) {
            break;
        }
        total += point->mWeight;
        i++;
    }
    roll = (u16)HSD_Rand() % (u16)total;
    sum = 0;
    i = 0;
    for (;;) {
        point = _underTakerGetPoint(i);
        if (point == NULL) {
            break;
        }
        sum += point->mWeight;
        if (roll < sum) {
            place = point->mPlace;
            break;
        }
        i++;
    }
    floor = floorGetCurrent();
    if (place == gPlaceWork->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace) {
        if (!_underTakerFirstPlaceOpen()) {
            _underTakerGetEmptySpot(&place);
        }
    } else if (place == gPlaceWork->mPoint[PLACE_POINT_REALGAM_TOWER].mPlace) {
        if (!_underTakerSecondPlaceOpen()) {
            place = _underTakerFirstPlaceOpen() ? gPlaceWork->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace : 0;
            if (place == 0) {
                _underTakerGetEmptySpot(&place);
            }
        }
    } else if (place == gPlaceWork->mPoint[PLACE_POINT_FIRST_SPOT].mPlace) {
        if (esabaGetPokemonID(ESABA_FIRST_SPOT_INDEX) != 0 || floor == place) {
            place = _underTakerFirstPlaceOpen() ? gPlaceWork->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace : 0;
        }
    } else if (place == gPlaceWork->mPoint[PLACE_POINT_SECOND_SPOT].mPlace) {
        if (esabaGetPokemonID(ESABA_SECOND_SPOT_INDEX) != 0 || floor == place) {
            place = _underTakerFirstPlaceOpen() ? gPlaceWork->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace : 0;
        }
    } else if (place == gPlaceWork->mPoint[PLACE_POINT_THIRD_SPOT].mPlace) {
        if (esabaGetPokemonID(ESABA_THIRD_SPOT_INDEX) != 0 || floor == place) {
            place = _underTakerFirstPlaceOpen() ? gPlaceWork->mPoint[PLACE_POINT_PYRITE_TOWN].mPlace : 0;
        }
    }
    if (place != 0) {
        _underTakerAppearPlace(place);
    } else {
        GSflagSet(gPlaceWork->mStepFlag, 0);
        GSflagSet(gPlaceWork->mPlaceFlag, (u16)(gPlaceWork->mLevelMax - 2));
    }
}

// Debug placement choices, beginning with the none entry; the first two places
// are listed only while they are closed to the roaming.
u32 undertakerGetPlace(u32 *places)
{
    u32 place[PLACE_POINT_MAX];
    PLACE_POINT *point;
    u32 count = 0;
    int i;

    if (GSflagTest(gPlaceWork->mStartFlag) && !GSflagTest(gPlaceWork->mStopFlag) &&
        darkpokemonCheckUnderTaker()) {
        i = 0;
        for (;;) {
            point = _underTakerGetPoint(i);
            if (point == NULL) {
                break;
            }
            place[i] = point->mPlace;
            i++;
        }
        places[count++] = 0;
        if (!_underTakerFirstPlaceOpen()) {
            places[count++] = place[PLACE_POINT_PYRITE_TOWN];
        }
        if (!_underTakerSecondPlaceOpen()) {
            places[count++] = place[PLACE_POINT_REALGAM_TOWER];
        }
        for (i = ESABA_FIRST_SPOT_INDEX; i <= ESABA_THIRD_SPOT_INDEX; i++) {
            if (esabaGetPokemonID(i) == 0) {
                places[count++] = place[i + PLACE_POINT_FIRST_SPOT];
            }
        }
    }
    return count;
}

// Debug placement of the roaming.
void undertakerSetPlace(u32 place)
{
    _underTakerAppearPlace(place);
}

// GSflag id that enables the roaming.
long undertakerGetStartFlagID(void)
{
    return gPlaceWork->mStartFlag;
}
