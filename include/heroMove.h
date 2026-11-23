#ifndef HEROMOVE_H
#define HEROMOVE_H

#include <GSgfx.h>
#include <types.h>

class AnalogStickEmu {
public:
    void init();
    s8 stickState[4];
    u8 accelerationStep;
    u8 decelerationStep;
};

// One collision event of a heroMove event list: the floor triangle stepped on
// and the script event it runs.
typedef struct {
    GSvec vertices[3];
    GSvec normal;
    u16 eventId;
    u8 reserved[2];
} HeroMoveEventList[4];

enum FootStepCounter {
    FOOT_STEP_COUNTER_WALK = 0,
    FOOT_STEP_COUNTER_BATTLE = 1,
    FOOT_STEP_COUNTER_TRAVEL = 2,
    FOOT_STEP_COUNTER_RECOVERY = 3
};

struct HeroMoveStepCallback {
    void (*function)(long context, FootStepCounter counter, long steps);
    long context;
};

enum HEROMOVE_MEMBER {
    HERO_MOVE_HERO = 0,
    HERO_MOVE_COMPANION_1 = 1,
    HERO_MOVE_COMPANION_2 = 2,
    HERO_MOVE_COMPANION_3 = 3,
    HERO_MOVE_MEMBER_COUNT
};

struct FOOTWORK {
    f32 values[4];
};

struct HeroMoveMember {
    u16 isMember;
    u8 _06[6];
    u32 neckMode;
    u8 _10[4];
    FOOTWORK footWork;
};

struct HeroMoveState {
    HEROMOVE_MEMBER leader;
    HeroMoveMember members[HERO_MOVE_MEMBER_COUNT];
    long leaderLogTimer; // 0x84: frames since the leader position was last logged.
    long leaderLogCount; // 0x88: positions the leader log holds, up to 60.
    GSvec leaderLog[60]; // 0x8C: past leader positions, newest first.
    f32 footStepDistance; // 0x35C: distance walked since the last foot step.
    HeroMoveStepCallback stepCallbacks[8];
    u32 poisonWalkCount; // 0x3A0: walking callbacks before poison damage is checked.
    u32 friendshipStepCount; // 0x3A4: accumulated steps processed for the companion.
    long lockFrames;
    u8 smoothFloorLink;
    u8 _3AD;
    u8 stickOverride;
    s8 stickState[4];
    u8 _3B3;
    int autoEvent[5]; // 0x3B4: floor script the next step runs.
    HeroMoveEventList eventList0;
    HeroMoveEventList eventList1;
    HeroMoveEventList eventList2;
    int eventListCounts[3]; // 0x638: collision entries in each event list.
    int stepPool[4]; // 0x644: pending steps for each counter.
    AnalogStickEmu analogStick;
    u8 _65A[2];
    unsigned long lastMoveFrame;
};

extern HeroMoveState HeroMove;

/* Resolve the people group and resource ID of a hero or follower member. */
bool getResID(u32* group, u32* resource, HEROMOVE_MEMBER member);

#ifdef __cplusplus
extern "C" {
#endif

void heroMoveInitEvent(void);
long heroMoveCheckEvent(HeroMoveEventList eventList);
void heroMoveAddAutoEvent(s32 scriptID, s32 scriptArgument1, s32 scriptArgument2, s32 scriptArgument3, s32 scriptArgument4);
void heroMoveSetColision(u8 enabled);
long heroMoveAddStepCallback(void (*callback)(long, FootStepCounter, long), long context);
/* Add steps to a counter for dispatch with the next movement update. */
void heroMovePoolCounter(FootStepCounter counter, long amount);
/* Clear all pending footstep counter steps. */
void heroMoveClearPoolCounter(void);
void heroMoveGetHeroPos(GSvec *pos);
void heroMoveGetHeroRot(GSvec *rot);
unsigned long heroGetFollower(void);
u8 heroMoveIsSmoothFloorLink(void);
u8 heroMoveDismissMember(s32 member);

#ifdef __cplusplus
}
#endif

#endif
