#ifndef MENU_PDA_DPM_H
#define MENU_PDA_DPM_H

#include <winSprite.h>
#include <menuPda.h>
#include <Pokemon.h>
#include <menuModel.h>

struct DPM_CONTENTS {
    CURSOR *mCursor;
};

class menuPdaDPM {
public:
    menuPdaDPM(CURSOR *cursor);
    ~menuPdaDPM();
    void Release();
    void LoadPokemon(CURSOR *cursor);
    void Init(CURSOR *cursor);
    CURSOR *Main(CURSOR *cursor);
    void Button(tagWINDOW_WORK *window);
    void Ctrl(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    void SetViewFlag(CURSOR *cursor);
    void DispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonRelive(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonRibbon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonPlace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void PokemonName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void PokemonSex(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void PokemonBall(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void _menuClose(long menuID);
    Pokemon *GetPokemonPointer(long index);

private:
    u32 mRnd;
    Pokemon mPokemon;
    MENU_MODEL mModel;
};

extern menuPdaDPM *_pDPM;

enum {
    DPM_SPRITE_POKEMON_BALL = 97,
    DPM_SPRITE_DARK_POKEMON_RIBBON = 98,
    DPM_SPRITE_DARK_POKEMON_RELIVE = 103,
    DPM_SPRITE_DISP_POKEMON_MODEL = 104,
    DPM_SPRITE_DARK_POKEMON_INFO = 105,
    DPM_SPRITE_DARK_POKEMON_PLACE = 106,
    DPM_SPRITE_POKEMON_SEX = 108,
    DPM_SPRITE_POKEMON_NAME = 109,
};

#endif
