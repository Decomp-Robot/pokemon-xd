#include <winSprite.h>

class menuPdaDPM;
struct DPM_CONTENTS;

class menuPdaDPM {
public:
    void Button(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    void Ctrl(tagWINDOW_WORK *window);
    void PokemonBall(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonRibbon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonRelive(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void DarkPokemonPlace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void PokemonSex(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
    void PokemonName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents);
};

extern menuPdaDPM *_pDPM;

// Sprite item selectors dispatched by menuPdaDPMCallBack.
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

extern "C" long menuPdaDPMCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
extern "C" void menuPdaDPMButton(tagWINDOW_WORK *window);
extern "C" long menuPdaDPMCursor(tagWINDOW_WORK *window);
extern "C" long menuPdaDPMCtrl(tagWINDOW_WORK *window);

extern "C" long menuPdaDPMCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (_pDPM == 0) {
        return 0;
    }
    DPM_CONTENTS *contents = (DPM_CONTENTS *)window->mArg;
    switch (sprite->mItemID) {
    case DPM_SPRITE_DISP_POKEMON_MODEL:
        _pDPM->DispPokemonModel(window, sprite, contents);
        break;
    case DPM_SPRITE_DARK_POKEMON_RIBBON:
        _pDPM->DarkPokemonRibbon(window, sprite, contents);
        break;
    case DPM_SPRITE_DARK_POKEMON_INFO:
        _pDPM->DarkPokemonInfo(window, sprite, contents);
        break;
    case DPM_SPRITE_DARK_POKEMON_RELIVE:
        _pDPM->DarkPokemonRelive(window, sprite, contents);
        break;
    case DPM_SPRITE_DARK_POKEMON_PLACE:
        _pDPM->DarkPokemonPlace(window, sprite, contents);
        break;
    case DPM_SPRITE_POKEMON_NAME:
        _pDPM->PokemonName(window, sprite, contents);
        break;
    case DPM_SPRITE_POKEMON_SEX:
        _pDPM->PokemonSex(window, sprite, contents);
        break;
    case DPM_SPRITE_POKEMON_BALL:
        _pDPM->PokemonBall(window, sprite, contents);
        break;
    }
    return 0;
}

extern "C" void menuPdaDPMButton(tagWINDOW_WORK *window)
{
    if (_pDPM != 0) {
        _pDPM->Button(window);
    }
}

extern "C" long menuPdaDPMCursor(tagWINDOW_WORK *window)
{
    if (_pDPM == 0) {
        return 0;
    }
    _pDPM->Cursor(window);
    return 0;
}

extern "C" long menuPdaDPMCtrl(tagWINDOW_WORK *window)
{
    if (_pDPM == 0) {
        return 0;
    }
    _pDPM->Ctrl(window);
    return 0;
}
