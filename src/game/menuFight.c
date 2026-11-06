#include <types.h>
#include <menu.h>

s32 menuFightCloseTarget(bool wait)
{
    if (menuIsCheck(MENU_ID_FIGHT_TARGET_SELECT))
        menuCloseCustom(MENU_ID_FIGHT_TARGET_SELECT, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_TARGET_SUB))
        menuCloseCustom(MENU_ID_FIGHT_TARGET_SUB, 0, wait);

    return 0;
}
