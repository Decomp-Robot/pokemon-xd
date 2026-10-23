#ifndef FIGHTENCOUNT_H
#define FIGHTENCOUNT_H

#include <types.h>

#ifdef __cplusplus
class FIGHT_ENCOUNT {
private:
    u16 mField;
    u16 mMessages[4];
    u8 mAddMode;
    u8 mAddPlace;
    u8 mAddID;
    u8 mSwapMode;
    u16 mSwapDeckNO;
    u8 mFightMode;
    u8 mTrainerKind;
    u8 mDestroyFlag;
    u8 mUnknownByte;
    unsigned long mRoundNo;
    bool mDirty;

public:
    bool getDirtyFlag() const;
    void setDirtyFlag(bool);
    void clear();
    u16 getMsg(u8) const;
    void setMsg(u8, u16);
    u16 getSwapDeckNO() const;
    void setSwapDeckNO(u16);
    void setRoundNo(unsigned long);
    u32 getRoundNo() const;
    void setDestroyFlag(u8);
    u32 getDestroyFlag() const;
    void setTrainerKind(u8);
    u8 getTrainerKind() const;
    void setFightMode(u8);
    u8 getFightMode() const;
    void setSwapMode(u8);
    u32 getSwapMode() const;
    void setAddID(u8);
    u8 getAddID() const;
    void setAddPlace(u8);
    u8 getAddPlace() const;
    void setAddMode(u8);
    u32 getAddMode() const;
    void setField(u16);
    u16 getField() const;
};
#else
typedef struct FIGHT_ENCOUNT FIGHT_ENCOUNT;
#endif

#ifdef __cplusplus
extern "C" {
#endif

u16 fightEncount(u16 encountId);
FIGHT_ENCOUNT *fightEncountGetStrObj();
#ifdef __cplusplus
void fightSoundOpenPlay(bool);
void fightSoundOpenPlayEnv(bool);
#else
void fightSoundOpenPlay(u8);
void fightSoundOpenPlayEnv(u8);
#endif
void fightEncountAnnihilationRecovery();

#ifdef __cplusplus
}
#endif

#endif
