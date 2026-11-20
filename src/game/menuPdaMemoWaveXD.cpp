#include <types.h>
#include <memo.h>
#include <GSinput.h>
#include <GSthread.h>
#include <dolphin/pad.h>
#include <PokemonData.h>
#include <GSsndPlayback.h>
#include "menuPdaMemoListXD.h"
#include "menuPdaMemoWaveXD.h"

pdaMemoNakigoe *_nakigoe;

void menuPdaMemoWaveHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    if (_nakigoe != NULL) {
        _nakigoe->NakigoeDispFunc(window, sprite);
    }
}

void menuPdaMemoWaveInit(void) {
    if (_nakigoe != NULL) {
        _nakigoe->Init();
    }
}

void menuPdaMemoWave(long index) {
    if (_nakigoe == NULL) {
        _nakigoe = new pdaMemoNakigoe;
    }
    _nakigoe->Init();
    _nakigoe->SetNakigoe(index);
    pdaMemoNakigoe *wave = _nakigoe;
    while (true) {
        if (!wave->NakigoeCheck()) {
            break;
        }
        const u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
        const u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
        if ((pressed & changed & PAD_BUTTON_A) != 0) {
            wave->SetNakigoe(index);
        } else {
            const u32 bChanged = GSinputButtonsChanged(GS_INPUT_PORT_1);
            const u32 bPressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
            if ((bPressed & bChanged & PAD_BUTTON_B) != 0) {
                break;
            }
        }
        GSthreadSwitch();
    }
}

void pdaMemoNakigoe::SetNakigoe(long index) {
    s32 voice = GetVocie(index);
    u16 soundID = (u16)voice;

    SetSE(soundID);
    DispRequest(GetWaveDispDataAddress(index), soundID);
}

void pdaMemoNakigoe::SetSE(unsigned long soundID) {
    GSsndPlay(soundID, 0, 0);
}

s32 pdaMemoNakigoe::GetVocie(long index) {
    if (memoDataGetCount(NULL) != 0) {
        PokemonSpeciesStats *stats = menuPdaMemoListGetPokemonDataPointer(index);
        if (stats != NULL) {
            return pokemonDataBiosGetVoice(stats);
        }
        return POKEMON_VOICE_ID_INVALID;
    }
    return POKEMON_VOICE_ID_INVALID;
}
