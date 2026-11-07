#include <types.h>
#include <GSinput.h>
#include <menuSub.h>
#include <dbgMenu.h>
#include <GSmsg.h>
#include <GSsndPlayback.h>
#include <GSthread.h>
#include <menuIds.h>
#include <floor.h>
#include <floorDataBios.h>
#include <roomData.h>
#include <GSAPI/GSfsys/GSfsys.h>

#pragma force_active on

// Pad buttons the floor-change debug entry reacts to.
enum {
    FLOOR_CHANGE_PAD_TRIGGER_R = 0x20,
    FLOOR_CHANGE_PAD_TRIGGER_L = 0x40,
    FLOOR_CHANGE_PAD_BUTTON_Y = 0x800,
};

// Message shown when a floor change cannot be started.
static const char sFloorChangeBusyMsg[48] = "このフロアからフロアチェンジはおこなえません";

// Floor the debug entry changes to, keyed by the floor it is used on. The
// entry looks the current floor up here to find the floor linked to it.
typedef struct FloorChangeEntry {
    s32 floorID;
    s32 linkedFloorID;
} FloorChangeEntry;

static FloorChangeEntry floorChangeTable[247] = {
    {38, 163}, {39, 164}, {40, 100}, {41, 101}, {42, 102}, {43, 96},
    {44, 97}, {45, 98}, {46, 99}, {47, 103}, {48, 104}, {49, 94},
    {50, 95}, {51, 105}, {53, 107}, {52, 93}, {54, 119}, {55, 120},
    {56, 117}, {57, 122}, {58, 118}, {59, 115}, {60, 116}, {61, 121},
    {62, 123}, {63, 108}, {64, 109}, {65, 110}, {66, 111}, {67, 113},
    {68, 112}, {69, 132}, {70, 125}, {71, 126}, {72, 135}, {73, 127},
    {74, 128}, {75, 133}, {76, 134}, {77, 129}, {78, 130}, {79, 131},
    {105, 11}, {106, 7}, {107, 8}, {108, 9}, {109, 10}, {110, 1},
    {111, 2}, {112, 3}, {113, 4}, {114, 20}, {115, 21}, {116, 32},
    {117, 23}, {118, 33}, {119, 24}, {120, 34}, {121, 25}, {122, 17},
    {123, 26}, {124, 18}, {125, 27}, {126, 19}, {127, 28}, {128, 12},
    {129, 29}, {130, 13}, {131, 30}, {132, 14}, {133, 31}, {134, 15},
    {135, 16}, {136, 35}, {137, 39}, {138, 36}, {139, 44}, {140, 43},
    {141, 42}, {142, 37}, {143, 40}, {144, 41}, {145, 38}, {146, 57},
    {147, 58}, {148, 46}, {149, 49}, {150, 50}, {151, 51}, {152, 59},
    {153, 60}, {154, 61}, {155, 45}, {175, 71}, {176, 64}, {177, 65},
    {178, 66}, {179, 67}, {180, 70}, {181, 69}, {182, 68}, {183, 86},
    {184, 88}, {185, 72}, {186, 87}, {187, 76}, {188, 77}, {189, 78},
    {190, 79}, {191, 80}, {192, 81}, {193, 82}, {194, 83}, {195, 84},
    {196, 85}, {197, 73}, {198, 75}, {199, 74}, {200, 89}, {157, 165},
    {158, 166}, {159, 167}, {156, 168}, {160, 173}, {161, 169}, {162, 170},
    {163, 171}, {164, 172}, {213, 174}, {202, 910}, {203, 920}, {204, 921},
    {205, 922}, {206, 923}, {207, 924}, {209, 927}, {210, 928}, {211, 900},
    {482, 913}, {212, 918}, {208, 183}, {201, 995}, {168, 175}, {169, 176},
    {170, 177}, {171, 178}, {172, 179}, {173, 180}, {174, 181}, {215, 4001},
    {283, 407}, {284, 408}, {285, 409}, {286, 410}, {287, 411}, {288, 422},
    {289, 423}, {290, 424}, {291, 425}, {292, 426}, {293, 427}, {294, 428},
    {295, 429}, {296, 430}, {297, 431}, {298, 432}, {299, 433}, {300, 434},
    {80, 143}, {81, 140}, {82, 141}, {83, 142}, {84, 138}, {85, 139},
    {86, 153}, {87, 154}, {88, 155}, {89, 156}, {90, 157}, {91, 144},
    {92, 145}, {93, 146}, {94, 151}, {95, 152}, {96, 147}, {97, 148},
    {98, 149}, {99, 150}, {100, 158}, {101, 159}, {102, 160}, {103, 161},
    {104, 162}, {214, 400}, {216, 406}, {165, 90}, {166, 91}, {167, 92},
    {217, 182}, {235, 500}, {236, 501}, {237, 544}, {238, 502}, {239, 106},
    {240, 503}, {241, 504}, {242, 505}, {243, 112}, {244, 506}, {245, 507},
    {246, 508}, {247, 509}, {248, 510}, {249, 541}, {250, 540}, {251, 542},
    {252, 543}, {253, 514}, {254, 515}, {255, 529}, {256, 516}, {257, 528},
    {258, 517}, {259, 518}, {260, 519}, {261, 520}, {262, 530}, {263, 527},
    {264, 548}, {265, 521}, {266, 522}, {267, 523}, {268, 524}, {269, 533},
    {270, 531}, {271, 532}, {272, 550}, {273, 537}, {274, 538}, {275, 534},
    {276, 535}, {277, 536}, {278, 549}, {279, 174}, {280, 545}, {281, 546},
    {282, 547},
};

// Changes the floor linked to `floorID`. Pressing R first asks for the link
// mode with the number entry; pressing L or Y frees the linked floor's
// cached resources before the change is made.
s32 dbgMenuFloorChange(s32 floorID)
{
    s32 mode;
    s32 i;

    mode = 0;
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & FLOOR_CHANGE_PAD_TRIGGER_R) {
        if (!menuSubOpenNumberInputDigit(0, &mode)) {
            return -1;
        }
    }

    for (i = 0; i < 247; i++) {
        if (floorChangeTable[i].floorID == floorID) {
            if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & FLOOR_CHANGE_PAD_TRIGGER_L) ||
                (GSinputButtonsPressed(GS_INPUT_PORT_1) & FLOOR_CHANGE_PAD_BUTTON_Y)) {
                GSfsysFreeCache(mapGetGroupID(
                    floorDataBiosGetPtr(floorChangeTable[i].linkedFloorID)));
            }
            floorLink(floorChangeTable[i].linkedFloorID, mode);
            break;
        }
    }
    return 0;
}

// Reports an error while the player is on a pushed floor. Otherwise, when
// R or Y is pressed, asks for a floor number and links to that floor.
s32 dbgMenuFloorCheck(void)
{
    s32 floorID;
    s32 i;

    if (GSfloorIsFloorPushed()) {
        GSsndPlayNow(MENU_SE_ERROR);
        for (i = 0; i < 15; i++) {
            GSprint(dbgMenuGetLayerID(), 200, 240, -1, sFloorChangeBusyMsg);
            GSthreadSwitch();
        }
        return 0;
    }

    if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & FLOOR_CHANGE_PAD_TRIGGER_R) ||
        (GSinputButtonsPressed(GS_INPUT_PORT_1) & FLOOR_CHANGE_PAD_BUTTON_Y)) {
        if (!menuSubOpenNumberInputDigit(0, &floorID)) {
            return -1;
        }
        floorLink(floorID, 0);
        return 0;
    }
    return 1;
}

#pragma force_active reset
