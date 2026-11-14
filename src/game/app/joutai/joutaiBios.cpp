#include "joutaiBios.h"

u8 joutaiDataBiosGetKaisuuDataId(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->kaisuuDataId;
}

u8 joutaiDataBiosGetMaxCount(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->maxCount;
}

u16 joutaiDataBiosGetSameJoutai(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->sameJoutai;
}

u8 joutaiDataBiosGetWriteMode(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->writeMode;
}

u8 joutaiDataBiosGetUseBanme(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->useBanme;
}

u16 joutaiDataBiosGetStatus(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->status;
}

u8 joutaiDataBiosGetStatusKind(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->statusKind;
}

u32 joutaiDataBiosGetName(u16 dataId)
{
    JoutaiData* data = joutaiDataBiosGetDataPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->name;
}

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
