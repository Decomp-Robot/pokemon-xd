#include <pokecolo.h>
#include <gamedata.h>
#include <savedata.h>
#include <GSAPI/GSflag/GSflag.h>

GamedataSaveMetadata *_getSaveSystemDataPtr(void);

enum {
    PLAY_TIME_COUNTING = 1,
    PLAY_TIME_SUSPENDED_FLAG = 781
};

static const f64 PLAY_TIME_MAX = 3599940.0;
static const f32 MICROSECONDS_PER_MILLISECOND = 1000.0f;
static const f32 MILLISECONDS_PER_SECOND = 1000.0f;

/* ROM versions this build knows about: the one a normal configuration starts
   with and the one an expanded-disc build reports. */
enum {
    POKECOLO_ROM_VERSION_DEFAULT = 0,
    POKECOLO_ROM_VERSION_EX = 4
};

/* Play-time accounting state: the play-time gate only advances the timer once
   counting has been started. */
u8 _playTimeCountStarted;
/* Game mode, returned by pokecoloGetMode(). */
u8 _mode;

/* Version of the ROM configuration the game was booted with. */
s32 _RomVer;

u64 _frameTime;

void pokecoloStartCountPlayTime(u8 started)
{
    _playTimeCountStarted = started;
}

void pokecoloSetPlayTime(f64 playTime)
{
    GamedataSaveMetadata *systemData = _getSaveSystemDataPtr();
    if (systemData != NULL) {
        gamedatasaveBiosSetPlaytime(systemData, playTime);
    }
}

unsigned long pokecoloGetPlayTimeValue(void)
{
    return (unsigned long)pokecoloGetPlayTime();
}

f64 pokecoloGetPlayTime(void)
{
    GamedataSaveMetadata *systemData = _getSaveSystemDataPtr();
    if (systemData != NULL) {
        return gamedatasaveBiosGetPlaytime(systemData);
    }
    return 0.0;
}

void pokecoloCountPlayTime(void)
{
    GamedataSaveMetadata *systemData = _getSaveSystemDataPtr();
    if (systemData != NULL && _playTimeCountStarted == PLAY_TIME_COUNTING &&
        GSflagGet(PLAY_TIME_SUSPENDED_FLAG) == 0) {
        f64 playTime = gamedatasaveBiosGetPlaytime(systemData);
        playTime += pokecoloGetFrameTime();
        if (playTime >= PLAY_TIME_MAX) {
            playTime = PLAY_TIME_MAX;
        }
        gamedatasaveBiosSetPlaytime(systemData, playTime);
    }
}

GamedataSaveMetadata *_getSaveSystemDataPtr(void)
{
    void *saveData = savedataBiosGetNowSavedataPtr();
    if (saveData != NULL) {
        GamedataSave *gameData = (GamedataSave *)savedataBiosGetGamedataPtr(saveData);
        if (gameData != NULL) {
            return gamedatasaveBiosGetPtr(gameData);
        }
    }
    return NULL;
}

s32 pokecoloGetCountryCode(void)
{
    return POKECOLO_COUNTRY_CODE_USA;
}

/* Whether the inserted disc is the expanded-disc build rather than the
   original game. */
bool _pokecoloCheckExDisc(void)
{
    return pokecoloGetRomVersion() == POKECOLO_ROM_VERSION_EX;
}

void pokecoloInitRomVersion(void)
{
    _RomVer = POKECOLO_ROM_VERSION_DEFAULT;
}

s32 pokecoloGetRomVersion(void)
{
    return _RomVer;
}

f32 pokecoloGetFrameTime(void)
{
    return (f32)_frameTime / MICROSECONDS_PER_MILLISECOND / MILLISECONDS_PER_SECOND;
}
