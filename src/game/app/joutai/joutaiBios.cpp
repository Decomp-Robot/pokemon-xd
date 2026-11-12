#include "joutaiBios.h"

JoutaiData* joutaiDataBiosGetDataPtr(u16 dataId)
{
    if (dataId >= joutai_data_number) {
        return 0;
    }
    return &joutai_data[dataId];
}

void joutaiBiosSetIdBuff(_JOUTAI* joutai, u16 idBuff)
{
    if (joutai == 0) {
        return;
    }
    joutai->idBuff = idBuff;
}

void joutaiBiosSetBuff(_JOUTAI* joutai, u32 buff)
{
    if (joutai == 0) {
        return;
    }
    joutai->buff = buff;
}

void joutaiBiosSetNowKaisuu(_JOUTAI* joutai, u8 nowKaisuu)
{
    if (joutai == 0) {
        return;
    }
    joutai->nowKaisuu = nowKaisuu;
}

u16 joutaiBiosGetIdBuff(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->idBuff;
}

u32 joutaiBiosGetBuff(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->buff;
}

u8 joutaiBiosGetCount(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->count;
}

s32 joutaiBiosGetNowKaisuu(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->nowKaisuu;
}

s32 joutaiBiosGetKaisuu(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->kaisuu;
}

u16 joutaiBiosGetUserFightTargetId(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->userFightTargetId;
}

u16 joutaiBiosGetJoutaiDataId(_JOUTAI* joutai)
{
    if (joutai == 0) {
        return 0;
    }
    return joutai->joutaiDataId;
}
