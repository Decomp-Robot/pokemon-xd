#include <GSmsg.h>
#include <Pokemon.h>
#include <gamedata.h>
#include <hero.h>
#include <pokemonBios.h>
#include <types.h>
#include <waza.h>
#include <libc/memory.h>

// The block the GBA is sent holds the doc of every waza of every Pokemon of the
// party: four waza per Pokemon, one doc of 80 bytes for each of them.
static const s32 GBA_POKEMON_MAX = 6;
static const s32 GBA_MOVE_MAX = 4;
static const s32 GBA_WAZA_DOC_SIZE = 80;
// Marks the end of a GBA string, and the break between the two lines of a doc.
static const u16 GBA_STRING_END = 0xFFFF;
static const u8 GBA_WAZA_LINE_BREAK = 0xFE;
static const u8 GBA_WAZA_DOC_END = 0xFF;

extern "C" u32 itemBallDataBiosGetSnatchSnatchWzxDataId(WazaData *waza);

extern "C" long GetWazaDocGBA(u8 *destination, u16 moveId);
static long convline(u8 *destination, const u16 **source);

extern "C" long wazaconv(Hero *hero, u8 *destination)
{
    s32 pokemonSlot;
    s32 moveSlot;

    for (pokemonSlot = 0; pokemonSlot < GBA_POKEMON_MAX; pokemonSlot++) {
        Pokemon *pokemon = heroBiosGetPokemonPtr(hero, pokemonSlot);

        for (moveSlot = 0; moveSlot < GBA_MOVE_MAX; moveSlot++) {
            u16 moveId = pokemonBiosGetPokemonWazaDataId(pokemon, moveSlot);
            long result = GetWazaDocGBA(destination, moveId);

            if (result != 0) {
                return result;
            }
            destination += GBA_WAZA_DOC_SIZE;
        }
    }
    return 0;
}

extern "C" long GetWazaDocGBA(u8 *destination, u16 moveId)
{
    WazaData *waza = wazaDataBiosGetPtr(moveId);

    memset(destination, 0, GBA_WAZA_DOC_SIZE);
    if (waza != 0) {
        const u16 *message =
            GSmsgGetStringPtr(itemBallDataBiosGetSnatchSnatchWzxDataId(waza));

        destination += convline(destination, &message);
        if (*message == GBA_STRING_END) {
            *destination++ = GBA_WAZA_LINE_BREAK;
            // The second line starts behind the terminator and the byte that
            // follows it.
            message = (const u16 *)((const u8 *)message + 3);
            destination += convline(destination, &message);
        }
        *destination = GBA_WAZA_DOC_END;
    }
    return 0;
}

// Copies one line of the message behind source into line, with the length a doc
// has room for, and codes it for the GBA.
static long convline(u8 *destination, const u16 **source)
{
    u16 line[GBA_WAZA_DOC_SIZE + 1];
    s32 length = 0;

    while (**source != 0 && **source != GBA_STRING_END) {
        if (length < GBA_WAZA_DOC_SIZE) {
            line[length++] = **source;
        }
        (*source)++;
    }
    line[length] = 0;
    return GScharConvToGBACode(destination, line, (s32)gamedataGetStatus(0, GAMEDATA_STATUS_LANGUAGE));
}
