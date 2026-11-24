#include <hsd/jobj.h>

u32 HSD_JObjGetFlags(_HSD_JObj *jobj)
{
    return jobj != NULL ? jobj->flags : 0;
}
