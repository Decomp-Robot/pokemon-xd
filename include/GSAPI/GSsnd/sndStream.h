#ifndef GSAPI_SNDSTREAM_H
#define GSAPI_SNDSTREAM_H

#include <types.h>
#include <GSsnd.h>

#ifdef __cplusplus
extern "C" {
#endif

void _sndStreamPrepareGetData(GSsndStream* stream);
void _sndStreamPrepareGetData_Exec(GSsndStream* stream, GSsndStream* target, u8 halfIndex);
void _sndStreamPrepareGetData_GetOldData(GSsndStream* stream, GSsndStream* target, u8 halfIndex);
void _sndStreamPrepareGetData_GetNewData(GSsndStream* stream, GSsndStream* target, u8 halfIndex);
void _sndStreamPrepareGetDataCallback(long fileMode, void* context, long argument);
void _sndStreamPrepareGetDataCallback_Exec(GSsndStream* stream, GSsndStream* target,
                                           GSsndStreamBuffer* buffer, u32 headerPos, u8 halfIndex);
u32 _sndStreamNibble_GetHeaderPos(u32 nibbles);
u32 _sndStreamNibble_HeaderOFF2ON(u32 nibbles);
u32 _sndStreamNibble_HeaderON2OFF(u32 nibbles);
u32 _sndStreamGetData(GSsndStream* stream, GSsndStreamDataCallback callback);
void _sndStreamGetDataCallback_START(u32 fileNo, u32 arg, GSsndStream* stream);
void _sndStreamGetDataCallback_ACTIVATE(u32 fileNo, u32 arg, GSsndStream* stream);
void _sndStreamGetDataCallback_EXEC_UPDATE(u32 fileNo, u32 arg, GSsndStream* stream);
u32 _sndStreamCallback(void* buffer1, u32 len1, void* buffer2, u32 len2, u32 user);
void _sndStreamCtrl(GSsndStream* stream, u8 mode);

#ifdef __cplusplus
}
#endif

#endif
