#include <hsd/class.h>
#include <hsd/object.h>

void ObjInfoInit(void);

_HSD_ClassInfo hsdObj = { { ObjInfoInit } };

void ObjInfoInit(void)
{
    hsdInitClassInfo(&hsdObj, &hsdClass, "sysdolphin_base_library", "hsd_obj",
                     sizeof(_HSD_ClassInfo), sizeof(_HSD_Obj));
}
