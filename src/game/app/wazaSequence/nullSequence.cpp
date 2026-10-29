#include <NullSequence.h>

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
