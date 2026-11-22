#include <NullSequence.h>
#include <new.h>
#include <wazaSequence.h>

// Stop and free every sequence still loaded on this stage.
void NullSequence::FreeAllWaza()
{
    WazaSequence *waza = this->waza;
    while (waza != NULL) {
        WazaSequence *next = waza->next;
        delete waza;
        waza = next;
    }
    this->waza = NULL;
}

// A stage stands where its sequences are placed, and how it is turned and
// scaled: the sequence system reads and writes that placement through these
// accessors. A plain stage keeps its placement in its own fields, and the
// Pokemon model overrides the accessors to move its model instead.

// Show or hide the stage.
void NullSequence::SetVisibility(bool visible)
{
    this->visible = visible;
}

void NullSequence::SetScale(GSvec *scale)
{
    this->scale.x = scale->x;
    this->scale.y = scale->y;
    this->scale.z = scale->z;
}

void NullSequence::SetRotation(GSvec *rotation)
{
    this->rotation.x = rotation->x;
    this->rotation.y = rotation->y;
    this->rotation.z = rotation->z;
}

void NullSequence::SetPosition(GSvec *position)
{
    this->position.x = position->x;
    this->position.y = position->y;
    this->position.z = position->z;
}

void NullSequence::GetScale(GSvec *scale)
{
    scale->x = this->scale.x;
    scale->y = this->scale.y;
    scale->z = this->scale.z;
}

void NullSequence::GetRotation(GSvec *rotation)
{
    rotation->x = this->rotation.x;
    rotation->y = this->rotation.y;
    rotation->z = this->rotation.z;
}

void NullSequence::GetPosition(GSvec *position)
{
    position->x = this->position.x;
    position->y = this->position.y;
    position->z = this->position.z;
}

/*
 * A stage owns the sequences loaded on it, so destroying one stops and frees
 * every sequence still attached to its stage. The stage is then unlinked from
 * the list of live stages.
 */
NullSequence::~NullSequence()
{
    NullSequence *head = g_Sequences;
    FreeAllWaza();

    if (head == NULL) {
        return;
    }
    if (head == this) {
        g_Sequences = this->next;
    } else {
        while (head->next != NULL) {
            if (head->next == this) {
                head->next = this->next;
                break;
            }
            head = head->next;
        }
    }
}

// A plain stage rests at the origin and is drawn at unit scale until the
// sequence system places it.
NullSequence::NullSequence()
    : modelID(0), visible(true), loadFailed(false), isModel(0),
      position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1),
      waza(NULL), renderCamera(NULL), next(NULL)
{
}

// Create a stage and put it on the list of live stages so that the sequence
// system can find it and place it. A failed allocation leaves the list alone.
NullSequence *New_NullSequence()
{
    NullSequence *stage = new NullSequence;
    if (stage == NULL) {
        return NULL;
    }
    stage->next = g_Sequences;
    g_Sequences = stage;
    return stage;
}
