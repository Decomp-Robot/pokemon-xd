#include <types.h>

class CMetaMenu {
public:
    void localWindowEntry(long value, long index, void (*callback)(CMetaMenu*, long));
    long localWindowSearch(long value);

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

long CMetaMenu::localWindowSearch(long value) {
    for (long i = 0; i < 64; i++) {
        if (mLocalWindowIds[i] == value) {
            return i;
        }
    }
    return -1;
}
