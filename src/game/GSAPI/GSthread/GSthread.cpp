#include <GSthread.h>
#include <GSlog.h>
#include <GSmem.h>
#include <GSmemman.h>
#include <GSscratch.h>
#include <dolphin/os/OSCache.h>
#include <string.h>
#include <stdarg.h>

enum GSthreadSize {
    THREAD_NAME_SIZE = 32,
    THREAD_FPR_COUNT = 32,
    THREAD_LINKAGE_AREA_SIZE = 8,
    THREAD_STACK_TOP_WORDS = 2,
    THREAD_MAX_ARGS = 8,
    THREAD_NAME_COPY_LENGTH = THREAD_NAME_SIZE - 1
};

enum { THREAD_FIRST_ARG_REG = 3 };

enum GSthreadStackDirection {
    STACK_DIRECTION_NEXT = 0,
    STACK_DIRECTION_CURRENT = 1
};

struct GSthreadRegisterState {
    u32 gpr[32];
    u32 ctr;
    u32 lr;
};

struct GSthread {
    GSthread* prev;
    GSthread* next;
    bool used;
    bool blocked;
    bool groupBlocked;
    u8 priority;
    u32 groupId;
    u32 stackSize;
    bool running;
    bool terminateRequested;
    u8 usesFPU;
    u8 terminateOnExit;
    long result;
    GSthreadEntry entry;
    u32* stackBase;
    GSthreadRegisterState* regs;
    s32 heap;
    u32 savedLR;
    char name[THREAD_NAME_SIZE];
};

static GSthread* _activeThread;
static GSthread* _nextThread;
static GSthread* _threadListTop;
static bool _threadListChanged;
static u32 _saveLRReg;
static u32 _usesFPURegisters;
static u32 _tempGPR2;
static u32 _tempGPR1;
static GSthreadRegisterState* _destRegStatePtr;
static GSthreadRegisterState* _threadRegStatePtr;
static GSthreadRegisterState* _localRegStatePtr;
static GSthread* _threadList;
static u32 _maxThreads;

static GSthreadRegisterState _GSthreadRegisterState;

extern "C" {

void threadExecutePass(u32 groupId, bool useGroup);
void threadRemoveFromList(GSthread* thread);
void threadInsertIntoList(GSthread* thread);
void threadInitialize(GSthread* thread);
void threadRun(GSthread* thread, GSthreadRegisterState* state, u32* stack);
void threadExitFunc(long result);
u32* threadDMAStackToScratch(u32* stack, u32 size, GSthreadStackDirection direction);
GSthread* threadFindNext(GSthread* thread, u32 groupId, bool useGroup);
GSthread* threadFindUnused(void);

asm void threadLoadGPRRegisters(void)
{
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, _destRegStatePtr
    lwz r0, 0x0(r3)
    lwz r2, 0x8(r3)
    lwz r4, 0x10(r3)
    lwz r5, 0x14(r3)
    lwz r6, 0x18(r3)
    lwz r7, 0x1c(r3)
    lwz r8, 0x20(r3)
    lwz r9, 0x24(r3)
    lwz r10, 0x28(r3)
    lwz r11, 0x2c(r3)
    lwz r12, 0x30(r3)
    lwz r13, 0x34(r3)
    lwz r14, 0x38(r3)
    lwz r15, 0x3c(r3)
    lwz r16, 0x40(r3)
    lwz r17, 0x44(r3)
    lwz r18, 0x48(r3)
    lwz r19, 0x4c(r3)
    lwz r20, 0x50(r3)
    lwz r21, 0x54(r3)
    lwz r22, 0x58(r3)
    lwz r23, 0x5c(r3)
    lwz r24, 0x60(r3)
    lwz r25, 0x64(r3)
    lwz r26, 0x68(r3)
    lwz r27, 0x6c(r3)
    lwz r28, 0x70(r3)
    lwz r29, 0x74(r3)
    lwz r30, 0x78(r3)
    lwz r31, 0x7c(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

asm void threadLoadFPRRegisters(void)
{
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, _destRegStatePtr
    addi r3, r3, sizeof(GSthreadRegisterState)
    lfd f0, 0x0(r3)
    lfd f1, 0x8(r3)
    lfd f2, 0x10(r3)
    lfd f3, 0x18(r3)
    lfd f4, 0x20(r3)
    lfd f5, 0x28(r3)
    lfd f6, 0x30(r3)
    lfd f7, 0x38(r3)
    lfd f8, 0x40(r3)
    lfd f9, 0x48(r3)
    lfd f10, 0x50(r3)
    lfd f11, 0x58(r3)
    lfd f12, 0x60(r3)
    lfd f13, 0x68(r3)
    lfd f14, 0x70(r3)
    lfd f15, 0x78(r3)
    lfd f16, 0x80(r3)
    lfd f17, 0x88(r3)
    lfd f18, 0x90(r3)
    lfd f19, 0x98(r3)
    lfd f20, 0xa0(r3)
    lfd f21, 0xa8(r3)
    lfd f22, 0xb0(r3)
    lfd f23, 0xb8(r3)
    lfd f24, 0xc0(r3)
    lfd f25, 0xc8(r3)
    lfd f26, 0xd0(r3)
    lfd f27, 0xd8(r3)
    lfd f28, 0xe0(r3)
    lfd f29, 0xe8(r3)
    lfd f30, 0xf0(r3)
    lfd f31, 0xf8(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

asm void threadSaveGPRRegisters(void)
{
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, _destRegStatePtr
    stw r0, 0x0(r3)
    stw r2, 0x8(r3)
    stw r3, 0xc(r3)
    stw r4, 0x10(r3)
    stw r5, 0x14(r3)
    stw r6, 0x18(r3)
    stw r7, 0x1c(r3)
    stw r8, 0x20(r3)
    stw r9, 0x24(r3)
    stw r10, 0x28(r3)
    stw r11, 0x2c(r3)
    stw r12, 0x30(r3)
    stw r13, 0x34(r3)
    stw r14, 0x38(r3)
    stw r15, 0x3c(r3)
    stw r16, 0x40(r3)
    stw r17, 0x44(r3)
    stw r18, 0x48(r3)
    stw r19, 0x4c(r3)
    stw r20, 0x50(r3)
    stw r21, 0x54(r3)
    stw r22, 0x58(r3)
    stw r23, 0x5c(r3)
    stw r24, 0x60(r3)
    stw r25, 0x64(r3)
    stw r26, 0x68(r3)
    stw r27, 0x6c(r3)
    stw r28, 0x70(r3)
    stw r29, 0x74(r3)
    stw r30, 0x78(r3)
    stw r31, 0x7c(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

asm void threadSaveFPRRegisters(void)
{
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, _destRegStatePtr
    addi r3, r3, sizeof(GSthreadRegisterState)
    stfd f0, 0x0(r3)
    stfd f1, 0x8(r3)
    stfd f2, 0x10(r3)
    stfd f3, 0x18(r3)
    stfd f4, 0x20(r3)
    stfd f5, 0x28(r3)
    stfd f6, 0x30(r3)
    stfd f7, 0x38(r3)
    stfd f8, 0x40(r3)
    stfd f9, 0x48(r3)
    stfd f10, 0x50(r3)
    stfd f11, 0x58(r3)
    stfd f12, 0x60(r3)
    stfd f13, 0x68(r3)
    stfd f14, 0x70(r3)
    stfd f15, 0x78(r3)
    stfd f16, 0x80(r3)
    stfd f17, 0x88(r3)
    stfd f18, 0x90(r3)
    stfd f19, 0x98(r3)
    stfd f20, 0xa0(r3)
    stfd f21, 0xa8(r3)
    stfd f22, 0xb0(r3)
    stfd f23, 0xb8(r3)
    stfd f24, 0xc0(r3)
    stfd f25, 0xc8(r3)
    stfd f26, 0xd0(r3)
    stfd f27, 0xd8(r3)
    stfd f28, 0xe0(r3)
    stfd f29, 0xe8(r3)
    stfd f30, 0xf0(r3)
    stfd f31, 0xf8(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

asm void threadExecute(void)
{
    nofralloc
    stwu r1, -0xc(r1)
    stw r3, 0xc(r1)
    mflr r3
    stw r3, 0x8(r1)
    lwz r3, _localRegStatePtr
    stw r3, _destRegStatePtr
    bl threadSaveGPRRegisters
    stw r1, GSthreadRegisterState.gpr[1](r3)
    lwz r3, _threadRegStatePtr
    stw r3, _destRegStatePtr
    bl threadLoadGPRRegisters
    lwz r5, _usesFPURegisters
    cmplwi r5, 0
    beq skipFpr
    bl threadLoadFPRRegisters
skipFpr:
    lwz r5, GSthreadRegisterState.lr(r3)
    mtlr r5
    lwz r1, GSthreadRegisterState.gpr[1](r3)
    li r5, 0
    subi r5, r5, 1
    stw r5, 0x0(r1)
    lwz r5, GSthreadRegisterState.ctr(r3)
    mtctr r5
    lwz r5, GSthreadRegisterState.gpr[5](r3)
    lwz r3, GSthreadRegisterState.gpr[3](r3)
    bctr
}

static asm void threadExecuteResume(void)
{
    nofralloc
    lwz r3, 0x8(r1)
    mtlr r3
    lwz r3, 0xc(r1)
    addi r1, r1, 0xc
    blr
}

asm void _threadSwitch(void)
{
    nofralloc
    stw r3, _tempGPR1
    stw r5, _tempGPR2
    mflr r5
    lwz r3, _threadRegStatePtr
    stw r3, _destRegStatePtr
    bl threadSaveGPRRegisters
    stw r5, GSthreadRegisterState.ctr(r3)
    lwz r5, _tempGPR1
    stw r5, GSthreadRegisterState.gpr[3](r3)
    lwz r5, _tempGPR2
    stw r5, GSthreadRegisterState.gpr[5](r3)
    stw r1, GSthreadRegisterState.gpr[1](r3)
    lwz r5, _usesFPURegisters
    cmplwi r5, 0
    beq skipFpr
    bl threadSaveFPRRegisters
skipFpr:
    lwz r3, _localRegStatePtr
    stw r3, _destRegStatePtr
    bl threadLoadGPRRegisters
    lwz r5, GSthreadRegisterState.lr(r3)
    mtlr r5
    lwz r1, GSthreadRegisterState.gpr[1](r3)
    lwz r5, GSthreadRegisterState.gpr[5](r3)
    lwz r3, GSthreadRegisterState.gpr[3](r3)
    blr
}

void GSthreadSetName(GSthread* thread, const char* name)
{
    if (thread != NULL) {
        memcpy(thread->name, name, THREAD_NAME_COPY_LENGTH);
        thread->name[THREAD_NAME_COPY_LENGTH] = '\0';
    }
}

void GSthreadSetGroupID(GSthread* thread, u32 groupId)
{
    if (thread != NULL) {
        thread->groupId = groupId;
    }
}

void GSthreadSwitch(void)
{
    register GSthread* thread = _activeThread;
    if (thread != NULL) {
        asm {
            mflr r5
            stw r5, _saveLRReg
            stw r5, GSthread.savedLR(thread)
        }
        _threadSwitch();
    }
}

GSthread* GSthreadGetCurrentThread(void)
{
    return _activeThread;
}

void GSthreadUnblockGroup(u32 groupId)
{
    if (groupId == 0) {
        return;
    }
    for (u32 i = 0; i < _maxThreads; i++) {
        GSthread* thread = &_threadList[i];
        if (thread->groupId == groupId) {
            thread->groupBlocked = false;
        }
    }
    _threadListChanged = true;
}

void GSthreadBlockGroup(u32 groupId)
{
    if (groupId == 0) {
        return;
    }
    for (u32 i = 0; i < _maxThreads; i++) {
        GSthread* thread = &_threadList[i];
        if (thread->groupId == groupId) {
            thread->groupBlocked = true;
        }
    }
    _threadListChanged = true;
}

void GSthreadUnblock(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    thread->blocked = false;
    _threadListChanged = true;
}

void GSthreadBlock(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    thread->blocked = true;
    _threadListChanged = true;
}

void GSthreadExecuteAll(void)
{
    threadExecutePass(0, false);
}

void GSthreadExecuteGroup(u32 groupId)
{
    threadExecutePass(groupId, true);
}

long GSthreadClose(GSthread* thread)
{
    if (thread == NULL) {
        return 0;
    }
    if (thread->terminateOnExit == 1) {
        return 0;
    }
    long result = thread->result;
    thread->used = false;
    return result;
}

bool GSthreadIsRunning(GSthread* thread)
{
    if (thread == NULL) {
        return false;
    }
    return thread->running;
}

void GSthreadTerminateGroup(u32 groupId)
{
    for (GSthread* thread = _threadListTop; thread != NULL; thread = thread->next) {
        if (thread->groupId == groupId) {
            GSthreadTerminate(thread);
        }
    }
}

void GSthreadTerminate(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    if (thread == _activeThread || thread == _nextThread) {
        thread->terminateRequested = true;
        if (thread == _nextThread) {
            _threadListChanged = true;
        }
        return;
    }
    thread->running = false;
    thread->used = false;
    threadRemoveFromList(thread);
    if (thread->stackBase != NULL) {
        GSmemFreeHeap(thread->heap, thread->stackBase);
        thread->stackBase = NULL;
    }
    if (thread->regs != NULL) {
        GSmemFreeHeap(thread->heap, thread->regs);
        thread->regs = NULL;
    }
}

void GSthreadSetArgs(GSthread* thread, s32 count, ...)
{
    s32 i;
    s32 index;
    s32 regCount;
    GSthreadRegisterState* state;
    u32* stackBase;
    va_list args;

    if (thread != NULL && thread->running != true) {
        stackBase = thread->stackBase;
        state = thread->regs;
        va_start(args, count);
        index = THREAD_FIRST_ARG_REG;
        regCount = THREAD_MAX_ARGS;
        if (count < THREAD_MAX_ARGS) {
            regCount = count;
        }
        for (i = 0; i < regCount; i++) {
            state->gpr[index++] = va_arg(args, u32);
        }
        if (count > THREAD_MAX_ARGS) {
            state->gpr[1] -= (count - i) * sizeof(u32);
            index = state->gpr[1] / sizeof(u32) + THREAD_STACK_TOP_WORDS;
            for (i = THREAD_MAX_ARGS; i < count; i++) {
                stackBase[index++] = va_arg(args, u32);
            }
        }
        va_end(args);
    }
}

GSthread* GSthreadCreate(s32 priority, u32 groupId, u32 stackSize, u8 usesFPU, s32 terminateOnExit, GSthreadEntry entry)
{
    s32 heap;
    GSthread* thread;
    u32 stateSize;

    if (usesFPU == 0) {
        GSlogWrite("GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n");
    }
    thread = threadFindUnused();
    if (thread == NULL) {
        return NULL;
    }
    heap = GSmemGetCurrentHeap();
    thread->stackBase = (u32*)GSmemAllocAlignHeap(heap, stackSize, ALIGNMENT);
    if (thread->stackBase == NULL) {
        return NULL;
    }
    stateSize = sizeof(GSthreadRegisterState);
    if (usesFPU != 0) {
        stateSize += THREAD_FPR_COUNT * sizeof(f64);
    }
    thread->regs = (GSthreadRegisterState*)GSmemAllocHeap(heap, stateSize);
    if (thread->regs == NULL) {
        return NULL;
    }
    thread->used = true;
    thread->groupId = groupId;
    thread->stackSize = stackSize;
    thread->running = false;
    thread->terminateRequested = false;
    thread->usesFPU = usesFPU;
    thread->entry = entry;
    thread->blocked = false;
    thread->groupBlocked = false;
    thread->priority = priority;
    thread->prev = NULL;
    thread->next = NULL;
    thread->terminateOnExit = terminateOnExit;
    thread->heap = heap;
    thread->savedLR = 0;
    thread->name[0] = '\0';
    threadInitialize(thread);
    threadInsertIntoList(thread);
    _threadListChanged = true;
    return thread;
}

u32 GSthreadInit(u32 maxThreads)
{
    _maxThreads = maxThreads;
    _threadList = (GSthread*)GSmemAlloc(maxThreads * sizeof(GSthread));
    if (_threadList == NULL) {
        return 0;
    }
    for (u32 i = 0; i < _maxThreads; i++) {
        _threadList[i].used = false;
    }
    _activeThread = NULL;
    _threadListTop = NULL;
    GSlogWrite("GSthread: Init OK, maximum of %d threads\n", _maxThreads);
    return 1;
}

void threadRemoveFromList(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    if (thread->prev != NULL) {
        thread->prev->next = thread->next;
    }
    if (thread->next != NULL) {
        thread->next->prev = thread->prev;
    }
    if (_threadListTop == thread) {
        _threadListTop = thread->next;
    }
}

void threadInsertIntoList(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    if (_threadListTop == NULL) {
        _threadListTop = thread;
        return;
    }
    GSthread* current = _threadListTop;
    while (current->next != NULL && current->priority < thread->priority) {
        current = current->next;
    }
    if (current->next == NULL && current->priority < thread->priority) {
        thread->prev = current;
        thread->next = NULL;
        current->next = thread;
        return;
    }
    if (current->prev != NULL) {
        current->prev->next = thread;
    }
    thread->prev = current->prev;
    thread->next = current;
    current->prev = thread;
    if (_threadListTop == current) {
        _threadListTop = thread;
    }
}

void threadExecutePass(u32 groupId, bool useGroup)
{
    GSthreadRegisterState* regs;
    u32* stack;
    u32* scratch;
    u32* nextStack;
    u32* nextScratch;

    GSthread* thread = threadFindNext(NULL, groupId, useGroup);
    if (thread == NULL) {
        return;
    }
    stack = thread->stackBase;
    regs = thread->regs;
    scratch = threadDMAStackToScratch(stack, thread->stackSize, STACK_DIRECTION_CURRENT);
    while (thread != NULL) {
        _nextThread = threadFindNext(thread, groupId, useGroup);
        if (_nextThread != NULL) {
            nextStack = _nextThread->stackBase;
            nextScratch = threadDMAStackToScratch(nextStack, _nextThread->stackSize, STACK_DIRECTION_NEXT);
        }
        threadRun(thread, regs, scratch);
        if (thread->terminateRequested) {
            if (GSscratchIsPtr(scratch)) {
                GSscratchFree(scratch);
            }
            GSthreadTerminate(thread);
        } else if (GSscratchIsPtr(scratch)) {
            DCFlushRange(stack, thread->stackSize);
            GSscratchStore(stack, scratch, thread->stackSize);
            GSscratchWaitForCompletion();
            GSscratchFree(scratch);
        }
        if (!_threadListChanged || _nextThread == threadFindNext(thread, groupId, useGroup)) {
            thread = _nextThread;
            if (thread != NULL) {
                regs = thread->regs;
                scratch = nextScratch;
                stack = nextStack;
            }
        } else {
            if (GSscratchIsPtr(nextScratch)) {
                GSscratchWaitForCompletion();
                GSscratchFree(nextScratch);
            }
            if (_nextThread != NULL && _nextThread->terminateRequested) {
                GSthread* terminated = _nextThread;
                _nextThread = NULL;
                GSthreadTerminate(terminated);
            }
            _nextThread = threadFindNext(thread, groupId, useGroup);
            thread = _nextThread;
            if (thread != NULL) {
                stack = thread->stackBase;
                regs = thread->regs;
                scratch = threadDMAStackToScratch(stack, thread->stackSize, STACK_DIRECTION_CURRENT);
            }
        }
        _threadListChanged = false;
    }
}

void threadExitFunc(long result)
{
    GSthread* thread = _activeThread;
    thread->result = result;
    if (thread->terminateOnExit == 1) {
        GSthreadTerminate(thread);
    } else {
        thread->terminateRequested = true;
        thread->running = false;
        threadRemoveFromList(thread);
        _threadListChanged = true;
    }
    GSthreadSwitch();
}

void threadRun(GSthread* thread, GSthreadRegisterState* state, u32* stack)
{
    _activeThread = thread;
    thread->running = true;
    state->gpr[1] = (u32)stack + state->gpr[1];
    _GSthreadRegisterState.lr = (u32)threadExecuteResume;
    _localRegStatePtr = &_GSthreadRegisterState;
    _threadRegStatePtr = state;
    _usesFPURegisters = thread->usesFPU;
    threadExecute();
    state->gpr[1] -= (u32)stack;
    _activeThread = NULL;
}

void threadInitialize(GSthread* thread)
{
    if (thread == NULL) {
        return;
    }
    GSthreadRegisterState* state = thread->regs;
    u32* stack = thread->stackBase;
    _destRegStatePtr = state;
    threadSaveGPRRegisters();
    if (thread->usesFPU) {
        threadSaveFPRRegisters();
    }
    state->gpr[1] = thread->stackSize - THREAD_LINKAGE_AREA_SIZE;
    state->ctr = (u32)thread->entry;
    state->lr = (u32)threadExitFunc;
    stack[0] = -1;
}

u32* threadDMAStackToScratch(u32* stack, u32 size, GSthreadStackDirection direction)
{
    return stack;
}

GSthread* threadFindNext(GSthread* thread, u32 groupId, bool useGroup)
{
    GSthread* current = thread == NULL ? _threadListTop : thread->next;
    while (current != NULL) {
        if (current->used && !current->blocked && !current->groupBlocked &&
            !current->terminateRequested &&
            (!useGroup || current->groupId == groupId)) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

GSthread* threadFindUnused(void)
{
    for (u32 i = 0; i != _maxThreads; i++) {
        GSthread* thread = &_threadList[i];
        if (!thread->used) {
            return thread;
        }
    }
    return NULL;
}

}
