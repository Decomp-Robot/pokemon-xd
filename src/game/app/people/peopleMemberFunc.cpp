#include <people.h>
#include <peopleInfoBios.h>

extern "C" float peopleViewerGetPlaySpeedRate(tagPeopleWork* people);

namespace {

enum {
    HERO_OBJ_ID_STYLE0 = 0x16C90400,
    HERO_OBJ_ID_STYLE1 = 0x20250400,
    HERO_OBJ_ID_STYLE2 = 0x169F0400,
    HERO_OBJ_ID_STYLE0_FOLLOWER = 0x169E0400,
    HERO_OBJ_ID_STYLE1_FOLLOWER = 0x1EE20400,
    HERO_OBJ_ID_STYLE2_FOLLOWER = 0x16A00400,
    HERO_OBJ_ID_MANA = 0x16C70400,
    HERO_OBJ_ID_GIRL_FOLLOWER = 0x195B0400,
    HERO_OBJ_ID_KLEIN = 0x1A7C0400,
    HERO_OBJ_ID_KLEIN_FOLLOWER = 0x19540400,
};

} // namespace

u8 tagPeopleWork::getMoveType()
{
    return state.moveType;
}

int tagPeopleWork::getRunMotion()
{
    if (runMotionIndex >= 0)
    {
        int motion = runMotionIndex;
        if (!((u8)GSmodelIsNullAnimation(model, motion)))
            return motion;
    }
    int motion = walkMotionIndex;
    if (motion >= 0 && !((u8)GSmodelIsNullAnimation(model, motion)))
        return motion;
    return PEOPLE_ANIM_WAIT;
}

int tagPeopleWork::getWalkMotion()
{
    if (walkMotionIndex >= 0)
    {
        int motion = walkMotionIndex;
        if (!((u8)GSmodelIsNullAnimation(model, motion)))
            return motion;
    }
    int motion = runMotionIndex;
    if (motion >= 0 && !((u8)GSmodelIsNullAnimation(model, motion)))
        return motion;
    return PEOPLE_ANIM_WAIT;
}

void tagPeopleWork::setAnimType(GSmodelAnimType type)
{
    if (model)
        GSmodelSetAnimType(model, type);
}

void tagPeopleWork::setAnimFrame(float frame)
{
    if (!model)
        return;
    if (frame < 0.0f)
        frame = 0.0f;
    GSmodelSetAnimFrame(model, frame);
}

void tagPeopleWork::setAnimBlend(int baseIndex, int blendIndex)
{
    if (!model)
        return;
    if ((u8)GSmodelIsNullAnimation(model, baseIndex))
        return;
    if ((u8)GSmodelIsNullAnimation(model, blendIndex))
        setAnimIndex(baseIndex);
    else
        GSmodelSetBlendAnimIndex(model, baseIndex, blendIndex);
}

void tagPeopleWork::setAnimIndex(int index)
{
    if (model)
        GSmodelSetAnimIndex(model, index);
}

void tagPeopleWork::setAnimRate(float rate, bool scaleByPlaySpeed)
{
    if (!model)
        return;
    if (checkFlags(PEOPLE_FLAG_DISABLE_ANIM_RATE_UPDATE))
        return;
    if (scaleByPlaySpeed)
        rate *= peopleViewerGetPlaySpeedRate(this);
    GSmodelSetAnimRate(model, rate);
}

u8 tagPeopleWork::isAnimLast() const
{
    if (model == NULL)
        return 0;
    else
        return GSmodelHasAnimationEnded(model);
}

unsigned long tagPeopleWork::getFlags() const
{
    return state.flags;
}

u8 tagPeopleWork::checkFlags(unsigned long flags) const
{
    return peopleBiosCheckFlag(const_cast<tagPeopleWork*>(this), flags);
}

bool tagPeopleWork::isWalkStartMotion() const
{
    tagPeopleInfoBios* info = peopleInfoBiosGetPtr(state.infoId);
    if (!info)
        return false;
    if (info->noWalkStartMotion)
        return false;

    s32 index;
    s32 blendIndex;
    GSmodelGetAnimIndex(model, reinterpret_cast<u32*>(&index), reinterpret_cast<u32*>(&blendIndex));
    if (blendIndex != -1)
        return false;

    if (index == waitMotionIndex || index == walkStartMotionIndex || index == runStartMotionIndex)
        return true;
    return false;
}

bool tagPeopleWork::isAlwayBlendChara() const
{
    if (state.infoId == HERO_OBJ_ID_STYLE0 ||
        state.infoId == HERO_OBJ_ID_STYLE0_FOLLOWER ||
        state.infoId == HERO_OBJ_ID_STYLE2 ||
        state.infoId == HERO_OBJ_ID_STYLE2_FOLLOWER ||
        state.infoId == HERO_OBJ_ID_STYLE1 ||
        state.infoId == HERO_OBJ_ID_STYLE1_FOLLOWER ||
        state.infoId == HERO_OBJ_ID_MANA ||
        state.infoId == HERO_OBJ_ID_GIRL_FOLLOWER ||
        state.infoId == HERO_OBJ_ID_KLEIN ||
        state.infoId == HERO_OBJ_ID_KLEIN_FOLLOWER) {
        return true;
    }
    return false;
}
