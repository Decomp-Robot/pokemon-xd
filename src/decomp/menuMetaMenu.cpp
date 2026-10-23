#include <types.h>

class CMetaMenu {
public:
    void localWindowEntry(long value, long index, void (*callback)(CMetaMenu*, long));

private:
    u8 _00[0x1c];
    long mLocalWindowIds[64];
    void (*mLocalWindowCallbacks[64])(CMetaMenu*, long);
    long* mLocalWindowTable;
};

void CMetaMenu::localWindowEntry(long value, long index, void (*callback)(CMetaMenu*, long)) {
    mLocalWindowIds[index] = value;
    mLocalWindowCallbacks[index] = callback;
}
