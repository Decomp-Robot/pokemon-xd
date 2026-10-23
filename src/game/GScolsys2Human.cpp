#include <GScolsys2Internal.h>
#include <hero.h>
#include <peopleTalk.h>
extern "C" {
#include <math.h>
}
extern "C" {
#include <dolphin/gx/GXEnum.h>
}

#include <people.h>

extern "C" {
void* peopleSearchID(s32 peopleId);
s32 peopleGetHumanID(s32 groupId, s32 resourceId);
bool gimmickBoxFixDistanceForPushBox(f32* reach, void* work, f32 dx, f32 dz);
u8 gimmickBoxIsBox(void* work);
GSvec* peopleBiosGetPosPtr(void* work);
void GSgfxSetLineWidth(f32 width);
void GSgfxSetPrimitiveType(s32 type);
void GSgfxDrawBeginSurface(s32 count);
void GSgfxDrawEndSurface(void);
}

static const s32 kCollisionCircleSectors = 16;
static const f32 kCollisionCircleAngleStep = (2.0f * M_PI) / kCollisionCircleSectors;

extern "C" f32 GScolsys2HumanGetHumanRadius(s32 index)
{
    GSCOLSYS2_HUMAN* human;
    GSCOLSYS2_HUMAN* h;
    GSCOLSYS2_FLOOR* floor;
    s32 ret;

    if (index < 0 || GSCOLSYS2_HUMAN_MAX <= index) {
        ret = GSCOLSYS2_INVALID_HUMAN;
    } else {
        floor = GScolsys2GetCurFloor();
        if (floor == 0) {
            ret = GSCOLSYS2_NO_CURRENT_FLOOR;
        } else {
            h = &floor->mHuman[index];
            if (!(h->mFlags & GSCOLSYS2_HUMAN_IN_USE)) {
                ret = GSCOLSYS2_INVALID_HUMAN;
            } else {
                human = h;
                ret = GSCOLSYS2_SUCCESS;
            }
        }
    }

    if (ret != 0) {
        return (0.0f);
    }
    return human->mRadius;
}

extern "C" void GScolsys2HumanDispColision(void)
{
    GSCOLSYS2_HUMAN* human;
    GSCOLSYS2_FLOOR* floor;
    s32 segment;
    floor = GScolsys2GetCurFloor();

    if (floor == 0) {
        return;
    }
    GSgfxLayerSetActive((GSgfxLayerID)2);
    GScameraLoadActiveCameraMatrix(GSGFX_LAYER_DEFAULT);
    GSgfxCullMode(GX_CULL_NONE);
    GSgfxSelectVertexFormat(GSgfxVFBuildTemp(GX_POS_XYZ, GX_F32, GX_FALSE, GX_NRM_XYZ,
                                           1, GX_RGBA8, 0, GX_TEX_S, GX_U8));
    GSgfxBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA);
    GSgfxPixelMode(GX_TRUE, GX_TRUE, GX_TRUE);
    GSgfxSetLineWidth((2.0f));
    for (human = floor->mHuman;
         human != &floor->mHuman[GSCOLSYS2_HUMAN_MAX]; human++) {
        GSvec position;
        f32 sine;
        f32 angle;
        f32 talkAngle;
        f32 talkRadius;

        if (!(human->mFlags & GSCOLSYS2_HUMAN_IN_USE)) {
            continue;
        }
        if (human->mFlags & GSCOLSYS2_HUMAN_DISABLED) {
            continue;
        }
        position.x = position.y = position.z = (0.0f);
        peopleGetPos(human->mGroupId, human->mResourceId, &position);
        GSgfxSetPrimitiveType(GX_TRIANGLEFAN);
        GSgfxDrawBeginSurface(kCollisionCircleSectors + 2);
        GSgfxVertex3f(position.x, (0.1f) + position.y, position.z);
        GSgfxColor4u8(0, 0x7F, 0x7F, 0xC0);
        for (segment = 0; segment < kCollisionCircleSectors + 1; segment++) {
            angle = kCollisionCircleAngleStep * (f32)segment;

            sine = sin(angle);
            f32 cosine = cos(angle);
            f32 radius = human->mRadius;

            GSgfxVertex3f(position.x + radius * cosine,
                          (0.1f) + position.y,
                          position.z + radius * sine);
            GSgfxColor4u8(0, 0x7F, 0x7F, 0xC0);
        }
        GSgfxDrawEndSurface();
        GSgfxSetPrimitiveType(GX_LINESTRIP);
        GSgfxDrawBeginSurface(kCollisionCircleSectors + 1);
        talkRadius = human->mRadius + peopleGetTalkDistance(human->mGroupId,
                                                          human->mResourceId);
        for (segment = 0; segment < kCollisionCircleSectors + 1; segment++) {
            talkAngle = kCollisionCircleAngleStep * (f32)segment;

            sine = sin(talkAngle);
            f32 cosine = cos(talkAngle);

            GSgfxVertex3f(position.x + talkRadius * cosine,
                          (0.15f) + position.y,
                          position.z + talkRadius * sine);
            GSgfxColor4u8(0x7F, 0x7F, 0xFF, 0xC0);
        }
        GSgfxDrawEndSurface();
    }
}

bool checkCollisionHeadToHead(GSCOLSYS2_HUMAN* a, GSCOLSYS2_HUMAN* b,
                             GSvec* outA, GSvec* outB)
{
    GSvec* positionA;
    tagPeopleWork* characterA;
    tagPeopleWork* characterB;
    f32 distance;
    f32 combinedRadius;
    f32 dx;
    f32 dz;
    u8 isPushBoxA;
    bool pushBoxDistanceFixed;

    characterA = static_cast<tagPeopleWork*>(peopleSearchID(peopleGetHumanID(a->mGroupId, a->mResourceId)));
    if (characterA == 0) {
        return 0;
    }
    positionA = peopleBiosGetPosPtr(characterA);
    if (positionA == 0) {
        return 0;
    }
    dx = outA->x - positionA->x;
    dz = outA->z - positionA->z;
    distance = sqrtf(dx * dx + dz * dz);
    combinedRadius = a->mRadius + b->mRadius;
    pushBoxDistanceFixed = !gimmickBoxFixDistanceForPushBox(&combinedRadius, characterA, dx, dz);
    isPushBoxA = gimmickBoxIsBox(characterA);
    if (isPushBoxA) {
        GSvec step;
        f32 gap = positionA->y + a->mHeight - (heroMoveGetHeroPos(&step), step.y);

        if (gap < (0.00001f) && gap > (-0.00001f) && !pushBoxDistanceFixed) {
            return 0;
        }
    } else if (positionA->y >= outA->y + b->mHeight || positionA->y + a->mHeight <= outA->y) {
        return 0;
    }
    if (distance >= combinedRadius) return 0;
    if (distance <= (0.0f)) {
        dx = (1.0f);
        distance = dx;
    }
    characterB = static_cast<tagPeopleWork*>(peopleSearchID(peopleGetHumanID(b->mGroupId, b->mResourceId)));
    if (characterB == 0) {
        return 0;
    }
    if (gimmickBoxIsBox(characterA) || gimmickBoxIsBox(characterA)) {
        f32 yB = peopleBiosGetPosPtr(characterB)->y;
        f32 yA = peopleBiosGetPosPtr(characterA)->y;
        f32 heightDifference = fabs(yA - yB);
        if (heightDifference > (2.0f)) return 0;
    }
    static f32 followerRadiusScale = 0.99f;
    if (heroGetFollower()) {
        if (characterB->isHero() && characterA->isFollower()) {
            combinedRadius *= followerRadiusScale;
        } else if (characterA->isHero() && characterB->isFollower()) {
            combinedRadius *= followerRadiusScale;
        }
    }
    if (distance >= combinedRadius) return 0;
    f32 pushScale = ((0.0001f) + combinedRadius) / distance;
    if (gimmickBoxIsBox(characterA) && pushBoxDistanceFixed) {
        f32 absDeltaZ = fabs(dz);
        f32 absDeltaX = fabs(dx);

        if (absDeltaX > absDeltaZ) {
            dx *= pushScale;
        } else {
            dz *= pushScale;
        }
    } else {
        dx *= pushScale;
        dz *= pushScale;
    }
    f32 resultX = positionA->x + dx;
    f32 resultZ = positionA->z + dz;
    f32 resultY = outA->y;
    if (outB != 0) {
        outB->x = resultX;
        outB->y = resultY;
        outB->z = resultZ;
    }
    return 1;
}

bool checkCollision(GSCOLSYS2_HUMAN* human, int self, GSvec* from, GSvec* to,
                   GSvec* out)
{
    GSvec resolved;
    GSvec pos;
    GSCOLSYS2_FLOOR* floor;
    s32 i;
    s32 passes;
    GSCOLSYS2_HUMAN* slot;
    GSvec* corrected = &resolved;

    pos = *to;
    floor = GScolsys2GetCurFloor();
    for (passes = 0; passes < GSCOLSYS2_HUMAN_PASSES; passes++) {
        slot = floor->mHuman;
        for (i = 0; i < GSCOLSYS2_HUMAN_MAX; i++, slot++) {
            if (i == self) continue;
            if (!(slot->mFlags & GSCOLSYS2_HUMAN_IN_USE)) continue;
            if (slot->mFlags & GSCOLSYS2_HUMAN_DISABLED) continue;
            if (checkCollisionHeadToHead(slot, human, &pos, corrected)) break;
        }
        if (i >= GSCOLSYS2_HUMAN_MAX) break;
        pos = resolved;
    }
    if (passes <= 0) return 0;
    f32 x = pos.x;
    f32 y = pos.y;
    f32 z = pos.z;
    if (out != 0) {
        out->x = x;
        out->y = y;
        out->z = z;
    }
    return 1;
}

extern "C" s32 GScolsys2HumanCollision(int index, GSvec* a, GSvec* b, GSvec* out)
{
    GSvec dir;
    GSvec start;
    GSvec at;
    GSCOLSYS2_HUMAN* human;
    GSCOLSYS2_FLOOR* floor;
    f32 total;
    f32 limit;
    f32 stepLen;
    f32 travelled;
    f32 next;

    s32 status;
    if (index < 0 || GSCOLSYS2_HUMAN_MAX <= index) {
        status = GSCOLSYS2_INVALID_HUMAN;
    } else {
        floor = GScolsys2GetCurFloor();
        if (floor == 0) {
            status = GSCOLSYS2_NO_CURRENT_FLOOR;
        } else {
            GSCOLSYS2_HUMAN* slot = &floor->mHuman[index];
            if (!(slot->mFlags & GSCOLSYS2_HUMAN_IN_USE)) {
                status = GSCOLSYS2_INVALID_HUMAN;
            } else {
                human = slot;
                status = GSCOLSYS2_SUCCESS;
            }
        }
    }
    if (status != 0) return status;
    total = PSVECDistance((const Vec *)a, (const Vec *)b);
    if (total > (0.0f)) {
        stepLen = human->mRadius / total;
        if (stepLen > 1.0f) {
            stepLen = 1.0f;
        }
    } else {
        stepLen = (0.0f);
    }
    PSVECSubtract((const Vec *)b, (const Vec *)a, (Vec *)&dir);
    limit = (1.0f);
    for (travelled = (0.0f); travelled < limit;
         travelled = travelled + stepLen) {
        next = travelled + stepLen;

        if (next > limit) {
            next = limit;
        }
        PSVECScale((const Vec *)&dir, (Vec *)&start, travelled);
        PSVECAdd((const Vec *)&start, (const Vec *)a, (Vec *)&start);
        PSVECScale((const Vec *)&dir, (Vec *)&at, next);
        PSVECAdd((const Vec *)&at, (const Vec *)a, (Vec *)&at);
        if (checkCollision(human, index, &start, &at, out)) {
            return GSCOLSYS2_COLLISION;
        }
        if (stepLen <= (0.0f)) {
            break;
        }
    }
    return GSCOLSYS2_NO_COLLISION;
}

extern "C" s32 GScolsys2HumanEnable(s32 index, s32 enable)
{
    GSCOLSYS2_HUMAN* human;
    GSCOLSYS2_HUMAN* h;
    GSCOLSYS2_FLOOR* floor;
    s32 ret;

    if (index < 0 || GSCOLSYS2_HUMAN_MAX <= index) {
        ret = GSCOLSYS2_INVALID_HUMAN;
    } else {
        floor = GScolsys2GetCurFloor();
        if (floor == 0) {
            ret = GSCOLSYS2_NO_CURRENT_FLOOR;
        } else {
            h = &floor->mHuman[index];
            if (!(h->mFlags & GSCOLSYS2_HUMAN_IN_USE)) {
                ret = GSCOLSYS2_INVALID_HUMAN;
            } else {
                human = h;
                ret = GSCOLSYS2_SUCCESS;
            }
        }
    }

    if (ret != 0) {
        return ret;
    }
    if ((u8)enable) {
        human->mFlags = human->mFlags & ~GSCOLSYS2_HUMAN_DISABLED;
    } else {
        human->mFlags = human->mFlags | GSCOLSYS2_HUMAN_DISABLED;
    }
    ret = GSCOLSYS2_SUCCESS;
    return ret;
}

extern "C" s32 GScolsys2HumanAddCurrentFloor(s32* slotIndex, void* character)
{
    GSCOLSYS2_FLOOR* floor = GScolsys2GetCurFloor();
    s32 used;
    u32 resourceId;
    const GSCOLSYS2_HUMAN* registration = static_cast<const GSCOLSYS2_HUMAN*>(character);

    if (floor == 0) {
        return GSCOLSYS2_NO_CURRENT_FLOOR;
    }
    for (used = 0; used < GSCOLSYS2_HUMAN_MAX; used++) {
        if (!(floor->mHuman[used].mFlags & GSCOLSYS2_HUMAN_IN_USE)) break;
    }
    if (GSCOLSYS2_HUMAN_MAX <= used) {
        return GSCOLSYS2_HUMAN_TABLE_FULL;
    }
    floor->mHuman[used].mGroupId = registration->mGroupId;
    floor->mHuman[used].mResourceId = registration->mResourceId;
    resourceId = (u32)registration->mResourceId;
    if (resourceId == HERO_RESOURCE_MANA ||
        resourceId == HERO_RESOURCE_FOLLOWER_SLOT_2 ||
        resourceId == HERO_RESOURCE_FOLLOWER_SLOT_3) {
        floor->mHuman[used].mRadius = (3.0f);
    } else {
        floor->mHuman[used].mRadius = registration->mRadius;
    }
    floor->mHuman[used].mHeight = registration->mHeight;
    floor->mHuman[used].mFlags = 0;
    floor->mHuman[used].mFlags |= GSCOLSYS2_HUMAN_IN_USE;
    *slotIndex = used;
    return GSCOLSYS2_SUCCESS;
}

extern "C" f32 GScolsys2HumanGetWalkHeight(GSvec* pos)
{
    GSCOLSYS2_WALK_HEIGHT heights[GSCOLSYS2_WALK_MAX];
    GSvec result;
    f32 originalHeight;
    f32 height;
    f32 highestSurface;
    f32 highestBelowStep;
    bool foundBelowStep;
    s32 heightCount;
    s32 heightIndex;

    result.x = pos->x;
    result.y = pos->y;
    result.z = pos->z;
    heightCount = GScolsys2WalkGetHeight(&result, heights);
    if (heightCount >= 2) {
    originalHeight = result.y;
    highestBelowStep = (-1000000.0f);
    highestSurface = highestBelowStep;
    foundBelowStep = false;
    for (heightIndex = 0; heightIndex < heightCount; heightIndex++) {
        height = heights[heightIndex].mHeight;
        if (highestSurface < height) {
            highestSurface = height;
        }
        if (height - originalHeight >= 10.0f) {
            continue;
        }
        if (highestBelowStep < height) {
            highestBelowStep = height;
            foundBelowStep = 1;
        }
    }
    if (foundBelowStep) {
        result.y = highestBelowStep;
    } else {
        result.y = highestSurface;
    }
    f32 rise = fabs(result.y - pos->y);
    if (rise >= (10.0f)) {
        f32 nearestHeight = heights[0].mHeight;
        f64 initialDistance = fabs(pos->y - nearestHeight);
        f32 nearestDistance = initialDistance;
        for (heightIndex = 1; heightIndex < heightCount; heightIndex++) {
            f32 candidateDistance = fabs(pos->y - heights[heightIndex].mHeight);
            height = heights[heightIndex].mHeight;

            if (nearestDistance >= candidateDistance) {
                if (nearestDistance > candidateDistance) {
                    nearestDistance = candidateDistance;
                    nearestHeight = height;
                } else if (nearestHeight < height) {
                    nearestHeight = height;
                }
            }
        }
        result.y = nearestHeight;
    }
    } else if (heightCount > 0) {
        result.y = heights[0].mHeight;
    } else {
        result.y = (0.0f);
    }
    return result.y;
}

extern "C" f32 GScolsys2HumanGetWalkHighest(GSvec* pos)
{
    GSCOLSYS2_WALK_HEIGHT heights[GSCOLSYS2_WALK_MAX];
    GSvec p;
    s32 count;
    s32 i;
    f32 highest;

    p.x = pos->x;
    p.y = pos->y;
    p.z = pos->z;
    count = GScolsys2WalkGetHeight(&p, heights);
    if (count >= 2) {
        highest = (-1000000.0f);
        for (i = 0; i < count; i++) {
            if (highest < heights[i].mHeight) {
                highest = heights[i].mHeight;
            }
        }
        return highest;
    } else {
        if (count > 0) {
            return heights[0].mHeight;
        }
        return (0.0f);
    }
}
