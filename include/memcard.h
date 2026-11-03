#ifndef MEMCARD_H
#define MEMCARD_H

#include <types.h>

/* Requests memcardTaskCall() hands to the memcard task thread, and the
   results it reports back once a task is done. The thread knows further
   request codes for the same card work; a load request leaves the save data
   in memory and reports MEMCARD_RESULT_LOADED, a save request writes the save
   file and reports MEMCARD_RESULT_WRITTEN, and a delete request removes the
   save file and leaves it deleted. */
enum MemcardTaskRequest {
    MEMCARD_TASK_LOAD = 2,
    MEMCARD_TASK_SAVE = 11,
    MEMCARD_TASK_DELETE = 12
};

enum MemcardTaskResult {
    MEMCARD_RESULT_LOADED = 3,
    MEMCARD_RESULT_WRITTEN = 4
};

#ifdef __cplusplus
extern "C" {
#endif
/* msgType is the window kind from <winMsg.h> the task opens its messages in;
   the memcard message routine passes it through to winMsgOpen(). Zero selects
   WINMSG_WINDOW_MENU. */
s32 memcardTaskCall(s32 task, s32 msgType, s32 argument);
void memcardSetupTask(long task, long msgType, long argument);
u8 memcardCheckTaskWaitGBA();
void memcardUnlockWaitGBA(u8 abort);
long memcardGetTaskResult();
void memcardFinalizeTask();


void *memcardMemAllocForSavedata(void);
void *memcardMemFreeForSavedata(void *save);

// Debug menu transfers of a save image through the memcard task.
void memcard_debug_Load(void *save);
void memcard_debug_Load2(void);
void memcard_debug_Save(void *save);

#ifdef __cplusplus
}
#endif

#endif
