
#include "msgport.h"
#include "mem.h"
#include "kernel.h"
#include "list.h"
#include "signal.h"

MessagePort_t* CreatePort(const char *name) {
	
	Kernel_t* k = GetKernel();

	return CreatePortForTask(k->currentTask, name);
}

MessagePort_t* CreatePortForTask(Task_t* task, const char *name) {

	SignalBit_t sig = AllocSignalForTask(task, SIG_ANY);
	if( sig == SIG_ANY ) {
		// No signals available!
		return NULL;
	}

	MessagePort_t* port = (MessagePort_t*)AllocMem(sizeof(MessagePort_t));
	InitNode(&port->n, 0, name);

	port->owner = task;
	port->signal = sig;

	InitList(&port->messages, NT_MSGPORT, LF_NONE);

	if( name == NULL ) {
		Insert(&port->owner->msgPorts, (Node_t*)port, NULL);
	} else {
		Insert(&GetKernel()->msgPorts, (Node_t*)port, NULL);
	}

	return port;
}

MessagePort_t* FindPort(const char *name) {

	Kernel_t* k = GetKernel();

	return (MessagePort_t*)FindNode(&k->msgPorts, name);
}

void RemovePort(MessagePort_t* port) {

	if( port->n.name == NULL ) {
		RemoveNode(&port->owner->msgPorts, (Node_t*)port);
	} else {
		Kernel_t* k = GetKernel();
		RemoveNode(&k->msgPorts, (Node_t*)port);
	}
}

#define ALIGN_USERDATA (sizeof(Message_t) % 4 == 0 ? 0 : (4 - (sizeof(Message_t) % 4)))


Message_t* AllocMessage(uint32 length) {

	return (Message_t*)AllocMem(sizeof(Message_t) 
					+ ALIGN_USERDATA
					+ length);
}

void *GetUserData(Message_t* msg) {
	return (void*)((uint8*)msg + sizeof(Message_t) + ALIGN_USERDATA);
}

void SetUserData(Message_t* msg, void* data, uint32 length) {

	uint8* src = data;
	uint8* dst = GetUserData(msg);
	for( uint32 n = 0; n < length; n++ ) {
		*(dst++) = *(src++);
	}
}

void FreeMessage(Message_t* message) {
	
	FreeMem(message);
}

Message_t* WaitMessage(MessagePort_t* port) {

	SetSignalMask(GetSignalMask() | port->signal);
	Wait();
	SignalBit_t sigs = GetSignals();
	while( ! (sigs & port->signal) ){
		SetSignalMask(GetSignalMask() | port->signal);
		Wait();
		sigs = GetSignals();
	}
	return GetMessage(port);
}

Message_t* GetMessage(MessagePort_t* port) {
	
	return (Message_t*)RemoveHead(&(port->messages));
}

void SendMessage(MessagePort_t* port, Message_t* message) {

	Insert(&(port->messages), (Node_t*)message, NULL);
	Signal(port->owner, port->signal);
}

void ReplyMessage(Message_t* message) {

	SendMessage(message->replyPort, message);
}

