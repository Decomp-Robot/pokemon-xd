#ifndef SYNTHDATA_H
#define SYNTHDATA_H

#include "musyx/hardware.h"
#include "musyx/macros.h"

#ifdef __cplusplus
extern "C" {
#endif
// TODO: 32bit assumptions

typedef struct SAMPLE_INFO {
  u32 info;
  void* addr;
  void* extraData;
  u32 offset;
  u32 length;
  u32 loop;
  u32 loopLength;
  u8 compType;
} SAMPLE_INFO;

typedef struct GROUP_DATA {
  u32 nextOff;
  u16 id;
  u16 type;
  u32 macroOff;
  u32 sampleOff;
  u32 curveOff;
  u32 keymapOff;
  u32 layerOff;
  union {
    struct fx {
      u32 tableOff;
    } fx;
    struct song {
      u32 normpageOff;
      u32 drumpageOff;
      u32 midiSetupOff;
    } song;
  } data;
} GROUP_DATA;

typedef struct SAMPLE_HEADER {
  u32 info;
  u32 length;
  u32 loopOffset;
  u32 loopLength;
} SAMPLE_HEADER;

typedef struct SDIR_DATA {
  u16 id;
  u16 ref_cnt;
  u32 offset;
  void* addr;
  SAMPLE_HEADER header;
  u32 extraData;
} SDIR_DATA;

/*! A direct copy of the above structure to allow us to load in legacy data
 * this must be done via sndConvert32BitSDIRto64BitSDIR via client code,
 * this is explicit on the part of the programmer.
 * MusyX data built with the new tools will not require this step.
 */
typedef struct SDIR_DATA_INTER {
  // total size: 0x20
  u16 id;               // offset 0x0, size 0x2
  u16 ref_cnt;          // offset 0x2, size 0x2
  u32 offset;           // offset 0x4, size 0x4
  u32 addr;             // offset 0x8, size 0x4
  SAMPLE_HEADER header; // offset 0xC, size 0x10
  u32 extraData;        // offset 0x1C, size 0x4
} SDIR_DATA_INTER;

typedef struct SDIR_TAB {
  SDIR_DATA* data;
  void* base;
  u16 numSmp;
  u16 res;
} SDIR_TAB;

typedef struct DATA_TAB {
  void* data;
  u16 id;
  u16 refCount;
} DATA_TAB;

typedef struct LAYER_TAB {
  void* data;
  u16 id;
  u16 num;
  u16 refCount;
  u16 reserved;
} LAYER_TAB;

typedef struct MAC_MAINTAB {
  u16 num;
  u16 subTabIndex;
} MAC_MAINTAB;

typedef struct MAC_SUBTAB {
  void* data;
  u16 id;
  u16 refCount;
} MAC_SUBTAB;

typedef struct GSTACK {
  GROUP_DATA* gAddr;
  SDIR_DATA* sdirAddr;
  void* prjAddr;
} GSTACK;

#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
typedef struct GSTACK_INST {
  struct GSTACK_INST* next;
  unsigned long id;
  signed short sp;
  struct GSTACK gs[128];
  struct ARAMInfo aramInfo;
} GSTACK_INST;
#endif

typedef struct LAYER {
  u16 id;
  u8 keyLow;
  u8 keyHigh;
  s8 transpose;
  u8 volume;
  s16 prioOffset;
  u8 panning;
  u8 reserved[3];
} LAYER;

typedef struct KEYMAP {
  u16 id;
  s8 transpose;
  u8 panning;
  s16 prioOffset;
  u8 reserved[2];
} KEYMAP;

typedef struct MEM_DATA {
  u32 nextOff;
  u16 id;
  u16 reserved;
  union {
    struct {
      u32 num;
      LAYER entry[1];
    } layer;
    KEYMAP map[128];
    u8 tab[1];
    MSTEP cmd[1][2];
  } data;
} MEM_DATA;

typedef struct POOL_DATA {
  u32 macroOff;
  u32 curveOff;
  u32 keymapOff;
  u32 layerOff;
} POOL_DATA;

typedef struct FX_TAB {
  u16 id;
  u16 macro;
  u8 maxVoices;
  u8 priority;
  u8 volume;
  u8 panning;
  u8 key;
  u8 vGroup;
} FX_TAB;

typedef struct FX_DATA {
  u16 num;
  u16 reserverd;
  FX_TAB fx[1];
} FX_DATA;

typedef struct FX_GROUP {
  u16 gid;
  u16 fxNum;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
  u16 refCount;
  u16 reserved;
#endif
  FX_TAB* fxTab;
} FX_GROUP;

void dataInit(u32, u32);
void dataExit();
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
void dataInitStack(unsigned long aramBase, unsigned long aramSize);
ARAMInfo* dataARAMGetInfo();
ARAMInfo* dataARAMDefaultGetInfo();
#else
void dataInitStack();
#endif
bool dataInsertSDir(SDIR_DATA* sdir, void* smp_data);
bool dataRemoveSDir(SDIR_DATA* sdir);
bool dataInsertMacro(u16 mid, void* macroaddr);
bool dataRemoveMacro(u16 mid);
bool dataInsertCurve(u16 cid, void* curvedata);
bool dataRemoveCurve(u16 sid);
s32 dataGetSample(u16 sid, SAMPLE_INFO* newsmp);
void* dataGetCurve(u16 cid);
bool dataAddSampleReference(u16 sid
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
                            ,
                            ARAMInfo* aramInfo
#endif
);
bool dataRemoveSampleReference(u16 sid
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
                               ,
                               ARAMInfo* aramInfo
#endif
);
bool dataInsertKeymap(u16 cid, void* keymapdata);
bool dataRemoveKeymap(u16 sid);
bool dataInsertLayer(u16 cid, void* layerdata, u16 size);
bool dataRemoveLayer(u16 sid);
bool dataInsertFX(u16 gid, FX_TAB* fx, u16 fxNum);
bool dataRemoveFX(u16 gid);
FX_TAB* dataGetFX(u16 fid);
void* dataGetLayer(u16 cid, u16* n);
void* dataGetKeymap(u16 cid);
MSTEP* dataGetMacro(u16 mid);

#ifdef __cplusplus
}
#endif

#endif // SYNTHDATA_H
