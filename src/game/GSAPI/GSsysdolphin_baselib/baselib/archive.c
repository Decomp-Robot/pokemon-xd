#include <HSDArchive.h>
#include <string.h>
#include <dolphin/os.h>

static const char sByteOrderMismatch[] =
    "HSD_ArchiveParse: byte-order mismatch! Please check data format\n";

static inline void HSD_ArchiveRelocate(HSD_Archive *archive)
{
    u32 index;

    for (index = 0; index < archive->header.relocation_count; index++) {
        *(u32 *) ((u8 *) archive->data + archive->relocations[index]) +=
            (u32) archive->data;
    }
}

void *HSD_ArchiveGetPublicAddress(HSD_Archive *archive, const char *name)
{
    u32 index;

    for (index = 0; index < archive->header.public_symbol_count; index++) {
        if (strcmp(archive->strings + archive->public_symbols[index].name_offset,
                   name) == 0)
        {
            return (u8 *) archive->data +
                   archive->public_symbols[index].data_offset;
        }
    }
    return NULL;
}

s32 HSD_ArchiveParse(HSD_Archive *archive, void *data, u32 size)
{
    u32 offset;
    u8 *file = data;

    if (archive == NULL) {
        return -1;
    }

    memset(archive, 0, sizeof(HSD_Archive));
    archive->flags |= HSD_ARCHIVE_INITIALIZED;
    memcpy(&archive->header, file, sizeof(HSD_ArchiveHeader));
    if (archive->header.file_size != size) {
        OSReport(sByteOrderMismatch);
        return -1;
    }

    offset = sizeof(HSD_ArchiveHeader);
    if (archive->header.data_size != 0) {
        archive->data = file + offset;
        offset += archive->header.data_size;
    }
    if (archive->header.relocation_count != 0) {
        archive->relocations = (u32 *) (file + offset);
        offset += archive->header.relocation_count * sizeof(u32);
    }
    if (archive->header.public_symbol_count != 0) {
        archive->public_symbols =
            (HSD_ArchivePublicSymbol *) (file + offset);
        offset += archive->header.public_symbol_count * sizeof(HSD_ArchivePublicSymbol);
    }
    if (archive->header.external_symbol_count != 0) {
        archive->external_symbols =
            (HSD_ArchiveExternalSymbol *) (file + offset);
        offset += archive->header.external_symbol_count * sizeof(HSD_ArchiveExternalSymbol);
    }
    if (offset < archive->header.file_size) {
        archive->strings = (char *) file + offset;
    }
    archive->file_base = file;

    HSD_ArchiveRelocate(archive);
    return 0;
}
