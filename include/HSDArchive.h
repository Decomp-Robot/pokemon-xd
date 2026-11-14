#ifndef HSD_ARCHIVE_H
#define HSD_ARCHIVE_H

#include <types.h>

enum { HSD_ARCHIVE_INITIALIZED = 1 };

typedef struct HSD_ArchiveHeader {
    u32 file_size;
    u32 data_size;
    u32 relocation_count;
    u32 public_symbol_count;
    u32 external_symbol_count;
    u32 reserved[3];
} HSD_ArchiveHeader;

typedef struct HSD_Archive {
    HSD_ArchiveHeader header;
    void *data;
    u32 *relocations;
    struct HSD_ArchivePublicSymbol *public_symbols;
    struct HSD_ArchiveExternalSymbol *external_symbols;
    char *strings;
    u32 reserved[2];
    u32 flags;
    void *file_base;
} HSD_Archive;

typedef struct HSD_ArchivePublicSymbol {
    u32 data_offset;
    u32 name_offset;
} HSD_ArchivePublicSymbol;

typedef struct HSD_ArchiveExternalSymbol {
    u32 data_offset;
    u32 name_offset;
} HSD_ArchiveExternalSymbol;

#ifdef __cplusplus
extern "C" {
#endif
s32 HSD_ArchiveParse(HSD_Archive *archive, void *data, u32 size);
void *HSD_ArchiveGetPublicAddress(HSD_Archive *archive, const char *name);
#ifdef __cplusplus
}
#endif

#endif
