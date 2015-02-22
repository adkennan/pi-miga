
#include "type.h"

#ifndef __EXEC_H__
#define __EXEC_H__


typedef enum {
	NT_MEMORY,
	NT_MSGPORT,
	NT_MESSAGE,
	NT_TASK,
	NT_TIMER,
	NT_IRQ,
	NT_LIBRARY
} NodeType_t;

typedef enum {
	LF_NONE = 0,
	LF_SORTED = 0x1
} ListFlag_t;

typedef struct Node_t Node_t;

struct Node_t {
	Node_t* prev;
	Node_t* next;
	int8 priority;
	const char* name;
};

typedef struct List_t {
	Node_t* head;
	Node_t* tail;
	NodeType_t type;
	ListFlag_t flags;
} List_t;

typedef void (*TimerCallback_t)(void*);

typedef enum {
	TM_SINGLE,
	TM_REPEAT
} TimerType_t;

typedef struct {
	Node_t n;
	uint32	frequency;
	TimerCallback_t callBack;
	TimerType_t timerType;
	void* data;
	uint64 lastCall;
} Timer_t;

typedef enum {
	TS_READY,
	TS_RUNNING,
	TS_SLEEPING
} TaskState_t;

typedef void (*TaskStart_t)(void);

typedef uint32* StackPtr_t;

typedef struct {
	Node_t n;
	uint32 id;
	List_t msgPorts;
	TaskState_t state;
	TaskStart_t start;
	StackPtr_t stackPtr;
	uint32 sigAlloc;
	uint32 sigWait;
	uint32 sigReceived; 
} Task_t;

typedef struct {
	uint8* heapBase;
	List_t mem;
	List_t timers;
	List_t msgPorts;
	List_t tasks;
	List_t libraries;
	Timer_t schedTimer;
	Task_t* currentTask;
} Kernel_t;


typedef enum {
	SIG_ANY = -1,
	SIG_BREAK = 0
} SignalBit_t;


typedef struct {
	Node_t n;
	SignalBit_t signal;
	Task_t* owner;
	List_t messages;
} MessagePort_t;

typedef struct {
	Node_t n;
	MessagePort_t* destPort;
	MessagePort_t* replyPort;
	uint32 length;
} Message_t;


typedef struct {
	void (*Init)();
	void (*Open)();
	void (*Close)();
	void (*Expunge)();
} Interface_t;

typedef struct {
	Node_t n;
	uint32 openCount;
	uint32 version;
	Interface_t* interface;
} Library_t;

typedef struct {
	Interface_t baseIface;

	/*
	 * Memory
	 */

	void* (*AllocMem)(size_t);
	void (*FreeMem)(void*);

	/*
	 * Signals
	 */

	SignalBit_t (*AllocSignal)(SignalBit_t);
	void (*DeallocSignal)(SignalBit_t);
	void (*Signal)(Task_t*, uint32);
	uint32 (*GetSignals)(void);
	void (*SetSignalMask)(uint32);
	uint32 (*GetSignalMask)(void);
	void (*Wait)(void);

	/*
	 * Message Ports
	 */

	MessagePort_t* (*CreatePort)(const char*);
	MessagePort_t* (*FindPort)(const char*);
	void (*RemovePort)(MessagePort_t*);
	Message_t* (*AllocMessage)(uint32);
	void* (*GetUserData)(Message_t*);
	void (*SetUserData)(Message_t*, void*, uint32);
	void (*FreeMessage)(Message_t*);
	Message_t* (*WaitMessage)(MessagePort_t*);
	Message_t* (*GetMessage)(MessagePort_t*);
	void (*SendMessage)(MessagePort_t*, Message_t*);
	void (*ReplyMessage)(Message_t*);

	/*
	 * Libraries
	 */

	Library_t* (*CreateLibrary)(const char*, uint32, Interface_t*);
	void (*AddLibrary)(Library_t*);
	Library_t* (*OpenLibrary)(const char*, uint32);
	void (*CloseLibrary)(Library_t*);

	/*
	 * Tasks
	 */

	Task_t* (*CreateTask)(const char*, int8, uint32, TaskStart_t start);
	Task_t* (*FindTask)(const char*);
	void (*StartTask)(Task_t*);

} IExec_t;

extern IExec_t** IExec;

#endif // __EXEC_H__
