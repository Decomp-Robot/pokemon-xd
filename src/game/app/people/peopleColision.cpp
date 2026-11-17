#include <people.h>
#include <peopleCollision.h>
#include <GSgfxC.h>
#include <GScamera.h>
#include <GSres.h>
#include <hero.h>

extern "C" {
#include <dolphin/gx/GXEnum.h>
}

/*
 * People collision tests.  The line test sweeps a person's previous-to-current
 * movement against the collision geometry around a query segment; the
 * rectangle test is a simple containment test used by the script interface.
 */

const f32 collisionPrismHalfHeight = 50.0f;
const f32 defaultCollisionBallRadius = 4.0f;
const f32 collisionZero = 0.0f;
const f32 collisionStepLimit = 1.0f;
const f32 collisionEpsilon = 0.0001f;
bool _peopleGetCrossPoint(GSvec* crossPoint, GSvec* position, f32 radius,
                          GSvec* p1, GSvec* p2, GSvec* normal);
u8 getCpPointPoly(GSvec* crossPoint, GSvec* position, f32 radius,
                  GSvec* triangle, GSvec* normal);

extern "C" u8 peopleLineCheck(u32 group, u32 id, f32 x1, f32 z1, f32 x2, f32 z2,
                              f32 distance, u8 resolve)
{
#pragma unused(distance)
    tagPeopleWork* people;
    GSvec stepVector;
    GSvec start;
    GSvec end;
    GSvec previousPosition;
    GSvec position;
    GSvec movement;
    GSvec hit;
    GSvec correction;
    GSvec extended;
    GSvec closest;
    GSvec normal;
    GSvec direction;
    GSvec offset;
    tagPeopleInfoBios* info;
    f32 radius;
    f32 step;
    f32 length;
    f32 travelled;
    f32 fraction;
    CollisionDebugFlags* debugFlags;

    people = peopleGet(group, id);
    if (!people) return 0;
    position = *peopleBiosGetPosPtr(people);
    previousPosition = *peopleBiosGetPrevPosPtr(people);
    PSVECSubtract((const Vec*)&position, (const Vec*)&previousPosition, (Vec*)&movement);
    start.set(x1, position.y, z1);
    end.set(x2, position.y, z2);

    debugFlags = (CollisionDebugFlags*)GSresGetResource(0, RESOURCE_ID_COLLISION_DEBUG_FLAGS);
    if (debugFlags != 0 && debugFlags->drawCollision != 0) {
        GSgfxLayerSetActive(GSGFX_LAYER_DEFAULT);
        GScameraLoadActiveCameraMatrix(GSGFX_LAYER_DEFAULT);
        GSgfxCullMode(GX_CULL_NONE);
        GSgfxSetupRenderAndSelect(GSgfxVFBuildTemp(GX_POS_XYZ, GX_F32, GX_FALSE, GX_NRM_XYZ,
                                                   1, GX_RGBA8, 0, GX_TEX_S, GX_U8));
        GSgfxBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA);
        GSgfxZCompare(GX_TRUE, GX_LESS);
        GSgfxPixelMode(GX_TRUE, GX_TRUE, GX_TRUE);
        GSgfxAlphaCompare(GX_DISABLE, GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GSgfxSetPrimitiveType(GX_TRIANGLESTRIP);
        GSgfxBeginSurface(4);
        GSgfxVertex3f(x1, position.y - collisionPrismHalfHeight, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x1, position.y + collisionPrismHalfHeight, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, position.y - collisionPrismHalfHeight, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, position.y + collisionPrismHalfHeight, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxEndSurface();
    }

    info = peopleInfoBiosGetPtr(people->state.infoId);
    if (info != 0) {
        radius = peopleInfoBiosGetColBallSize(info);
    } else {
        radius = defaultCollisionBallRadius;
    }

    length = PSVECMag((const Vec*)&movement);
    if (length > collisionZero) {
        step = radius / length;
        if (step > collisionStepLimit) step = collisionStepLimit;
    } else {
        step = collisionStepLimit;
    }

    PSVECSubtract((const Vec*)&end, (const Vec*)&start, (Vec*)&direction);
    if (PSVECMag((const Vec*)&direction) < collisionEpsilon) return 0;
    PSVECNormalize((const Vec*)&direction, (Vec*)&direction);
    PSVECSubtract((const Vec*)&previousPosition, (const Vec*)&start, (Vec*)&offset);
    fraction = PSVECDotProduct((const Vec*)&offset, (const Vec*)&direction);
    PSVECScale((const Vec*)&direction, (Vec*)&offset, fraction);
    PSVECAdd((const Vec*)&offset, (const Vec*)&start, (Vec*)&closest);
    PSVECSubtract((const Vec*)&previousPosition, (const Vec*)&closest, (Vec*)&normal);

    travelled = collisionZero;
    while (travelled < collisionStepLimit) {
        fraction = travelled + step;
        if (fraction > collisionStepLimit) {
            fraction = collisionStepLimit;
        }
        PSVECScale((const Vec*)&movement, (Vec*)&stepVector, fraction);
        PSVECAdd((const Vec*)&previousPosition, (Vec*)&stepVector, (Vec*) &stepVector);
        if (_peopleGetCrossPoint(&closest, &stepVector, radius, &start, &end, &normal)) {
            if (resolve) {
                GScolsy2UtilGetPointExtentionLine(&extended, &closest,
                                                  radius + collisionEpsilon, &stepVector);
                if ((u8)GScolsys2HitCollision(&previousPosition, &extended,
                                              defaultCollisionBallRadius, &hit)) {
                    PSVECSubtract((const Vec*)&hit, (const Vec*)&extended, (Vec*)&correction);
                    PSVECAdd((const Vec*)&extended, (const Vec*)&correction, (Vec*)&extended);
                }
                peopleBiosSetPos(people, &extended);
                heroMoveSetLockFrame(1);
            }
            return 1;
        }
        travelled += step;
    }
    return 0;
}

bool _peopleGetCrossPoint(GSvec* crossPoint, GSvec* position, f32 radius,
                          GSvec* p1, GSvec* p2, GSvec* normal)
{
    GSvec triangle[3];

    triangle[0].x = p1->x;
    triangle[0].y = crossPoint->y - collisionPrismHalfHeight;
    triangle[0].z = p1->z;
    triangle[1].x = p2->x;
    triangle[1].y = crossPoint->y - collisionPrismHalfHeight;
    triangle[1].z = p2->z;
    triangle[2].x = p1->x;
    triangle[2].y = crossPoint->y + collisionPrismHalfHeight;
    triangle[2].z = p1->z;
    if (getCpPointPoly(crossPoint, position, radius, triangle, normal)) return true;

    triangle[0].x = p2->x;
    triangle[0].y = crossPoint->y - collisionPrismHalfHeight;
    triangle[0].z = p2->z;
    triangle[1].x = p2->x;
    triangle[1].y = crossPoint->y + collisionPrismHalfHeight;
    triangle[1].z = p2->z;
    if (getCpPointPoly(crossPoint, position, radius, triangle, normal)) return true;

    triangle[0].x = p2->x;
    triangle[0].y = crossPoint->y - collisionPrismHalfHeight;
    triangle[0].z = p2->z;
    triangle[1].x = p1->x;
    triangle[1].y = crossPoint->y - collisionPrismHalfHeight;
    triangle[1].z = p1->z;
    triangle[2].x = p1->x;
    triangle[2].y = crossPoint->y + collisionPrismHalfHeight;
    triangle[2].z = p1->z;
    if (getCpPointPoly(crossPoint, position, radius, triangle, normal)) return true;

    triangle[1].x = p1->x;
    triangle[1].y = crossPoint->y + collisionPrismHalfHeight;
    triangle[1].z = p1->z;
    triangle[2].x = p2->x;
    triangle[2].y = crossPoint->y + collisionPrismHalfHeight;
    triangle[2].z = p2->z;
    return getCpPointPoly(crossPoint, position, radius, triangle, normal);
}

u8 getCpPointPoly(GSvec* crossPoint, GSvec* position, f32 radius,
                    GSvec* triangle, GSvec* normal)
{
    GSvec closest;

    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, position) < collisionZero) {
        return false;
    }
    GScolsy2UtilGetCpPlanePoint(&closest, normal, triangle, position);
    if (PSVECSquareDistance((const Vec*)&closest, (const Vec*)position) >= radius * radius) {
        return false;
    }
    if (!GScolsy2UtilChkInTri((f32*)&closest, (f32*)triangle, normal)) {
        return false;
    }
    *crossPoint = closest;
    return true;
}

extern "C" u8 peopleRectCheck(u32 group, u32 id, f32 x1, f32 z1, f32 x2, f32 z2)
{
    tagPeopleWork* people;
    GSvec position;
    f32 minX;
    f32 maxX;
    f32 minZ;
    f32 maxZ;
    f32 topY;
    CollisionDebugFlags* debugFlags;

    people = peopleGet(group, id);
    if (!people) return 0;
    position = *peopleBiosGetPosPtr(people);

    debugFlags = (CollisionDebugFlags*)GSresGetResource(0, RESOURCE_ID_COLLISION_DEBUG_FLAGS);
    if (debugFlags != 0 && debugFlags->drawCollision != 0) {
        GSgfxLayerSetActive(GSGFX_LAYER_DEFAULT);
        GScameraLoadActiveCameraMatrix(GSGFX_LAYER_DEFAULT);
        GSgfxCullMode(GX_CULL_NONE);
        GSgfxSetupRenderAndSelect(GSgfxVFBuildTemp(GX_POS_XYZ, GX_F32, GX_FALSE, GX_NRM_XYZ,
                                                   1, GX_RGBA8, 0, GX_TEX_S, GX_U8));
        GSgfxBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA);
        GSgfxZCompare(GX_TRUE, GX_LESS);
        GSgfxPixelMode(GX_TRUE, GX_TRUE, GX_TRUE);
        GSgfxAlphaCompare(GX_DISABLE, GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GSgfxSetPrimitiveType(GX_TRIANGLESTRIP);
        GSgfxBeginSurface(10);
        GSgfxVertex3f(x1, position.y, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        topY = collisionPrismHalfHeight + position.y;
        GSgfxVertex3f(x1, topY, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x1, position.y, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x1, topY, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, position.y, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, topY, z2);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, position.y, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x2, topY, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x1, position.y, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxVertex3f(x1, topY, z1);
        GSgfxColor4u8(0, 0x80, 0xFF, 0xC0);
        GSgfxEndSurface();
    }

    if (x1 < x2) {
        minX = x1;
        maxX = x2;
    } else {
        minX = x2;
        maxX = x1;
    }
    if (z1 < z2) {
        minZ = z1;
        maxZ = z2;
    } else {
        minZ = z2;
        maxZ = z1;
    }
    if (minX <= position.x && position.x <= maxX &&
        minZ <= position.z && position.z <= maxZ) {
        return 1;
    }
    return 0;
}
