#include <pokecolo.h>

/* Country code this build reports; main() picks the display region from it. */
enum {
    POKECOLO_COUNTRY_CODE_USA = 1
};

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
