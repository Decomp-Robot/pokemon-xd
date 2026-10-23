#ifndef JOUTAI_H
#define JOUTAI_H

#include "types.h"

enum JoutaiWriteMode {
    JOUTAI_WRITE_REPLACE = 0,
    JOUTAI_WRITE_OVERWRITE = 1,
    JOUTAI_WRITE_IF_EMPTY = 2,
    JOUTAI_WRITE_IF_DIFFERENT = 3,
    JOUTAI_WRITE_ACCUMULATE = 4
};

enum JoutaiWriteResult {
    JOUTAI_WRITE_INVALID = 0,
    JOUTAI_WRITE_KEEP = 1,
    JOUTAI_WRITE_ACCEPT = 2
};

typedef struct _JOUTAI _JOUTAI;

struct _JOUTAI {
    u16 joutaiDataId;
    u16 userFightTargetId;
    u8 kaisuu;
    u8 nowKaisuu;
    u8 count;
    u8 reserved;
    u32 buff;
    u16 idBuff;

#ifdef __cplusplus
    void writeJoutaiDataId(u16 dataId, u16 targetId);
    s32 checkWriteJoutaiDataId(u16 dataId);
    u8 isValid();
    void init();
#endif
};

_JOUTAI* _joutaiGetTargetJoutaiPtrAsJoutaiDataId(void* status, u16 dataId);
void _joutaiInit(_JOUTAI* joutai);

#ifdef __cplusplus
extern "C" {
#endif
void joutaiCopy(void* destination, void* source, u16 dataId);
void joutaiSetBuff(void* status, u16 dataId, u32 buff);
u32 joutaiGetBuff(void* status, u16 dataId);
void joutaiSetIdBuff(void* status, u16 dataId, u16 idBuff);
u16 joutaiGetIdBuff(void* status, u16 dataId);
u16 joutaiGetUserFightTargetId(void* status, u16 dataId);
void joutaiSetKaisuuJoutaiDataId(void* status, u16 dataId, u8 kaisuu);
void joutaiSetNowKaisuuJoutaiDataId(void* status, u16 dataId, u8 nowKaisuu);
s32 joutaiGetNowKaisuuJoutaiDataId(void* status, u16 dataId);
s32 joutaiGetKaisuuJoutaiDataId(void* status, u16 dataId);
void joutaiSetCountAsJoutaiDataId(void* status, u16 dataId, u8 count);
s32 joutaiGetCountAsJoutaiDataId(void* status, u16 dataId);
void joutaiWriteJoutaiDataId(void* status, u16 dataId, u16 targetId);
s32 joutaiCheckWriteJoutaiDataId(void* status, u16 dataId);
u8 joutaiIsJoutaiDataId(void* status, u16 dataId);
void joutaiInitJoutaiDataId(void* status, u16 dataId);
void joutaiInitAry(_JOUTAI* joutai, u16 length);

u8 joutaiDataBiosGetKaisuuDataId(u16 dataId);
u8 joutaiDataBiosGetMaxCount(u16 dataId);
u16 joutaiDataBiosGetSameJoutai(u16 dataId);
u8 joutaiDataBiosGetWriteMode(u16 dataId);
u8 joutaiDataBiosGetUseBanme(u16 dataId);
u16 joutaiDataBiosGetStatus(u16 dataId);
u8 joutaiDataBiosGetStatusKind(u16 dataId);
u32 joutaiDataBiosGetName(u16 dataId);

void joutaiBiosSetIdBuff(_JOUTAI* joutai, u16 idBuff);
void joutaiBiosSetBuff(_JOUTAI* joutai, u32 buff);
void joutaiBiosSetNowKaisuu(_JOUTAI* joutai, u8 nowKaisuu);
u16 joutaiBiosGetIdBuff(_JOUTAI* joutai);
u32 joutaiBiosGetBuff(_JOUTAI* joutai);
u8 joutaiBiosGetCount(_JOUTAI* joutai);
s32 joutaiBiosGetNowKaisuu(_JOUTAI* joutai);
s32 joutaiBiosGetKaisuu(_JOUTAI* joutai);
u16 joutaiBiosGetUserFightTargetId(_JOUTAI* joutai);
u16 joutaiBiosGetJoutaiDataId(_JOUTAI* joutai);

s32 kaisuuGetKaisuu(s32 dataId);
#ifdef __cplusplus
}
#endif

#endif
