#ifndef C_MENU_GLOBAL_STATE_H
#define C_MENU_GLOBAL_STATE_H

#include <types.h>
#include <menuPokemonCursor.h>
#include <CState.h>

class CMenuGlobalState {
public:
    enum enumCallFrom {
        CALL_FROM_PC_BOX,
        CALL_FROM_RELIVE_HALL,
        CALL_FROM_HOLOGRAM,
        CALL_FROM_RELIVE_HALL_TUTORIAL
    };

    void init();

    enumCallFrom mCallFrom;
    long mMode;
    long mResult;
    CMenuPokemonCursor mCursor;
    bool mTutorial;
    bool mExit;
    bool mCancelExit;
    u8 _15f;
    CState mState;
};

#endif
