#include <dolphin/ai.h>
#include <dolphin/ar.h>
#include <dolphin/os.h>
#include <string.h>

struct _GSgfxInitParms {
    u8 displayEnabled;
    u8 pad_01[3];
    u8 field_04;
    u8 pad_05;
    u16 field_06;
    s32 region;
    u32 field_0C;
    u32 field_10;
    u32 field_14;
};

struct StartupSettings {
    s32 debugSysvars;
    s32 consoleMemoryMode;
    s32 perfbar;
    s32 errorTrap;
    s32 displayMemorySize;
    s32 battleDebugMenuFlag;
    s32 battleDebugDeckGroup;
    s32 battleDebugEncounterId;
    s32 battleDebugStoryEncounterId;
    s32 battleDebugHundredEncounterId;
    s32 battleDebugImmediateEncounterId;
    s32 battleDebugVirtualEncounterId;
    s32 battleDebugBingoEncounterId;
    s32 battleDebugColosseumEncounterId;
    s32 battleDebugItemSet1;
    s32 battleDebugAutoEncounter;
    s32 fieldDebugPokedoru;
    s32 fieldDebugPokecoupon;
    char localIp[0x10];
    char gamecubeIp[0x10];
    s32 serverPort;
    s32 outputPort;
    char netmask[0x10];
    char gateway[0x10];
    s32 heroCollisionDisableButton;
};


extern "C" {
void pokecoloInitRomVersion(void);
void sysvarsReadSettings(int *, char ***);
void *GSmemInit(void *, void *, s32);
u32 GSfsysCreateLocalHeap(void);
u32 GSfsysCreateCacheHeap(void);
u32 floorCreateLocalHeap(void);
void *GSmemCreateHeap(void *, void *);
u32 GSmemSetCurrentHeap(void *);
void GSmemInitSysdolAllocator(void);
s32 pokecoloGetCountryCode(void);
void sysvarsShowDebug(void);
void GSlogInit(u32, u32);
void GSscratchInit(u32);
void GSdvdSetDVDServer(void *, u32);
void GSdvdSetReportServer(void *, u32, u32);
void GSdvdSetGCClient(void *, void *, void *, u32);
s32 gameRuntimeMain(s32, char **);
extern StartupSettings startupSettings;
}

/* Stack the AI DMA callbacks run on. */
u8 aiStackMem[0x4000];

/* ARAM allocation stack indices ARInit manages. */
u32 aramMemArray[2];
void GSgfxInit(_GSgfxInitParms *);

int main(int argc, char **argv)
{
    pokecoloInitRomVersion();
    memset((void *)0x80001800, 0, 0x1800);
    sysvarsReadSettings(&argc, &argv);

    if (OSGetConsoleSimulatedMemSize() > 0x01800000) {
        if (startupSettings.consoleMemoryMode == 0x18) {
            OSSetArenaHi((void *)0x817ED780);
        } else if (startupSettings.consoleMemoryMode == 0x30) {
            OSSetArenaHi((void *)0x82DED720);
        }
    }

    void *arenaLo = OSGetArenaLo();
    OSGetArenaHi();
    OSSetArenaLo(GSmemInit(arenaLo, OSGetArenaHi(), 4));

    GSfsysCreateLocalHeap();
    GSfsysCreateCacheHeap();
    floorCreateLocalHeap();

    arenaLo = OSGetArenaLo();
    void *arenaHi = OSGetArenaHi();
    extern void *g_hSysdolphineHeap;
    extern void *g_hFloorHeap;
    GSmemSetCurrentHeap(
        g_hSysdolphineHeap = GSmemCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaHi);
    g_hFloorHeap = g_hSysdolphineHeap;
    GSmemInitSysdolAllocator();

    _GSgfxInitParms init;
    init.displayEnabled = startupSettings.errorTrap != 0;
    init.field_0C = 0x50000;
    init.field_10 = 0x50000;
    init.field_14 = 16;
    switch (pokecoloGetCountryCode()) {
    case 0:
    case 1:
        init.region = 0;
        break;
    case 2:
        init.region = 1;
        break;
    }
    init.field_04 = 2;
    init.field_06 = 0x1E0;
    GSgfxInit(&init);

    if (startupSettings.debugSysvars == 1) {
        sysvarsShowDebug();
    }
    ARInit(aramMemArray, 2);
    ARQInit();
    AIInit(aiStackMem + sizeof(aiStackMem) - 8);
    ARQSetChunkSize(0x8000);
    GSlogInit(0x8000, 2);
    GSscratchInit(0);

    GSdvdSetDVDServer(startupSettings.localIp,
                      (u16)startupSettings.serverPort);
    GSdvdSetReportServer(startupSettings.localIp,
                         (u16)startupSettings.outputPort, 1);
    GSdvdSetGCClient(startupSettings.gamecubeIp,
                     startupSettings.netmask,
                     startupSettings.gateway, 1);
    return gameRuntimeMain(argc, argv);
}
