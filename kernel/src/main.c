
#include "kernel.h"
#include "mem.h"
#include "debug.h"
#include "timer.h"
#include "irq.h"
#include "msgport.h"
#include "atag.h"

#include "uart.h"

extern uint32 _end;

void Pinger() {

	MessagePort_t* port = IExec->CreatePort(NULL);
	IORequest_t* req = IExec->CreateIORequest(port, sizeof(IOStdReq_t));

	if( IExec->OpenDevice("console.device", 0, req) ) {
		DebugPrintf("Error: %x\n", req->error);
	}

	req->command = CMD_WRITE;

	IOStdReq_t* stdReq = (IOStdReq_t*)req;
	stdReq->data = (uint8*)" Ping!\n";
	stdReq->length = 7;

	while(TRUE) {

		IExec->DoIO(req);

		Sleep(1000000);
	}
}

void Ponger() {

	MessagePort_t* port = IExec->CreatePort(NULL);
	IORequest_t* req = IExec->CreateIORequest(port, sizeof(IOStdReq_t));

	if( IExec->OpenDevice("console.device", 0, req) ) {
		DebugPrintf("Error: %x\n", req->error);
	}

	req->command = CMD_WRITE;

	IOStdReq_t* stdReq = (IOStdReq_t*)req;
	stdReq->data = (uint8*)" Pong!\n";
	stdReq->length = 7;

	while(TRUE) {

		Sleep(1000000);
		
		IExec->DoIO(req);
	}
}

int main(struct Atag* atags) {

	uart_init();

	DebugAtags(atags);

	InitKernel((uint8*)&_end);

	EnableInterrupts();
	
	Task_t* t = CreateTask("Pinger", 10, 1024, Pinger);
	StartTask(t);

	t = CreateTask("Ponger", 10, 1024, Ponger);
	StartTask(t);

	while(1) {
		Sleep(1000000);
	}
	return 0;
}
