#ifndef RELIVE_HALL_H
#define RELIVE_HALL_H

#include <Pokemon.h>

class CReliveStage {
public:
    Pokemon *getVisitorPointer();
    Pokemon *getDancerPointer(long slot);
    void insertDancer(Pokemon &pokemon, long slot);
    void pickupDancer(Pokemon &pokemon, long slot);
};

class CReliveHall {
public:
    static CReliveHall *getPointer();
    static bool isOpen();
    CReliveStage *getStage(long stage);
    void init();
    void allocMakeBackup();
    void checkWithBackupSet();
};

extern "C" {
void reliveHallInfomationPokemonDP0(Pokemon *pokemon);
}

#endif
