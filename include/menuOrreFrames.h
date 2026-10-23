#ifndef MENU_ORRE_FRAMES_H
#define MENU_ORRE_FRAMES_H

// The frame labels of the team-count flash scene, and the from/to pairs each
// selection step plays while the team grows towards the label of its size.
struct MenuOrreFrameLabels {
    const char *label[5];
};
struct MenuOrreFramePairs {
    const char *pair[10];
};

// The label strings the tables point at, owned by the selection code.
extern const char menuOrreFrameStart[];
extern const char menuOrreFrame1End[];
extern const char menuOrreFrame2End[];
extern const char menuOrreFrame3End[];
extern const char menuOrreFrame4End[];

#endif
