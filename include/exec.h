
#include <type.h>

#ifndef __EXEC_H__
#define __EXEC_H__

#include <exec/list.h>
#include <exec/timer.h>
#include <exec/task.h>
#include <exec/msgport.h>
#include <exec/device.h>
#include <exec/library.h>

/*! \brief The interface to the Exec library.
 *
 */
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

	/*
	 * Devices
	 */

	Device_t* (*CreateDevice)(const char* name, uint32 stackSize, TaskStart_t start);
	IORequest_t* (*CreateIORequest)(MessagePort_t* port, uint32 size);
	void (*DeleteIORequest)(IORequest_t* request);
	uint32 (*OpenDevice)(const char *name, uint32 unit, IORequest_t* request);
	uint32 (*CloseDevice)(IORequest_t* request);
	uint32 (*DoIO)(IORequest_t* request);
	void (*SendIO)(IORequest_t* request);
	bool (*CheckIO)(IORequest_t* request);
	uint32 (*WaitIO)(IORequest_t* request);
	uint32 (*AbortIO)(IORequest_t* request);

} IExec_t;

extern IExec_t* IExec;

#endif // __EXEC_H__
