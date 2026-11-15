#include <app/recovery/recoveryEvent.h>
#include <GSsnd.h>
#include <sound.h>
#include <types.h>

#include <fade.h>
#include <floor.h>
#include <floorBgmMask.h>
#include <GSres.h>
#include <GSmodel.h>
#include <GSpart.h>
#include <GSthread.h>
#include <hero.h>
#include <heroBios.h>
#include <heroMove.h>
#include <people.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <savedata.h>
#include <script.h>
#include <scriptModel.h>

extern "C" void recoveryPokemon(void);
extern "C" u8 revoceryCatchPokemonBall(u16 slot);
extern "C" u8 peopleMoveCheck(unsigned long group, unsigned long id, u8 wait);

s32 _recoveryPauseBGM(unsigned long time);
void _recoveryEventSetME(unsigned long soundID, unsigned long time, unsigned char volume);

unsigned long _recoveryTestBallTable[] = {
    0x02E50400, 0x02E60400, 0x02E50400, 0x02EC0400,
    0x02B70400, 0x18740400, 0x02E70400, 0x02C10400,
    0x18730400, 0x02EA0400, 0x02ED0400, 0x18720400,
    0x02E90400, 0
};

enum {
    RECOVERY_MUSIC = 968,
    RECOVERY_BALL_SE = 25,
    RECOVERY_FINISH_MOTION = 6,
    RECOVERY_PERSON_HERO = 100,
    RECOVERY_PARTY_SIZE = 6
};

extern "C" void recoveryEventCommon(long steps)
{
    recoveryPokemon();
    fadeSet(FADE_OUT_BLACK, 1.0f);
    fadeCheck(1);
    soundOpenPlayME(RECOVERY_MUSIC, 0, 127);
    fadeSet(FADE_IN_BLACK, 1.0f);
    fadeCheck(1);
    heroMovePoolCounter(FOOT_STEP_COUNTER_RECOVERY, steps);
}

extern "C" void recoveryEventPC(unsigned long personGroup, unsigned long personID,
                                unsigned long machineID, long *ballParts, long *motions)
{
    GSvec ballPosition(0.0f, 10.0f, 0.0f);
    GSvec personRotation;
    _GSmodel *balls[RECOVERY_PARTY_SIZE];
    GSpart *ballPart;
    s32 bgmID;
    _GSmodel *machine;
    tagPeopleWork *person;
    Hero *hero;
    unsigned long partyCount;
    bool turnPerson;
    FloorBgmMask *bgmMask;
    unsigned long slot;

    bgmMask = floorSound_getBgmMaskObj();
    machine = (_GSmodel *)GSresGetResource(floorGetCurrentGroupID(), machineID);
    person = peopleGet(personGroup, personID);
    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    partyCount = heroGetPokeomCount(hero);
    turnPerson = person != 0;

    for (slot = 0; slot < partyCount; slot++) {
        balls[slot] = floorOpenObject(_recoveryTestBallTable[revoceryCatchPokemonBall((u16)slot)]);
        GSmodelSetVisibility(balls[slot], false);
    }
    if (turnPerson) {
        GSmodelGetRotation(person->model, &personRotation);
        peopleRotateToTarget(personGroup, personID, floorGetCurrentGroupID(), machineID, 1.0f);
        peopleMoveCheck(personGroup, personID, 1);
    }
    if (bgmMask) {
        bgmMask->stopMask();
    }
    bgmID = _recoveryPauseBGM(500);
    for (slot = 0; slot < partyCount; slot++) {
        pokemonAllKaihuku(heroBiosGetPokemonPtr(hero, (u16)slot));
        ballPart = GSmodelGetPart(machine, ballParts[slot]);
        GSpartGetTransform(ballPart, &ballPosition, 0, 0);
        GSpartFree(ballPart);
        GSmodelSetPosition(balls[slot], &ballPosition);
        GSmodelSetVisibility(balls[slot], true);
        _recoveryEventSetSE(RECOVERY_BALL_SE);
        scriptWaitTime(0.5f);
    }
    for (slot = 0; slot < partyCount; slot++) {
        scriptModelSetMotion(machineID, motions[slot], 0, 0);
    }
    _recoveryEventSetME(RECOVERY_MUSIC, 0, 255);
    while (GSmodelIsAnimating(machine)) {
        GSthreadSwitch();
    }
    scriptModelSetMotion(machineID, motions[RECOVERY_FINISH_MOTION], 0, 1);
    for (slot = 0; slot < partyCount; slot++) {
        if (balls[slot]) {
            GSmodelSetVisibility(balls[slot], false);
            GSmodelFree(balls[slot]);
        }
    }
    scriptWaitTime(0.7f);
    soundContinueBGMEX(bgmID, 1000, 0);
    if (turnPerson && personGroup != 0 && personID != RECOVERY_PERSON_HERO) {
        peopleRotateToAngle(personGroup, personID, personRotation.y, 1.0f);
        peopleMoveCheck(personGroup, personID, 1);
    }
}

// Pauses the BGM the recovery event interrupts and returns the BGM id that
// recoveryEventPC resumes when the event is over.
s32 _recoveryPauseBGM(unsigned long time)
{
    return soundPauseBGMEX((s32)time, 0);
}

// Plays the sound effect of the recovery event.
void _recoveryEventSetSE(unsigned long soundID)
{
    GSsndPlay((long)soundID, 0, 0);
}

// Opens the music stream the recovery event plays in its scene.
void _recoveryEventSetME(unsigned long soundID, unsigned long time, unsigned char volume)
{
    soundOpenPlayME((s32)soundID, (s32)time, volume);
}
