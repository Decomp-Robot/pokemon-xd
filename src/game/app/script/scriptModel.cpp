#include <scriptModel.h>
#include <floor.h>
#include <GSres.h>
#include <GSmodel.h>
#include <GSpart.h>
#include <GSparticle.h>
#include <GSthread.h>
#include <people.h>
#include <peopleSave.h>

static signed char scriptModelCounter;
static const unsigned long scriptModelIDBase = 0x7FFE0000;

void scriptModelSetFractionalFramesFlag(unsigned long resourceID, bool enabled)
{
    _GSmodel* model = (_GSmodel*)GSresGetResource(floorGetCurrentGroupID(), resourceID);
    GSmodelSetForceUseFractionalFramesFlag(model, enabled);
}

void scriptModelPartPaticle(unsigned long modelID, unsigned long particleID, long partIndex)
{
    _GSmodel* model = (_GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelID);
    GSpart* part = GSmodelGetPart(model, partIndex);
    _GSparticleBank* bank = (_GSparticleBank*)GSresGetResource(floorGetCurrentGroupID(), particleID);
    _GSparticleGen* generator = GSparticleCreateGenerator(bank, 0, false);
    GSparticleAttachGeneratorToGSpart(generator, part, GS_ATTACH_TRANSFORM, false, true, false);
    GSpartFree(part);
}

void scriptModelDeleteParticle(unsigned long particleID, bool killFamily)
{
    _GSparticleBank* bank = (_GSparticleBank*)GSresGetResource(floorGetCurrentGroupID(), particleID);
    GSparticleDestroyAllGenerators(bank, killFamily);
}

void scriptModelSetParticle(unsigned long modelID, unsigned long particleID, u8)
{
    _GSmodel* model = (_GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelID);
    _GSparticleBank* bank = (_GSparticleBank*)GSresGetResource(floorGetCurrentGroupID(), particleID);
    GSmodelLinkToGSparticleBank(model, bank);
    GSmodelSetGSparticleLinkAttachMode(model, GS_ATTACH_POSITION_ROTATION);
}

int scriptModelDetach(unsigned long resourceID)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleDetach(groupID, resourceID, false);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (!model) return 0;
        GSmodelDetachFromGSpart(model, true);
    }
    return 1;
}

int scriptModelAttach(unsigned long resourceID, unsigned long parentID, long partIndex)
{
    _GSmodel* model;
    _GSmodel* parent;
    GSpart* part;
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleAttach(groupID, resourceID, groupID, parentID, partIndex);
    } else {
        model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (!model) return 0;
        parent = (_GSmodel*)GSresGetResource(groupID, parentID);
        if (!parent) return 0;
        part = GSmodelGetPart(parent, partIndex);
        GSmodelAttachToGSpart(model, part, GS_ATTACH_TRANSFORM, false, true);
        GSpartFree(part);
    }
    return 1;
}

int scriptModelWaitMotion(unsigned long resourceID, long wait)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        return peopleWaitSyncMotion(groupID, resourceID, (bool)wait);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (!model) return 0;
        while (true) {
            if (GSmodelHasAnimationEnded(model)) return 0;
            if (wait) GSthreadSwitch();
            else break;
        }
        return 1;
    }
}

void scriptModelStopMotion(unsigned long resourceID)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleStopMotion(groupID, resourceID);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (model) GSmodelStopAnimation(model);
    }
}

int scriptModelSetEndedMotion(unsigned long resourceID, long motion, bool loop)
{
    _GSmodel* model = (_GSmodel*)GSresGetResource(floorGetCurrentGroupID(), resourceID);
    if (!model) {
        return 0;
    } else {
        while (true) {
            if (GSmodelHasAnimationEnded(model)) break;
            GSthreadSwitch();
        }
        GSmodelSetAnimIndex(model, motion);
        GSmodelSetAnimFrame(model, 0.0f);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetTexAnimIndex(model, motion);
        GSmodelSetTexAnimFrame(model, 0.0f);
        GSmodelSetTexAnimRate(model, 0.5f);
        if (loop) GSmodelSetAnimType(model, GS_MODEL_ANIM_LOOP);
        else GSmodelSetAnimType(model, GS_MODEL_ANIM_ONCE);
        GSmodelStartAnimation(model);
        return 1;
    }
}

void scriptModelSetMotionRate(unsigned long resourceID, long motion, long frame, float rate, long loop)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleSetMotionForce(groupID, resourceID, motion, frame, (bool)loop);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (model) {
            GSmodelSetAnimIndex(model, motion);
            GSmodelSetAnimFrame(model, frame);
            GSmodelSetAnimRate(model, rate);
            GSmodelSetTexAnimIndex(model, motion);
            GSmodelSetTexAnimFrame(model, frame);
            GSmodelSetTexAnimRate(model, rate);
            if (loop) GSmodelSetAnimType(model, GS_MODEL_ANIM_LOOP);
            else GSmodelSetAnimType(model, GS_MODEL_ANIM_ONCE);
            GSmodelStartAnimation(model);
        }
    }
}

void scriptModelSetMotion(unsigned long resourceID, long motion, long frame, long loop)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleSetMotionForce(groupID, resourceID, motion, frame, (bool)loop);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (model) {
            GSmodelSetAnimIndex(model, motion);
            GSmodelSetAnimFrame(model, frame);
            GSmodelSetAnimRate(model, 0.5f);
            GSmodelSetTexAnimIndex(model, motion);
            GSmodelSetTexAnimFrame(model, frame);
            GSmodelSetTexAnimRate(model, 0.5f);
            if (loop) GSmodelSetAnimType(model, GS_MODEL_ANIM_LOOP);
            else GSmodelSetAnimType(model, GS_MODEL_ANIM_ONCE);
            GSmodelStartAnimation(model);
        }
    }
}

void scriptModelSetDisp(unsigned long resourceID, long display)
{
    unsigned long groupID = floorGetCurrentGroupID();
    if (peopleGetHumanID(groupID, resourceID)) {
        peopleSetDisp(groupID, resourceID, (bool)display);
    } else {
        _GSmodel* model = (_GSmodel*)GSresGetResource(groupID, resourceID);
        if (model) GSmodelSetVisibility(model, (bool)display);
    }
}

void scriptModelClose(unsigned long resourceID)
{
    peopleClose(floorGetCurrentGroupID(), resourceID, false);
}

unsigned long scriptModelOpen(unsigned long resourceID)
{
    unsigned long modelID = scriptModelCounter++ | scriptModelIDBase;
    unsigned long humanID = peopleOpen(floorGetCurrentGroupID(), modelID, resourceID);
    if (!humanID) return 0;
    GSmodelSetBoundCheck(peopleBiosGetModelPtr(peopleSearchID(humanID)), false);
    return modelID;
}

void scriptModelInit(void)
{
    scriptModelCounter = 0;
}
