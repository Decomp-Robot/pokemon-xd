#ifndef HSD_ARCHIVE_H
#define HSD_ARCHIVE_H
#include <types.h>

struct HSD_Archive {
    u32 mArchiveState[0x44 / sizeof(u32)];
};

extern "C" {
s32 HSD_ArchiveParse(HSD_Archive *archive, void *data, u32 size);
void *HSD_ArchiveGetPublicAddress(HSD_Archive *archive, const char *name);
}
#endif
