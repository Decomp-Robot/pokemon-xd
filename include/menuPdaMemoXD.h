#ifndef MENU_PDA_MEMO_XD_H
#define MENU_PDA_MEMO_XD_H

#include <cursorBios.h>
#include <winSprite.h>

struct MEMO_CONTENTS {
    CURSOR *mCursor;
};

class menuPdaMemoXD {
public:
    menuPdaMemoXD(CURSOR *cursor) { Init(cursor); }
    ~menuPdaMemoXD() { Release(); }
    CURSOR *Main(CURSOR *cursor);
    void Button(tagWINDOW_WORK *window);
    void Ctrl(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    void SetModelAngle(s16 page);
    void OokisaCameraControl();
    void CameraControl();
    void Release();
    void LoadPokemon(CURSOR *cursor);
    void Init(CURSOR *cursor);
    long TaskOpen();
    void TaskClose(tagWINDOW_WORK *window, bool sync);
    bool TaskCloseCheck();
    void _menuClose(long menuID, bool sync);

    void PokemonHeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents);
    void PokemonWeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents);
    void PokemonSyuzoku(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents);
    void PokemonAisyou(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents);
    void SetJoutaiSprite(tagWINDOW_WORK *window, unsigned short joutai);
    u16 GetAisyouIndex(unsigned short index);
    void PokemonTokuseiSetumei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot);
    void PokemonTokusei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot);
    void PokemonType(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot);
    void PokemonName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents);

    void SetPage(s32 page) {
        mPrevPage = mPage;
        mPage = page;
    }

    MEMO_CONTENTS mContents;
    u32 _04;
    // Shared by the menu task and window callbacks.
    volatile s32 mPage;
    s32 mPrevPage;
    s32 mMenuID;
    f32 mAngle[3];
    f32 mZoom;
    u8 _24[196];
};

extern menuPdaMemoXD *pMemo;

extern "C" {
void menuPdaMemoBackHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaMemoXDHeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaMemoXDWeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPdaMemoXDGetItem(void);
long menuPdaMemoXDHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
CURSOR *menuPdaMemoXDMain(CURSOR *cursor);
void menuPdaMemoXDButton(tagWINDOW_WORK *window);
long menuPdaMemoXDCursor(tagWINDOW_WORK *window);
long menuPdaMemoXDCtrl(tagWINDOW_WORK *window);
}

#endif
