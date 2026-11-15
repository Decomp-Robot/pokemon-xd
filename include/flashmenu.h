#ifndef FLASH_MENU_H
#define FLASH_MENU_H

#include <types.h>

struct GStexture;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

class FlashMenu {
public:
    FlashMenu();
    ~FlashMenu();
    s32 Init(void* resource, u32 resourceID);
    void Release();
    void Update();
    void Draw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    GStexture* GetGStexture(s32 textureID);
    u16 GetFrameFromLabel(char* label);
    u32 GetPlayFrame();
    void SetPlayFrame(u32 frame);

    u8 unknownPrefix[36];
    u8 loopFlag;
    u8 pauseFlag;
    u8 unknownFlags[2];
    u32 startFrame;
    u32 endFrame;
    u8 unknownTail[272];
};

#endif
