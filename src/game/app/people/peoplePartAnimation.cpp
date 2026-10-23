#include "peoplePartAnimation.h"

enum {
    PEOPLE_PART_ANIMATION_BLINK = 1,
    PEOPLE_PART_ANIMATION_SPEECH = 2,
    RANDOM_FRACTION_MASK = 0x7fff
};

void peoplePartAnimationSpeakStop(tagPeopleWork* people)
{
    GSpart* part;
    _GSmodel* model;
    int partIndex;
    tagPeopleInfoBios* bios;
    if (people && people->state.speaking) {
        model = people->model;
        bios = peopleInfoBiosGetPtr(people->state.infoId);
        for (partIndex = 0; partIndex < PEOPLE_SPEECH_PART_COUNT; partIndex++) {
            if (people->speechAnimations[partIndex] >= 0) {
                part = GSmodelGetPart(model, bios->speechParts[partIndex]);
                if (part) {
                    GSpartStopAnimIndex(part, people->speechAnimations[partIndex]);
                    GSpartFree(part);
                }
                people->speechAnimations[partIndex] = -1;
            }
        }
        people->state.speaking = false;
    }
}

void peoplePartAnimationSpeakStart(tagPeopleWork* people)
{
    bool started = false;
    if (people) {
        _GSmodel* model = people->model;
        tagPeopleInfoBios* bios = peopleInfoBiosGetPtr(people->state.infoId);
        if (!peopleBiosCheckFlag(people, PEOPLE_BIOS_DISABLE_SPEECH) && !people->state.speaking) {
            for (int partIndex = 0; partIndex < PEOPLE_SPEECH_PART_COUNT; partIndex++) {
                people->speechAnimations[partIndex] = _peoplePartAnimationStartAnimation(
                    model, bios->speechParts[partIndex], PEOPLE_PART_ANIMATION_SPEECH,
                    0.0f, GS_MODEL_ANIM_LOOP);
                if (people->speechAnimations[partIndex] >= 0) started = true;
            }
            if (started) {
                people->state.speaking = true;
                GSmodelSetKeepPartAnimFlag(model, false);
                people->speechTimer = 1.5f;
            }
        }
    }
}

void peoplePartAnimationBlinkStop(tagPeopleWork* people)
{
    if (people) {
        people->state.blinkEnabled = false;
        _GSmodel* model = people->model;
        if (model) GSmodelSetKeepPartAnimFlag(model, false);
    }
}

void peoplePartAnimationBlinkStart(tagPeopleWork* people)
{
    if (people && !peopleBiosCheckFlag(people, PEOPLE_BIOS_DISABLE_BLINK)) {
        people->state.blinkEnabled = true;
        u16 randomValue = HSD_Rand();
        people->state.blinkTimer = (randomValue & RANDOM_FRACTION_MASK) / 65536.0f;
    }
}

void peoplePartAnimationBlink(tagPeopleWork* people)
{
    if (!people) return;
    if (people->state.speaking) {
        people->speechTimer -= timeGetLastFrameSec();
        if (people->speechTimer < 0.0f) peoplePartAnimationSpeakStop(people);
    }
    if (!people->state.blinkEnabled) return;
    people->state.blinkTimer -= timeGetLastFrameSec();
    if (people->state.blinkTimer > 0.0f) return;

    u16 randomBits = HSD_Rand();
    int randomValue = randomBits & RANDOM_FRACTION_MASK;
    people->state.blinkTimer = 2.0f + randomValue / 32768.0f;
    peoplePartAnimationBlinkOnce(people);
}

void peoplePartAnimationBlinkOnce(tagPeopleWork* people)
{
    if (people) {
        _GSmodel* model = people->model;
        tagPeopleInfoBios* bios = peopleInfoBiosGetPtr(people->state.infoId);
        for (int partIndex = 0; partIndex < PEOPLE_BLINK_PART_COUNT; partIndex++) {
            (void)_peoplePartAnimationStartAnimation(
                model, bios->blinkParts[partIndex], PEOPLE_PART_ANIMATION_BLINK,
                0.0f, GS_MODEL_ANIM_ONCE);
        }
        GSmodelLinkTexAnimToAnim(model, false);
        GSmodelSetTexAnimIndex(model, PEOPLE_PART_ANIMATION_BLINK);
        GSmodelSetTexAnimType(model, GS_MODEL_ANIM_ONCE);
        GSmodelStartTexAnimation(model);
    }
}

s32 _peoplePartAnimationStartAnimation(_GSmodel* model, s8 partIndex, int animation,
                                      f32 frame, GSmodelAnimType type)
{
    s32 result = -1;
    if (partIndex >= 0) {
        GSpart* part = GSmodelGetPart(model, partIndex);
        if (!part) return -1;
        result = GSpartSetAnimIndex(part, animation, type, frame);
        GSpartFree(part);
    }
    return result;
}
