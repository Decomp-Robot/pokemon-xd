#ifndef TRK_TRK_H
#define TRK_TRK_H

#include <types.h>
#include <trk_trace.h>

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

// Shared declarations for the MetroTRK nub: the part of the debugger library
// that runs on the target.  The nub owns the event queue the target's
// interrupt and support request handlers post into, the pool of message
// buffers used to receive host packets, and the serial link that reads those
// packets in.

typedef s32 DSError;
typedef s32 MessageBufferID;
typedef u32 NubEventID;

#define kMessageBufferCount 3
#define kMessageBufferSize  0x880u

/* Pending events are held in a small ring; the nub never queues more than
   one message and one stop/step request at a time. */
#define NUBQUEUELENGTH 2

/* The host protocol prefixes every message with a fixed size header holding
   the message type and the total packet length, including the header. */
#define kMessageHeaderSize 0x40

/* The host owns the low message ids, so the first event the nub posts gets
   this id. */
#define kTRKFirstEventID 0x100

/* Rate the nub drives the debugger UART at. */
#define kTRKHostBaudRate 57600

/* Status codes reported by the queue and buffer routines. */
#define DS_NoError                  0
#define DS_EventQueueFull            0x100
#define DS_NoMessageBufferAvailable 0x300
#define DS_MessageBufferOverflow    0x301
#define DS_MessageBufferReadError   0x302

/* Debugger request commands and the fixed header fields they use. */
typedef enum {
    kDSConnect = 1,
    kDSDisconnect = 2,
    kDSReset = 3,
    kDSVersions = 4,
    kDSSupportMask = 5,
    kDSOverride = 7,
    kDSReadMemory = 16,
    kDSWriteMemory = 17,
    kDSReadRegisters = 18,
    kDSWriteRegisters = 19,
    kDSSetOption = 23,
    kDSContinue = 24,
    kDSStep = 25,
    kDSStop = 26
} DSCommand;

enum {
    kDSCommandOffset = 4,
    kDSOptionsOffset = 8,
    kDSLengthOffset = 12,
    kDSAddressOffset = 16,
    kDSRangeEndOffset = 20,
    kDSFirstRegisterOffset = 12,
    kDSLastRegisterOffset = 16,
    kDSOptionValueOffset = 12,
    kDSStepCountOffset = 12
};

enum {
    kDSStepIntoCount = 0,
    kDSStepIntoRange = 1,
    kDSStepOverCount = 16,
    kDSStepOverRange = 17,
    kDSOptionSerialIO = 1,
    kDSRegistersDefault = 0,
    kDSRegistersFP = 1,
    kDSRegistersExtended1 = 2,
    kDSRegistersExtended2 = 3
};

#define kDSMemoryExtended 0x02
#define kDSMemoryUserView 0x08
#define kDSMemoryARAM     0x40
#define DS_DispatchError 0x500
#define DS_InvalidMemory 0x700
#define DS_InvalidRegister 0x701
#define DS_CWDSException 0x702
#define DS_UnsupportedError 0x703
#define DS_InvalidProcessId 0x704
#define DS_InvalidThreadId 0x705
#define DS_OsError 0x706

/* Event queue slots.  Events of type NUBEVENT_Null are never posted; the
   switch in TRKNubMainLoop falls through and drops such an event.  A step
   request is serviced the same way as a full interrupt. */
typedef enum {
    NUBEVENT_Null = 0,
    NUBEVENT_Shutdown,
    NUBEVENT_Request,
    NUBEVENT_Breakpoint,
    NUBEVENT_Exception,
    NUBEVENT_Support
} NubEventType;

typedef struct {
    NubEventType eventType;   // what the event asks the nub to do
    NubEventID eventID;       // handed out by TRKPostEvent
    MessageBufferID msgBufID; // index into the buffer pool, -1 if none
} TRKEvent;

typedef struct {
    s32 lock;              // lock word handed to TRKAcquireMutex/TRKReleaseMutex
    int count;             // events currently queued
    s32 next;              // slot the next TRKGetNextEvent takes from
    TRKEvent events[NUBQUEUELENGTH];  // ring of pending events
    NubEventID eventID;       // id handed to the next posted event
} TRKEventQueue;

typedef struct {
    s32 lock;                      // lock word handed to TRKAcquireMutex/TRKReleaseMutex
    s32 isInUse;                   // non-zero while a message is being received
    u32 length;                    // number of bytes valid in data
    u32 position;                  // read/write cursor inside data
    u8  data[kMessageBufferSize];
} TRKBuffer;

/* Frame receiver state machine.  The packet I/O path this nub uses only
   ever sits in the wait state. */
typedef enum {
    kWaitFlag = 0,
    kFoundFlag,
    kInFrame,
    kFrameOverflow
} ReceiverState;

typedef struct {
    MessageBufferID msgBufID; // buffer a posted message came from, -1 idle
    TRKBuffer *buffer;    // buffer the frame is being read into
    ReceiverState receiveState; // position of the frame state machine
    s32 isEscape;         // next byte of the frame is escaped
    u32 fcs;              // running frame checksum
    u8  pad[3];
} TRKSerialHandlerState;


extern void *TRK_memcpy(void *dst, const void *src, unsigned long n);
extern void *TRK_memset(void *dst, int c, unsigned long n);

/* The debug nub declares its own non-variadic OSReport prototype. */
extern void OSReport(const char *msg);
extern void TRK_board_display(const char *msg);
extern void InitializeProgramEndTrap(void);
extern s32 TRKInitializeIntDrivenUART(u32 baudRate, s32 channel, s32 flags, u8 **inputPendingPtr);
extern void TRKTargetSetInputPendingPtr(u8 *inputPendingPtr);
extern s32 TRKTargetStopped(void);
extern u32 TRKTargetContinue(void);
extern DSError TRKTargetStop(void);
extern u32 TRKTargetGetPC(void);
extern DSError TRKTargetSingleStep(u32 count, BOOL stepOver);
extern DSError TRKTargetStepOutOfRange(u32 start, u32 end, BOOL stepOver);
extern DSError TRKTargetAccessDefault(u32 first, u32 last, TRKBuffer *buffer, u32 *length, BOOL read);
extern DSError TRKTargetAccessFP(u32 first, u32 last, TRKBuffer *buffer, u32 *length, BOOL read);
extern DSError TRKTargetAccessExtended1(u32 first, u32 last, TRKBuffer *buffer, u32 *length, BOOL read);
extern DSError TRKTargetAccessExtended2(u32 first, u32 last, TRKBuffer *buffer, u32 *length, BOOL read);
extern DSError TRKTargetAccessMemory(void *data, u32 address, u32 *length, s32 debuggerView, BOOL read);
extern DSError TRKTargetAccessARAM(void *data, u32 address, u32 *length, BOOL read);
extern void __TRK_reset(void);
extern void __TRK_copy_vectors(void);
extern void TRKSetUseSerialIO(u8 value);
extern void TRKTargetInterrupt(TRKEvent *event);
extern void TRKTargetSupportRequest(void);
extern s32 TRKInitializeTarget(void);
extern s32 TRKDispatchMessage(TRKBuffer *message);
extern s32 TRKInitializeDispatcher(void);
extern s32 TRKWriteUARTN(void *data, u32 length);
extern s32 TRKReadUARTN(void *data, u32 length);
extern s32 TRKPollUART(void);
extern void SetTRKConnected(BOOL connected);
extern BOOL GetTRKConnected(void);
DSError TRKDoConnect(TRKBuffer *message);
DSError TRKDoDisconnect(TRKBuffer *message);
DSError TRKDoReset(TRKBuffer *message);
DSError TRKDoVersions(TRKBuffer *message);
DSError TRKDoSupportMask(TRKBuffer *message);
DSError TRKDoOverride(TRKBuffer *message);
DSError TRKDoReadMemory(TRKBuffer *message);
DSError TRKDoWriteMemory(TRKBuffer *message);
DSError TRKDoReadRegisters(TRKBuffer *message);
DSError TRKDoWriteRegisters(TRKBuffer *message);
DSError TRKDoContinue(TRKBuffer *message);
DSError TRKDoStep(TRKBuffer *message);
DSError TRKDoStop(TRKBuffer *message);
DSError TRKDoSetOption(TRKBuffer *message);
void OutputData(u8 *data, int length);

extern void usr_put_initialize(void);
extern BOOL usr_puts_serial(const char *str);
// All three return a DSError status (DS_NoError on success), which the
// event-queue and message-buffer callers ignore.
extern DSError TRKAcquireMutex(void *lock);
extern DSError TRKReleaseMutex(void *lock);
extern DSError TRKInitializeMutex(void *lock);

typedef enum {
    kDSNotifyStopped = 144,
    kDSNotifyException = 145
} MessageCommandID;

extern DSError TRKDoNotifyStopped(MessageCommandID command);
extern DSError TRKTargetAddStopInfo(TRKBuffer *buffer);
extern DSError TRKTargetAddExceptionInfo(TRKBuffer *buffer);
extern DSError TRKRequestSend(TRKBuffer *request, MessageBufferID *replyID,
                             u32 minReplySize, u32 retryCount, BOOL useTimeout);

extern TRKEventQueue gTRKEventQueue;
extern TRKBuffer gTRKMessageBuffers[kMessageBufferCount];
extern TRKSerialHandlerState gTRKSerialHandlerState;
extern BOOL gTRKBigEndian;
extern u8 *gTRKInputPendingPtr;

/* Text handed to the host's trace and terminal output routines.  The nub
   keeps each message in a named constant so the layout report it sends on
   start up stays in step with the packet reader. */
extern const char sTRKNoBufferAvailable[];
extern const char sTRKMessageSend[];
extern const char sTRKPacketHeaderMessage[];
extern const char sTRKReadMemoryMessage[];
extern const char sTRKWriteMemoryMessage[];
extern const char sTRKConnectMessage[];
extern const char sTRKReplyAckMessage[];
extern const char sTRKReadRegistersMessage[];
extern const char sTRKFreeBufferMessage[];
extern const char sTRKReadingPayloadMessage[];
extern const char sTRKInvalidHeaderSizeMessage[];
extern const char sTRKInvalidPacketMessage[];
extern const char sTRKReturningMessage[];



// Nub entry points.  The event queue, the message buffers and the serial
// link call each other in both directions, so the whole set is declared up
// front.


void TRKNubMainLoop(void);
void TRKDestructEvent(TRKEvent *event);
void TRKConstructEvent(TRKEvent *event, NubEventType eventType);
DSError TRKPostEvent(TRKEvent *event);
BOOL TRKGetNextEvent(TRKEvent *event);
DSError TRKInitializeEventQueue(void);
void TRKNubWelcome(void);
DSError TRKTerminateNub(void);
DSError TRKInitializeNub(void);
DSError TRKMessageSend(TRKBuffer *message);
DSError TRKReadBuffer_ui32(TRKBuffer *buffer, u32 *dest, s32 count);
DSError TRKReadBuffer_ui8(TRKBuffer *buffer, u8 *dest, s32 count);
DSError TRKReadBuffer1_ui64(TRKBuffer *buffer, u64 *dest);
DSError TRKAppendBuffer_ui32(TRKBuffer *buffer, const u32 *src, s32 count);
DSError TRKAppendBuffer_ui8(TRKBuffer *buffer, const u8 *src, s32 count);
DSError TRKAppendBuffer1_ui64(TRKBuffer *buffer, u64 value);
DSError TRKReadBuffer(TRKBuffer *buffer, void *dest, unsigned long size);
DSError TRKAppendBuffer(TRKBuffer *buffer, const void *src, u32 size);
DSError TRKSetBufferPosition(TRKBuffer *buffer, u32 position);
void TRKResetBuffer(TRKBuffer *buffer, BOOL keepData);
void TRKReleaseBuffer(MessageBufferID bufferID);
TRKBuffer *TRKGetBuffer(MessageBufferID bufferID);
DSError TRKGetFreeBuffer(MessageBufferID *bufferID, TRKBuffer **buffer);
DSError TRKInitializeMessageBuffers(void);
DSError TRKTerminateSerialHandler(void);
DSError TRKInitializeSerialHandler(void);
void TRKProcessInput(MessageBufferID bufferID);
void TRKGetInput(void);
MessageBufferID TRKTestForPacket(void);

/* MetroTRK messages are big endian on the wire, so the read and append buffer
   routines below reverse the multi byte fields as they are copied, on a
   little endian target.  The append routines that need both widths store one
   byte directly and anything wider through the library's byte copy, so that
   idiom is shared as TRK_STORE_BYTES. */
#define TRK_STORE_BYTES(buffer, src, size)                      \
    do {                                                        \
        if ((size) == 1) {                                      \
            (buffer)->data[(buffer)->position] = *(const u8 *)(src); \
        } else {                                                \
            TRK_memcpy((buffer)->data + (buffer)->position,      \
                       (src), (size));                          \
        }                                                       \
    } while (0)

#endif
