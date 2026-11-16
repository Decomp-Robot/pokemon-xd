#include <FightResult.h>
#include <fightAction.h>

// Battle ending: the closing message is chosen by the result of the battle.
FIGHT_ACTION_DATA_dd_ fight_action_data_syuuryou[] = {
    {FIGHT_ACTION_KIND_SYUURYOU_PRE, 0},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultHostVictory},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultOtherSideVictory},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultLoseWin},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultWinLose},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultDraw},
    {FIGHT_ACTION_KIND_SYUURYOU, kFightResultOtherDraw},
    {FIGHT_ACTION_KIND_SYUURYOU_POST, 0},
    {FIGHT_ACTION_KIND_HEIJOU, 0},
    {0, 0},
};
