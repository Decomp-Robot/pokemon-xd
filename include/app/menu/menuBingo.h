#ifndef APP_MENU_MENUBINGO_H
#define APP_MENU_MENUBINGO_H

#include <types.h>
#include <Pokemon.h>
#include <app/bingo/bingoBios.h>

class cBingo : public bingo_bios {
public:
    struct PokemonPanel {
        Pokemon pokemon;
        u8 epoint;
        u8 stage;
    };

    enum { STAGE_COUNT = 4 };

    static PokemonPanel mPokemon[BINGO_PANEL_COUNT];
    int findPokemonByStage(u32 stageIndex);
    u8 getEpoint();

    u8 unk_000[0x122];
    u8 mMode;
    u8 unk_123[0x6];
    u8 mMball;
    u8 mEpoint;
    u8 unk_12B[0x21];
};

inline int cBingo::findPokemonByStage(u32 stageIndex)
{
    for (int panel = 0; panel < BINGO_PANEL_COUNT; panel++) {
        if (mPokemon[panel].stage == stageIndex + 1) {
            return panel;
        }
    }
    return -1;
}

inline u8 cBingo::getEpoint()
{
    int stage, epoint;
    for (stage = epoint = 0; stage < STAGE_COUNT; stage++) {
        int panel = findPokemonByStage(stage);
        if (panel == -1) {
            break;
        }
        epoint += mPokemon[panel].epoint;
    }
    return epoint + mEpoint;
}

extern cBingo gBingo;

extern "C" u8 menuBattleDeBingoGetEpoint(void);

#endif
