#ifndef MENU_PC_BOX_H
#define MENU_PC_BOX_H

#include <winSprite.h>

bool menuReliveHallTutorialGetIsCancelExit();

extern "C" {
void menuReliveHallTutorialExit();
void menuReliveHallTutorialMain();
void menuReliveHallTutorialInit();
void menuHologramExit();
void menuHologramMain();
void menuHologramInit();
void menuReliveHallExit();
void menuReliveHallMain();
void menuReliveHallInit();
void menuPcBoxExit();
void menuPcBoxMain();
void menuPcBoxInit();
long menuPcBoxSelectMenuItem(tagWINDOW_WORK *window);
long menuPcBoxCtrl(tagWINDOW_WORK *window);
long menuPcBoxItem3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxItem1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxItem0(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxBarRed3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxBarRed2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxBarRed1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPcBoxBarRed0(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
}

#endif
