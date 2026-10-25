#ifndef C_META_MENU_H
#define C_META_MENU_H

#include <types.h>
#include <CState.h>
#include <winSprite.h>
#include <menuInfoWindow.h>
#include <string.h>

extern "C" long menuMetaDefaults[];

#pragma pack(push, 4)
class CMetaMenu {
protected:
    enum AlphaFadeState { FADE_OPAQUE, FADE_OUT, FADE_TRANSPARENT, FADE_IN };
    enum WindowInfoSlot { WINDOW_INFO_DECIDE_SE = 1 };
    long mAlphaFadeFrames;
    long mAlphaFadeState;
    long mWindowInfo[5];
    long mLocalWindowIds[64];
    void (*mLocalWindowCallbacks[64])(CMetaMenu *, long);
    long *mLocalWindowTable;
    tagWINDOW_KEY mKeyInfo;
    long mKeyRepeatFrames;
    unsigned short mPreviousKeyOn;
    bool mTutorialInput;
    CInfoWindow mTutorialInfo;
    CState mState;
    u64 mAnimationTime;
    u8 reserved328[4];
    bool mTutorialKeysValid;
    short mTutorialKeys;

    void init(const long *info)
    {
        mAnimationTime = 0;
        mState.init();
        mAlphaFadeFrames = 0;
        mAlphaFadeState = FADE_OPAQUE;
        memset(mLocalWindowIds, 0, sizeof(mLocalWindowIds));
        memset(mLocalWindowCallbacks, 0, sizeof(mLocalWindowCallbacks));
        mLocalWindowTable = defautlLocalWindowOpenTable;
        if (info) {
            memcpy(mWindowInfo, info, sizeof(mWindowInfo));
        }
        mTutorialInput = false;
        mTutorialKeysValid = false;
        mTutorialKeys = 0;
        memset(&mKeyInfo, 0, sizeof(mKeyInfo));
        mKeyRepeatFrames = 0;
        mPreviousKeyOn = 0;
    }

    void setState(long state, long *work)
    {
        mState.mNext = state;
        memset(work, 0, sizeof(mState.mWork));
    }

    void setState(long state)
    {
        setState(state, mState.mWork);
    }

public:
    static long defautlLocalWindowOpenTable[];
    static CMetaMenu *pCurrentMenu;
    void drawHook();
    tagWINDOW_KEY *getKeyInfo();
    void aButtonWait();
    long actionMenu(unsigned long *messages, int count, long x, long y, long cursor);
    void waitAtTime(float time);
    long calcAnimationTimeRate0toX(float time, long count);
    float calcAnimationTimeRate0to1to0(float time);
    float calcAnimationTimeRate0to1(float time);
    void systemFadeOutKick(float time);
    void systemFadeOutKeepAfterFloorPush();
    void alphaFadeDraw();
    unsigned char getAlphaFadeAlpha() const;
    void alphaFadeOutAndWait();
    void alphaFadeUpdate();
    void alphaFadeOutKick();
    void alphaFadeInKick();
    bool isAlphaFadeEnd() const { return mAlphaFadeState == FADE_OPAQUE || mAlphaFadeState == FADE_TRANSPARENT; }
    unsigned short tutorialManage();
    void tutorialManageUpdate();
    void localWindowRemove(long windowID);
    void localWindowDrawAll(long from, long to);
    void localWindowDrawAll();
    virtual void drawAll() = 0;
    virtual bool isAnimationStateNone();
    virtual void updateAll();
    virtual void tutorialExitKick();
    void vsyncUpdate();
    void localWindowEntry(long value, long index, void (*callback)(CMetaMenu *, long));
    long localWindowSearch(long value);
    void localWindowSetTable(long *table);
    void localWindowCreateAll();
    void localWindowDeleteAll();
    void systemFadeOutSetAndWait(float time);
    static void systemFadeInKick();
    void systemFadeWaitToEnd();
};
#pragma pack(pop)

#endif
