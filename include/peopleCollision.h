#ifndef PEOPLECOLLISION_H
#define PEOPLECOLLISION_H
#include <GScolsys2.h>
#include <GSmath.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 gimmickBoxIsPushBoxFloor();
int gimmickBoxPop();
u8 peopleLineCheck(u32 group, u32 id, f32 x1, f32 z1, f32 x2, f32 z2, f32 distance, u8 resolve);
u8 peopleRectCheck(u32 group, u32 id, f32 x1, f32 z1, f32 x2, f32 z2);
#ifdef __cplusplus
}
#endif
#endif
