#include <types.h>
#include <GSlog.h>

extern "C" {

// The loaded table resources: the number of registered blocks, and the table
// of pointers to them.
extern u32* table_res_number;
extern void** table_res;

// Returns the resource block registered under index. An index outside the
// registered count is logged and yields null instead of a read past the table.
void* tableResBiosGetResPtr(u32 index) {
    if (index >= *table_res_number) {
        GSlogWrite("[%s]: テーブルインデックスが範囲外です\n", __FUNCTION__);
        return 0;
    }
    return table_res[index];
}

}
