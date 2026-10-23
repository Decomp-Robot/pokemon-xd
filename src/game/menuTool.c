#include <menuTool.h>

#pragma force_active on

s32 _menuToolSelect;

void menuToolSetSelect(s32 select)
{
    _menuToolSelect = select;
}

void menuToolInit(void)
{
    _menuToolSelect = 0;
}

#pragma force_active reset
