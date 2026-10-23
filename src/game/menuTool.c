#include <types.h>

extern s32 menuToolSelect;

void menuToolSetSelect(s32 select)
{
    menuToolSelect = select;
}

void menuToolInit(void)
{
    menuToolSelect = 0;
}
