#ifndef RELIVE_HALL_H
#define RELIVE_HALL_H

#include <Pokemon.h>

enum ReliveHallAisyou {
    RELIVE_HALL_AISYOU_MISSING = -1,
    RELIVE_HALL_AISYOU_POOR = 0,
    RELIVE_HALL_AISYOU_GOOD = 1,
    RELIVE_HALL_AISYOU_BEST = 2,
    RELIVE_HALL_AISYOU_UNKNOWN = 123
};

enum ReliveHallDebugMode {
    RELIVE_HALL_DEBUG_PARTIAL_PURIFICATION = 0,
    RELIVE_HALL_DEBUG_READY_VISITORS = 1,
    RELIVE_HALL_DEBUG_MAX_TEMPO = 2
};

class CReliveStage {
public:
    static const long DANCER_SLOT_NONE = -1;
    static const long DANCER_MAX = 4;
    static const long TEMPO_MAX = 96;
    static const long PASSION_MAX_WITHOUT_BONUS = 192;
    enum State {
        STATE_NO_TEMPO = 0,
        STATE_HAS_TEMPO = 1,
        STATE_MAX_TEMPO = 2,
        STATE_RELIVE_READY = 3
    };
    State getState();
    bool isLugiaReliveReady();
    bool isReliveReady();
    Pokemon *heroWalkReliveCallBack(long steps, bool canRelive);
    bool isPokemonZokuseiAllUnique(bool withVisitor);
    long getTempo();
    long getPassionWoBonus();
    bool isBonusGet();
    long getPassion();
    void pickupVisitor(Pokemon &pokemon);
    void insertDancer(Pokemon &pokemon, long slot);
    void pickupDancer(Pokemon &pokemon, long slot);
    void exchangeVisitor(Pokemon &pokemon);
    void exchangeDancer(Pokemon &pokemon, long slot);
    long getDancerQuantity();
    Pokemon *getDancerPointer(long slot);
    void putVisitor(Pokemon &pokemon);
    Pokemon *getVisitorPointer();
    void init();

    Pokemon mDancers[DANCER_MAX];
    Pokemon mVisitor;
    s8 mCenterDancer;
};

class CReliveHall {
public:
    static const long STAGE_MAX = 9;
    static CReliveHall *getPointer();
    static bool isOpen();
    bool isReliveReadyDarkLugiaExist();
    bool isReliveReadyPokemonExist();
    long getReliveGiveStageQuantity();
    long getReliveReadyVisitorQuantity();
    Pokemon *heroWalkReliveCallBack(long steps);
    void debugPokemonSetup(long mode);
    CReliveStage *getStage(long stage);
    void init();
    void allocMakeBackup();
    void checkWithBackupSet();

    CReliveStage mStages[STAGE_MAX];
};

ReliveHallAisyou reliveHallPokemonToAisyou(Pokemon *pokemon, Pokemon *partner);

extern "C" {
void reliveHallInfomationPokemonDP0(Pokemon *pokemon);
void reliveHallDebugPokemonSetup(long mode);
void reliveHallCallBackInit();
}

#endif
