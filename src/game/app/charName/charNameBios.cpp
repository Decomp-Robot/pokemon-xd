#include <types.h>
#include <charNameBios.h>
#include <GSlog.h>

// One record of the loaded character name table.
struct CharNameData {
    u16 hearFlag;
    u16 reserved;
    u32 nameID;
};

extern "C" {

// The loaded character name table: the number of records, and the records
// themselves.
extern u32* char_name_number;
extern CharNameData* char_name;

// Returns the record at index. An index outside the loaded count is reported
// instead of reading past the table.
static inline CharNameData* charNameBiosGetPtr(u16 index) {
    if (index >= *char_name_number) {
        GSlogWrite("[%s]: テーブルインデックスが範囲外です\n", __FUNCTION__);
        return 0;
    }
    return &char_name[index];
}

// The index of the record holding nameID, or 0 when no record holds it.
u16 charNameBiosSearchIndex(u32 nameID) {
    u16 index;

    for (index = 0; index < *char_name_number; index++) {
        if (nameID == char_name[index].nameID) {
            return index;
        }
    }
    return 0;
}

// The name ID of the indexed record, or 0 when the index is out of range.
u32 charNameBiosGetNameID(u16 index) {
    CharNameData* data = charNameBiosGetPtr(index);

    if (data == 0) {
        return 0;
    }
    return data->nameID;
}

// The heard flag of the indexed record, or 0 when the index is out of range.
u16 charNameBiosGetHearFlag(u16 index) {
    CharNameData* data = charNameBiosGetPtr(index);

    if (data == 0) {
        return 0;
    }
    return data->hearFlag;
}

}
