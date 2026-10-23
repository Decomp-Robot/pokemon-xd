#ifndef TRK_TRK_H
#define TRK_TRK_H

#include <types.h>

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
    s32 lock;              // lock word handed to TRKLock/TRKUnlock
    int count;             // events currently queued
    s32 next;              // slot the next TRKGetNextEvent takes from
    TRKEvent events[NUBQUEUELENGTH];  // ring of pending events
    NubEventID eventID;       // id handed to the next posted event
} TRKEventQueue;

typedef struct {
    s32 lock;                      // lock word handed to TRKLock/TRKUnlock
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


/* MWTRACE levels used by the serial link: the higher the level, the more
   unusual the condition being reported. */
#define TRK_TRACE_LEVEL_DETAIL 1
#define TRK_TRACE_LEVEL_DEBUG  4
#define TRK_TRACE_LEVEL_ERROR  8

extern void *TRK_memcpy(void *dst, const void *src, unsigned long n);
extern void *TRK_memset(void *dst, int c, unsigned long n);

/* The debug nub declares its own non-variadic OSReport prototype. */
extern void OSReport(const char *msg);
extern void MWTRACE(int level, const char *fmt, ...);
extern void TRK_board_display(const char *msg);
extern void InitializeProgramEndTrap(void);
extern s32 TRKInitializeIntDrivenUART(u32 baudRate, s32 channel, s32 flags, u8 **inputPendingPtr);
extern void TRKTargetSetInputPendingPtr(u8 *inputPendingPtr);
extern s32 TRKTargetStopped(void);
extern void TRKTargetContinue(void);
extern void TRKTargetInterrupt(TRKEvent *event);
extern void TRKTargetSupportRequest(void);
extern s32 TRKInitializeTarget(void);
extern s32 TRKDispatchMessage(TRKBuffer *message);
extern s32 TRKInitializeDispatcher(void);
extern s32 TRKWriteUARTN(const void *data, u32 length);
extern s32 TRKReadUARTN(void *data, u32 length);
extern s32 TRKPollUART(void);
extern void SetTRKConnected(s32 connected);
extern s32 TRKIsConnected(void);
extern void usr_put_initialize(void);
extern BOOL usr_puts_serial(const char *str);
// All three return a DSError status (DS_NoError on success), which the
// event-queue and message-buffer callers ignore.
extern DSError TRKLock(void *lock);
extern DSError TRKUnlock(void *lock);
extern DSError TRKLockInit(void *lock);

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
