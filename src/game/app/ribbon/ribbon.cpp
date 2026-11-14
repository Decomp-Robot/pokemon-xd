#include <ribbon.h>
#include <relglobal.h>

u32 getRibbonMsgID(s32 ribbon)
{
    if (*ribbon_data_number <= ribbon) {
        return 0;
    }
    return ribbon_data[ribbon].messageID;
}

s32 getRibbonCnt(void)
{
    return *ribbon_data_number;
}
