#include <FightRuleData.h>
#include <types.h>
#include <saveData.h>

/*
 * menuCBBiosRuleMsg.cpp
 *
 * Message-ID bookkeeping for the six colosseum rule slots of the save table
 * (getTrainerDataStartOffset(0, 0xE)).
 */


/*
 * Stamp or clear the message IDs of the six save-table rule slots: set marks
 * all six slots with their display messages (the three custom slots get
 * their own), clear empties them again.
 */
extern "C" void menuCBBios_SetRuleMessages(u8 set)
{
    FightRuleData *ruleData =
        (FightRuleData *)getTrainerDataStartOffset(0, TRAINER_DATA_FIGHT_RULES);

    if (set == 1) {
        ruleData[0].mMsgId = 0x9124;
        ruleData[1].mMsgId = 0xA2D4;
        ruleData[2].mMsgId = 0xB51C;
        ruleData[3].mMsgId = 0x47FD;
        ruleData[4].mMsgId = 0x7422;
        ruleData[5].mMsgId = 0x5633;
    } else {
        ruleData[5].mMsgId = 0;
        ruleData[4].mMsgId = 0;
        ruleData[3].mMsgId = 0;
        ruleData[2].mMsgId = 0;
        ruleData[1].mMsgId = 0;
        ruleData[0].mMsgId = 0;
    }
}
