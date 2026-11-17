#include <types.h>
#include <new.h>
#include <HSDArchive.h>
#include <wazaSequence.h>
#include <app/wazaSequence/wazaSequenceSys.h>

/*
 * Reading and releasing the move data of a sequence. The data of a move is the
 * header of the sequence followed by the block that describes how the entries
 * of the sequence are laid out, the archive the models, cameras and particles
 * of the entries were packed in and the blocks of the entries themselves.
 * Every block of the file is read with the 32 byte alignment the archive
 * reader hands the data out with, so the cursor the callbacks walk is advanced
 * by the size of a block, aligned, and then by the archive it carries.
 */

// Kind of block a move resource is made of. The first block of the resource is
// the sequence itself, the blocks behind it are the entries it plays back.
enum enumSequenceEntryDataType {
    SEQUENCE_ENTRY_DATA_TYPE_MAIN,
    SEQUENCE_ENTRY_DATA_TYPE_CAMERA,
    SEQUENCE_ENTRY_DATA_TYPE_MODEL,
    SEQUENCE_ENTRY_DATA_TYPE_PARTICLE,
    SEQUENCE_ENTRY_DATA_TYPE_EFFECT,
    SEQUENCE_ENTRY_DATA_TYPE_SOUND,
    SEQUENCE_ENTRY_DATA_TYPE_EVENT,
    SEQUENCE_ENTRY_DATA_TYPE_LENS_FLARE
};

// Head of the block every entry of a move resource starts with. The entry
// itself reads the rest of its block; the sequence only dispatches the entries
// by the type they carry.
struct SequenceEntryData {
    u32 entryID;                  // 0x00: identifier copied into the runtime entry.
    enumSequenceEntryDataType type;   // 0x04: kind of entry the block holds.
};

// Block that follows the header of a move resource and describes the sequence
// the resource holds.
struct WazaSequenceData {
    u16 unk00;            // 0x00
    u16 unk02;            // 0x02
    u32 entryCount;       // 0x04: number of blocks of the resource, the
                          //       sequence itself counted as one.
    u32 unk08;            // 0x08
    u32 unk0C;            // 0x0c
    u32 unk10;            // 0x10
    s32 archiveSize;      // 0x14: size of the archive embedded behind the
                          //       block, zero when the resource carries none.
    u32 unk18;            // 0x18
    HSD_Archive *archive; // 0x1c: archive parsed out of the embedded data.
};

// Sequence entry a move resource is read into. The entry reads its own block
// and plays the sequence back, so only the interface the resource callbacks
// use is declared here: the sequence entry module implements it.
class SequenceEntry {
public:
    virtual ~SequenceEntry();

private:
    u8 unknown[44];
};

class MainSequenceEntry : public SequenceEntry {
public:
    MainSequenceEntry(WazaSequence *sequence, u8 **data);
    virtual ~MainSequenceEntry();
};

// The entries a sequence is made of.
class CameraEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class ModelEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class ParticleEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class EffectEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class SoundEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class EventEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class LensFlareEntry {
public:
    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

/*
 * Releases the move data a sequence was loaded from: the archive the entries
 * were parsed into is released first, then every entry walks its own block and
 * releases the resources it holds. The header of the resource is read through
 * a temporary sequence entry, which advances the cursor past the sequence block.
 */
void WazaSequence::Unload(u8 *data)
{
    MainSequenceEntry entry(NULL, &data);
    WazaSequenceData *sequence = (WazaSequenceData *)data;
    data = (u8 *)(sequence + 1);
    u16 count = sequence->entryCount - 1;
    data = (u8 *)(((u32)data + 31) & ~31);

    if (sequence->archive) {
        delete sequence->archive;
        sequence->archive = NULL;
    }
    if (sequence->archiveSize) {
        data += (sequence->archiveSize + 31) & ~31;
    }

    if (count != 0) {
        for (u16 i = 0; i < count; i++) {
            switch (((SequenceEntryData *)data)->type) {
            case SEQUENCE_ENTRY_DATA_TYPE_CAMERA:
                CameraEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_MODEL:
                ModelEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_PARTICLE:
                ParticleEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_EFFECT:
                EffectEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_SOUND:
                SoundEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_EVENT:
                EventEntry::Unload(&data);
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_LENS_FLARE:
                LensFlareEntry::Unload(&data);
                break;
            }
        }
    }
}

/*
 * Reads the move data of a sequence: the archive the entries are packed in is
 * parsed first, then every entry walks its own block and loads the resources
 * it plays back. The resource identity the sequence system hands out is taken
 * first, so the entries that load their resources are registered behind it.
 * The sequence reports whether all of its entries could be loaded. The data of
 * the resource is left as it is when an entry fails; the caller releases it
 * through Unload.
 */
bool WazaSequence::Load(u8 *data, u32, u32)
{
    MainSequenceEntry entry(NULL, &data);
    wazaSequenceSysGetResID();
    WazaSequenceData *sequence = (WazaSequenceData *)data;
    data = (u8 *)(sequence + 1);
    u16 count = sequence->entryCount - 1;
    data = (u8 *)(((u32)data + 31) & ~31);

    sequence->archive = NULL;
    if (sequence->archiveSize) {
        HSD_Archive *archive = new HSD_Archive;
        if (archive) {
            sequence->archive = archive;
            HSD_ArchiveParse(archive, data, sequence->archiveSize);
        }
        data += (sequence->archiveSize + 31) & ~31;
    }

    if (count != 0) {
        for (u16 i = 0; i < count; i++) {
            switch (((SequenceEntryData *)data)->type) {
            case SEQUENCE_ENTRY_DATA_TYPE_CAMERA:
                if (!CameraEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_MODEL:
                if (!ModelEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_PARTICLE:
                if (!ParticleEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_EFFECT:
                if (!EffectEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_SOUND:
                if (!SoundEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_EVENT:
                if (!EventEntry::Load(&data)) {
                    return false;
                }
                break;
            case SEQUENCE_ENTRY_DATA_TYPE_LENS_FLARE:
                if (!LensFlareEntry::Load(&data)) {
                    return false;
                }
                break;
            default:
                return false;
            }
        }
    }
    return true;
}
