#include <fightAction.h>

// One turn of the battle: every trainer selects an action, the selected
// actions play out, and the one-turn counters are cleared afterwards.
FIGHT_ACTION_DATA_dd_ fight_action_data_turn[] = {
    {FIGHT_ACTION_KIND_ALL_FIGHT_TRAINER_SELECT_FIGHT_ACTION, 0},
    {FIGHT_ACTION_KIND_FIGHT_NIGERU, 0},
    {FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_IREKAE, 0},
    {FIGHT_ACTION_KIND_FIGHT_TRAINER_CALL, 0},
    {FIGHT_ACTION_KIND_ALL_FIGHT_OUT_POKEMON_DO_FIGHT_ACTION, 0},
    {FIGHT_ACTION_KIND_FOCUS_PUNCH_PREPARE, 0},
    {FIGHT_ACTION_KIND_ONE_TURN_POST, 0},
    {0, 0},
};
