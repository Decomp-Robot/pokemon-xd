#include <relglobal.h>

extern "C" u16 mballBiosGetSpriteID(u16 ballDataId)
{
    if (ballDataId < *mball_data_number) {
        return mball_data[ballDataId];
    }
    return 0;
}
