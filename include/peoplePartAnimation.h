#ifndef PEOPLE_PART_ANIMATION_H
#define PEOPLE_PART_ANIMATION_H
#include <people.h>
#include <hsdRandom.h>
#include <time.h>
enum PeopleBiosAnimationFlag {
    PEOPLE_BIOS_DISABLE_BLINK = 0x10000,
    PEOPLE_BIOS_DISABLE_SPEECH = 0x20000
};
enum PeoplePartAnimationCount {
    PEOPLE_SPEECH_PART_COUNT = 7,
    PEOPLE_BLINK_PART_COUNT = 4
};

extern "C" {
void peoplePartAnimationSpeakStop(tagPeopleWork* people);
void peoplePartAnimationSpeakStart(tagPeopleWork* people);
void peoplePartAnimationBlinkStop(tagPeopleWork* people);
void peoplePartAnimationBlinkStart(tagPeopleWork* people);
void peoplePartAnimationBlink(tagPeopleWork* people);
void peoplePartAnimationBlinkOnce(tagPeopleWork* people);
}
s32 _peoplePartAnimationStartAnimation(_GSmodel* model, s8 partIndex, int animation,
                                      f32 frame, GSmodelAnimType type);
#endif
