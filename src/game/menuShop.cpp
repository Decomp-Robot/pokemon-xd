#include <types.h>
#include <memcard.h>
#include <winMsg.h>

class VPC_BOX;

// Writes the current save data to the memory card and reports the task's success.
bool memCardSave(VPC_BOX *box)
{
    return memcardTaskCall(MEMCARD_TASK_WRITE, WINMSG_WINDOW_MENU, 0) == MEMCARD_RESULT_WRITTEN;
}
