#ifndef RELIVE_HALL_H
#define RELIVE_HALL_H

#include <Pokemon.h>

class CReliveStage {
public:
    enum State {
        STATE_NO_TEMPO = 0,
        STATE_HAS_TEMPO = 1,
        STATE_MAX_TEMPO = 2,
        STATE_RELIVE_READY = 3
    };
    long getState();
    long getTempo();
    bool isReliveReady();
    long getDancerQuantity();
    Pokemon *getVisitorPointer();
    void pickupVisitor(Pokemon &pokemon);
    Pokemon *getDancerPointer(long slot);
    void insertDancer(Pokemon &pokemon, long slot);
    void pickupDancer(Pokemon &pokemon, long slot);

    Pokemon mDancers[4];
    Pokemon mVisitor;
    s8 mCenterDancer;
};

class CReliveHall {
public:
    static CReliveHall *getPointer();
    static bool isOpen();
    bool isReliveReadyPokemonExist();
    CReliveStage *getStage(long stage);
    void init();
    void allocMakeBackup();
    void checkWithBackupSet();
};

int reliveHallPokemonToAisyou(Pokemon *pokemon, Pokemon *partner);

extern "C" {
void reliveHallInfomationPokemonDP0(Pokemon *pokemon);
}

#endif
