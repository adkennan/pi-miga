
#include "exec.h"
#include "device.h"
#include "mem.h"
#include "kernel.h"
#include "error.h"
#include "msgport.h"
#include "list.h"
#include "signal.h"

#define DEV_PRIORITY 5

Device_t* CreateDevice(const char* name, uint32 stackSize, TaskStart_t start) {

	Device_t* dev = (Device_t*)AllocMem(sizeof(Device_t));
	InitNode((Node_t*)dev, 0, name);

	dev->task = CreateTask(name, DEV_PRIORITY, stackSize, start);

	Kernel_t* k = GetKernel();
	Insert(&(k->devices), (Node_t*)dev, NULL);

	StartTask(dev->task);

	return dev;
}

IORequest_t* CreateIORequest(MessagePort_t* port, uint32 size) {
	
	IORequest_t* r = (IORequest_t*)AllocMem(size);
	r->m.replyPort = port;
	r->m.length = size;

	return r;
}

void DeleteIORequest(IORequest_t* request) {

	FreeMem((void*)request);
}

uint32 OpenDevice(const char *name, uint32 unit, IORequest_t* request) {

	Kernel_t* k = GetKernel();

	Device_t* dev = (Device_t*)FindNode(&(k->devices), name);
	if( dev == NULL ) {
		request->error = ERR_DEVICE_NOT_FOUND;
		return request->error;
	}

	request->device = dev;
	request->m.destPort = (MessagePort_t*)dev->task->msgPorts.head;

	request->command = CMD_OPEN;
	if( DoIO(request) ) {
		return request->error;
	}

	return 0;
}

uint32 CloseDevice(IORequest_t* request) {

	request->command = CMD_CLOSE;
	if( DoIO(request) ) {
		return request->error;
	}

	return 0;
}

uint32 DoIO(IORequest_t* request) {

	SendIO(request);

	return WaitIO(request);
}

void SendIO(IORequest_t* request) {

	SendMessage(request->m.destPort, (Message_t*)request);
}

bool CheckIO(IORequest_t* request) {

	SignalBit_t sigs = GetSignals();
	if( sigs & request->m.destPort->signal ) {
		return TRUE;
	}
	return FALSE;
}

uint32 WaitIO(IORequest_t* request) {

	WaitMessage(request->m.replyPort);

	return request->error;
}

uint32 AbortIO(IORequest_t* request) {

	return 0;
}
