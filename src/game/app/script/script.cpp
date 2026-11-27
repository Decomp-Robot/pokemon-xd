#include <types.h>
#include <dolphin/pad.h>

#include <hero.h>
#include <heroBios.h>
#include <savedata.h>
#include <Pokemon.h>
#include <PokemonStatus.h>
#include <pokemonBios.h>
#include <GSinput.h>
#include <app/floor/floorCharacterBios.h>
#include <etctools.h>
#include <script.h>

#define ABS(x) ((x) > 0 ? (x) : -(x))

const int SCRIPT_PARTY_SIZE = 6;
const int SCRIPT_STICK_DEADZONE = 2;

enum ScriptButtonMask {
    SCRIPT_BUTTON_MASK = PAD_TRIGGER_Z | PAD_TRIGGER_R | PAD_TRIGGER_L |
                         PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y | PAD_BUTTON_MENU,
    SCRIPT_DPAD_MASK = PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT | PAD_BUTTON_DOWN | PAD_BUTTON_UP
};

extern "C" {

void scriptPlayPokemonNakigoe(u16 species)
{
    etctoolSetPokemonNakigoe(species);
}

u16 *scriptGetPokemonNickName(int slot)
{
    if (slot >= SCRIPT_PARTY_SIZE) {
        return 0;
    }
    Hero *hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    Pokemon *pokemon = heroBiosGetPokemonPtr(hero, slot);
    if (pokemon == 0) {
        return 0;
    }
    return (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_NICKNAME, 0);
}

int scriptGetDarkPointZeroPokemonNum(void)
{
    Hero *hero;
    Pokemon *pokemon;
    u16 slot;
    u16 count;

    count = 0;
    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    for (slot = 0; slot < SCRIPT_PARTY_SIZE; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, slot);
        if (pokemonCheckValid(pokemon) && pokemonBiosGetDarkFlag(pokemon) &&
            pokemonGetDp(pokemon) == 0.0f) {
            count++;
        }
    }
    return count;
}

u16 scriptGetDarkPokemonNum(void)
{
    Hero *hero;
    Pokemon *pokemon;
    u16 slot;
    u32 count;

    count = 0;
    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    for (slot = 0; slot < SCRIPT_PARTY_SIZE; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, slot);
        if (pokemonCheckValid(pokemon) && pokemonBiosGetDarkFlag(pokemon)) {
            count++;
        }
    }
    return count;
}

u16 scriptGetPokemonNum(void)
{
    Hero *hero;
    u16 slot;
    u16 count = 0;
    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    for (slot = 0; slot < SCRIPT_PARTY_SIZE; slot++) {
        if (pokemonCheckValid(heroBiosGetPokemonPtr(hero, slot))) {
            count++;
        }
    }
    return (u16)count;
}

int scriptFloorCharSetPos(unsigned long index, float x, float y, float z)
{
    GSvec pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    floorCharacterBiosSetPos(floorCharacterBiosGetCurrentPtr(index), &pos);
    return 1;
}

int scriptFloorCharSetDisp(unsigned long index, unsigned long visible)
{
    floorCharacterBiosSetVisibility(floorCharacterBiosGetCurrentPtr(index), (bool)visible);
    return 1;
}

int scriptIsTrigerPush(void)
{
    int result = 0;
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & SCRIPT_BUTTON_MASK) {
        result = 1;
    }
    return result;
}

int scriptIsButtonPush(void)
{
    int result = scriptIsMoveButtonPush();
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & SCRIPT_BUTTON_MASK) {
        result = 1;
    }
    return result;
}

int scriptIsMoveButtonPush(void)
{
    int result = 0;
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & SCRIPT_DPAD_MASK) {
        result = 1;
    }
    if (ABS(GSinputGetLeftStickXData(GS_INPUT_PORT_1, 0)) > SCRIPT_STICK_DEADZONE) {
        result = 1;
    }
    if (ABS(GSinputGetLeftStickYData(GS_INPUT_PORT_1, 0)) > SCRIPT_STICK_DEADZONE) {
        result = 1;
    }
    return result;
}

}
