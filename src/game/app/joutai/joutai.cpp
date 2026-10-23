#include <status.h>
#include "joutai.h"

void _JOUTAI::writeJoutaiDataId(u16 dataId, u16 targetId)
{
    if (dataId != 0) {
        u8 writeMode = joutaiDataBiosGetWriteMode(dataId);
        u8 previousCount;
        if (writeMode == JOUTAI_WRITE_ACCUMULATE) {
            previousCount = joutaiBiosGetCount(this);
        } else {
            previousCount = 0;
        }
        init();
        joutaiDataId = dataId;
        userFightTargetId = targetId;
        kaisuu = kaisuuGetKaisuu(joutaiDataBiosGetKaisuuDataId(dataId));
        if (writeMode == JOUTAI_WRITE_ACCUMULATE) {
            previousCount++;
            u8 maxCount = joutaiDataBiosGetMaxCount(dataId);
            if (maxCount < previousCount) {
                previousCount = maxCount;
            }
            count = previousCount;
        }
    }
}

s32 _JOUTAI::checkWriteJoutaiDataId(u16 dataId)
{
    if (dataId == 0) {
        return JOUTAI_WRITE_KEEP;
    }
    u16 currentId = joutaiDataId;
    switch (joutaiDataBiosGetWriteMode(dataId)) {
    case JOUTAI_WRITE_REPLACE:
        return JOUTAI_WRITE_ACCEPT;
    case JOUTAI_WRITE_OVERWRITE:
        return JOUTAI_WRITE_ACCEPT;
    case JOUTAI_WRITE_IF_EMPTY:
        if (isValid() == 0) {
            return JOUTAI_WRITE_ACCEPT;
        }
        return JOUTAI_WRITE_KEEP;
    case JOUTAI_WRITE_IF_DIFFERENT:
        if (!isValid()) {
            return JOUTAI_WRITE_ACCEPT;
        }
        if (currentId != dataId && dataId != joutaiDataBiosGetSameJoutai(currentId)) {
            return JOUTAI_WRITE_ACCEPT;
        }
        return JOUTAI_WRITE_KEEP;
    case JOUTAI_WRITE_ACCUMULATE:
        if (!isValid()) {
            return JOUTAI_WRITE_ACCEPT;
        }
        if (currentId == dataId || dataId == joutaiDataBiosGetSameJoutai(currentId)) {
            return JOUTAI_WRITE_ACCEPT;
        }
        return JOUTAI_WRITE_KEEP;
    }
    return JOUTAI_WRITE_INVALID;
}

u8 _JOUTAI::isValid()
{
    return joutaiBiosGetJoutaiDataId(this) != 0;
}

void _JOUTAI::init()
{
    joutaiDataId = 0;
    userFightTargetId = 0;
    kaisuu = 0;
    nowKaisuu = 0;
    count = 0;
    buff = 0;
    idBuff = 0;
}

void joutaiCopy(void* destination, void* source, u16 dataId)
{
    if (dataId == 0) {
        return;
    }
    _JOUTAI* destinationJoutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(destination, dataId);
    if (destinationJoutai == 0) {
        return;
    }
    if (!joutaiIsJoutaiDataId(source, dataId)) {
        return;
    }
    _JOUTAI* sourceJoutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(source, dataId);
    if (sourceJoutai == 0) {
        return;
    }
    *destinationJoutai = *sourceJoutai;
}

void joutaiSetBuff(void* status, u16 dataId, u32 buff)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutaiBiosSetBuff(joutai, buff);
        }
    }
}

u32 joutaiGetBuff(void* status, u16 dataId)
{
    if (dataId == 0) {
        return 0;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return 0;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return 0;
    }
    return joutaiBiosGetBuff(joutai);
}

void joutaiSetIdBuff(void* status, u16 dataId, u16 idBuff)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutaiBiosSetIdBuff(joutai, idBuff);
        }
    }
}

u16 joutaiGetIdBuff(void* status, u16 dataId)
{
    if (dataId == 0) {
        return 0;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return 0;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return 0;
    }
    return joutaiBiosGetIdBuff(joutai);
}

u16 joutaiGetUserFightTargetId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return 0;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return 0;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return 0;
    }
    return joutaiBiosGetUserFightTargetId(joutai);
}

void joutaiSetKaisuuJoutaiDataId(void* status, u16 dataId, u8 kaisuu)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutai->kaisuu = kaisuu;
        }
    }
}

void joutaiSetNowKaisuuJoutaiDataId(void* status, u16 dataId, u8 nowKaisuu)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutaiBiosSetNowKaisuu(joutai, nowKaisuu);
        }
    }
}

s32 joutaiGetNowKaisuuJoutaiDataId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return -1;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return -1;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return -1;
    }
    return joutaiBiosGetNowKaisuu(joutai);
}

s32 joutaiGetKaisuuJoutaiDataId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return -1;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return -1;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return -1;
    }
    return joutaiBiosGetKaisuu(joutai);
}

void joutaiSetCountAsJoutaiDataId(void* status, u16 dataId, u8 count)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutai->count = count;
        }
    }
}

s32 joutaiGetCountAsJoutaiDataId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return -1;
    }
    if (!joutaiIsJoutaiDataId(status, dataId)) {
        return -1;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return -1;
    }
    return joutaiBiosGetCount(joutai);
}

void joutaiWriteJoutaiDataId(void* status, u16 dataId, u16 targetId)
{
    if (dataId != 0) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            joutai->writeJoutaiDataId(dataId, targetId);
        }
    }
}

s32 joutaiCheckWriteJoutaiDataId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return JOUTAI_WRITE_KEEP;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return JOUTAI_WRITE_INVALID;
    }
    return joutai->checkWriteJoutaiDataId(dataId);
}

u8 joutaiIsJoutaiDataId(void* status, u16 dataId)
{
    if (dataId == 0) {
        return 0;
    }
    _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
    if (joutai == 0) {
        return 0;
    }
    if (joutai->isValid() == 1 && dataId == joutaiBiosGetJoutaiDataId(joutai)) {
        return 1;
    }
    return 0;
}

void joutaiInitJoutaiDataId(void* status, u16 dataId)
{
    if (dataId != 0 && joutaiIsJoutaiDataId(status, dataId)) {
        _JOUTAI* joutai = _joutaiGetTargetJoutaiPtrAsJoutaiDataId(status, dataId);
        if (joutai != 0) {
            _joutaiInit(joutai);
        }
    }
}

void joutaiInitAry(_JOUTAI* joutai, u16 length)
{
    if (joutai != 0) {
        for (u16 index = 0; index < length; index++) {
            _joutaiInit(&joutai[index]);
        }
    }
}

void _joutaiInit(_JOUTAI* joutai)
{
    if (joutai != 0) {
        joutai->init();
    }
}

_JOUTAI* _joutaiGetTargetJoutaiPtrAsJoutaiDataId(void* status, u16 dataId)
{
    u16 useBanme = joutaiDataBiosGetUseBanme(dataId);
    u8 statusKind = joutaiDataBiosGetStatusKind(dataId);
    u16 statusFieldId = joutaiDataBiosGetStatus(dataId);
    _JOUTAI* joutai = (_JOUTAI*)statusGetStatus(statusKind, status, 0, statusFieldId, 0);
    if (joutai == 0) {
        return 0;
    }
    return &joutai[useBanme];
}
