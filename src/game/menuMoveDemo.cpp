#include <GSsnd.h>
#include <WorldMap.h>
#include <floorManager.h>

/* Floor the move demo is played on. */
#define FLOOR_ID_MOVE_DEMO 0x3A9

/* Sound played while the move demo is running. */
#define SE_ID_MOVE_DEMO 0x5B8

class kyogreMoveDemo : public WorldMapModel {
public:
    kyogreMoveDemo();
    ~kyogreMoveDemo();
    void Main();

private:
    u8 _04[0x08];
};

kyogreMoveDemo* pMDemo;
s32 _movdemo_type;

/* Move demo menu entry points. */
#pragma force_active on

/* Stop the demo sound, release the model and clear the singleton. */
extern "C" void menuMoveDemoExit(void)
{
    GSsndStopNow(SE_ID_MOVE_DEMO);
    delete pMDemo;
    pMDemo = 0;
}

/* Advance the move demo by one frame. */
extern "C" void menuMoveDemoMain(void)
{
    pMDemo->Main();
}

/* Create the move demo model. */
extern "C" void menuMoveDemoInit(void)
{
    pMDemo = new kyogreMoveDemo();
}

#pragma force_active reset

/* Remember the requested demo and change to the floor it is played on. */
extern "C" void menuMoveDemo(s32 demo)
{
    _movdemo_type = demo;
    floorChange(FLOOR_ID_MOVE_DEMO);
}
