#include <GSgfx.h>
#include <dolphin/gx/GXManage.h>

enum { GSGFX_TOKEN_REGISTER_MAX = 4, GSGFX_TOKEN_FRAME_SIZE = 4096, GSGFX_TOKEN_MAXIMUM = 57344 };

struct GSgfxTokenRegister {
    u16 mFirst;
    u16 mLast;
    void (*mCallback)(void);
};

GSgfxTokenRegister _tokenRegisterList[GSGFX_TOKEN_REGISTER_MAX];
u16 _tokenCurrent;
u16 _tokenFrameStart;
u16 _tokenFrameEnd;
u16 _tokenLastGPU;
u16 _tokenMaximum;
static u8 _tokenRegisterCount;
void _tokenDrawSyncCallback(u16 token);

u16 GSgfxTokenMark_Sub(void)
{
    u16 token;

    if (_tokenCurrent == _tokenFrameEnd) {
        return _tokenFrameEnd;
    }
    token = _tokenCurrent;
    GXSetDrawSync(token);
    _tokenCurrent++;
    if (_tokenCurrent >= _tokenMaximum) {
        _tokenCurrent = 1;
    }
    return token;
}

void GSgfxTokenMarkFrame(void)
{
    _tokenFrameStart = _tokenCurrent;
    _tokenFrameEnd = _tokenCurrent + GSGFX_TOKEN_FRAME_SIZE;
    if (_tokenFrameEnd >= _tokenMaximum) {
        _tokenFrameEnd -= _tokenMaximum;
    }
}

void GSgfxTokenSetDrawSyncCBFunc(void (*callback)(void (*)(u16)))
{
    if (callback != 0) {
        callback(_tokenDrawSyncCallback);
    }
}

void GSgfxTokenAbortFrame(void)
{
    _tokenLastGPU = _tokenFrameStart;
}

void GSgfxTokenInit(void)
{
    _tokenLastGPU = 0;
    _tokenCurrent = 1;
    _tokenMaximum = GSGFX_TOKEN_MAXIMUM;
    _tokenFrameStart = _tokenCurrent;
    _tokenFrameEnd = _tokenCurrent + GSGFX_TOKEN_FRAME_SIZE;
    _tokenRegisterList[0].mCallback = 0;
    _tokenRegisterList[1].mCallback = 0;
    _tokenRegisterList[2].mCallback = 0;
    _tokenRegisterList[3].mCallback = 0;
    _tokenRegisterCount = 0;
    GSgfxGPUSetDrawSyncCallback(_tokenDrawSyncCallback);
}

void _tokenDrawSyncCallback(u16 token)
{
    u8 count = _tokenRegisterCount;
    u8 i;

    for (i = 0; i < count; i++) {
        GSgfxTokenRegister *entry = &_tokenRegisterList[i];
        if (entry->mFirst <= token && entry->mLast >= token) {
            entry->mCallback();
            return;
        }
    }
    _tokenLastGPU = token;
}
