#ifndef MENU_ORRE_H
#define MENU_ORRE_H

#include <types.h>
#include <winSprite.h>
#include <menuOrreFrames.h>
#include <menuPokemonStatus.h>

struct Pokemon;
class Hero;

// Orre colosseum team selection: the window in which the player picks the
// four Pokemon that represent them in the colosseum battle from their own
// party (temochi); the deck rows of the window show the opposing trainer's
// party.
class menuOrre {
public:
    menuOrre()
    {
        long i;

        mFloorPushed = 0;
        mFlashStep = 0;
        mTimer = 0.0f;
        mFrameCount = 0;
        for (i = 0; i < 13; i++) {
            mPokemonList[i] = 0;
        }
        mSelectCount = 0;
        mSelectList[0] = -1;
        mSelectList[1] = -1;
        mSelectList[2] = -1;
        mSelectList[3] = -1;
    }

    void Release();
    s32 Hook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void RightPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void LeftPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    u16 *GetDeckPokemonName(u16 index);
    u16 GetDeckPokemonFaceID(u16 index);
    u8 GetDeckPokemonLevel(u16 index);
    u8 GetDeckPokemonSex(u16 index);
    u16 GetDeckPokemonMochimono(u16 index);
    u16 *GetTemochiPokemonName(u16 index);
    u16 GetTemochiPokemonFaceID(u16 index);
    u8 GetTemochiPokemonLevel(u16 index);
    u8 GetTemochiPokemonSex(u16 index);
    u16 GetTemochiPokemonMochimono(u16 index);

    void Init();
    void InitSub();
    void Main();
    inline u8 AppendSelectedPokemon();
    inline long GetPokemonStatusContext()
    {
        return (long)&mSelectID;
    }
    void ControlCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void SetNumber(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    Pokemon *GetDeckPokemonPtr(u16 index);
    u8 GetTemochiMaxPokemonLevel();
    Pokemon *GetTemochiPokemonPtr(u16 index);
    Hero *GetHeroPtr(u8 trainer);
    void Ctrl(tagWINDOW_WORK *window);
    void Button(tagWINDOW_WORK *window);
    u8 ConfirmPokemon();
    s32 SelectPokemon(u16 id, bool alreadySelected);
    void CreatePokemonList();

    /* 0x00 */ u8 mMaxPokemonLevel;       // highest level in the player's party
    /* 0x01 */ u8 mReserved;
    /* 0x02 */ u16 mEncountNo;            // encounter the winner plays through
    /* 0x04 */ s16 mSelectCount;          // team size picked so far (0..4)
    /* 0x06 */ s16 mSelectList[4];        // Pokemon picked so far, -1 = empty
    /* 0x0E */ u16 mSelectID;             // Pokemon the cursor is on (0..5)
    /* 0x10 */ s16 mFrameCount;           // frame counter, wraps at 1000
    /* 0x12 */ u8 mFloorPushed;           // a floor was pushed for the selection
    /* 0x14 */ u16 mTrainerDataID;        // deck trainer of the encounter
    /* 0x16 */ u16 mFlashStep;            // flash step played so far (0..4)
    /* 0x18 */ f32 mTimer;                // flash step hold time
    /* 0x1C */ Pokemon *mPokemonList[13]; // party + deck Pokemon, id -> record
};

#ifdef __cplusplus
extern "C" {
#endif

// The team selection singleton and its request state.
extern menuOrre *pOrre;
extern u16 _deckNumber;
extern u16 _fightresult;

// Next Pokemon record handed to the face loader.
extern u16 _loadFaceCount;

// Module entry points.
void menuOrreInit(void);
void menuOrreExit(void);
void menuOrreMain(void);
u16 menuOrreColosseumGetResult(void);
void menuOrreColosseum(u16 encountNo);
s32 menuOrreMenuHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuOrreCtrl(tagWINDOW_WORK *window);
void menuOrreButton(tagWINDOW_WORK *window);

#ifdef __cplusplus
}
#endif

Pokemon *_GetTemochiPokemonPtr(u16 index);

#endif
