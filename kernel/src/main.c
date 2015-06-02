
#include "kernel.h"
#include "mem.h"
#include "debug.h"
#include "timer.h"
#include "irq.h"
#include "msgport.h"
#include "atag.h"

#include "uart.h"
#include "gpu.h"

#include <exec.h>
#include <device/serial.h>

extern uint32 _end;

void SerialWriter(void) {

	DebugPrintf("starting task...\n");
	
	MessagePort_t* port = (MessagePort_t*)IExec->FindTask(NULL)->msgPorts.head;
	IOSerReq_t* req = (IOSerReq_t*)IExec->CreateIORequest(port, sizeof(IOSerReq_t));

	if( IExec->OpenDevice("serial.device", 0, (IORequest_t*)req) ) {
		DebugPrintf("Failed to open serial.device.\n");
	}
	DebugPrintf("Opened serial.device.\n");

	req->r.r.command = CMD_WRITE;
	req->r.length = -1;
	
	while(1) {
		DebugPrintf("Tick!\n");
		req->r.data = (uint8*)"Tick!\n";
		IExec->DoIO((IORequest_t*)req);
		Sleep(1000000);

		DebugPrintf("Tock!\n");
		req->r.data = (uint8*)"Tock!\n";
		IExec->DoIO((IORequest_t*)req);
		Sleep(1000000);
	}
}

int main(struct Atag* atags) {

//	uart_init();

	InitKernel((uint8*)&_end);
	
	DebugAtags(atags);

	EnableInterrupts();

	Task_t* t = IExec->CreateTask("tiktok", 10, 4096, SerialWriter);
	IExec->StartTask(t);

	while(1) {
		Sleep(1000000);
		//_Idle();
	}
	return 0;
}
