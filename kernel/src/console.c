
#include "type.h"
#include "exec.h"
#include "error.h"

#include "device.h"
#include "kernel.h"
#include "msgport.h"
#include "gpu.h"

#define CONSOLE_DEVICE_NAME "console.device"
#define CONSOLE_DEVICE_STACK 4096

void ConsoleDevice(void) {

	Task_t* task = FindTask(NULL);
	MessagePort_t* port = (MessagePort_t*)task->msgPorts.head;
	uint32 openCount = 0;
	while( TRUE ) {

		IOStdReq_t* req = (IOStdReq_t*)WaitMessage(port);
		if( req == NULL ) {
			continue;
		}

		switch( req->r.command ) {
			case CMD_OPEN:
				openCount++;
				req->r.error = 0;
				break;

			case CMD_CLOSE:
				if( openCount > 0 ) {
					openCount--;
					req->r.error = 0;
				} else {
					req->r.error = ERR_DEVICE_NOT_OPEN;
				}
				break;

			case CMD_WRITE:
				for( uint32 ix = 0; ix < req->length; ix++ ) {
					PutChar(*(req->data + ix));
				}
				req->r.error = 0;
				break;

			default:
				req->r.error = ERR_CMD_NOT_SUPPORTED;
				break;
		}	

		ReplyMessage((Message_t*)req);
	}
}

void StartConsole() {

	CreateDevice(CONSOLE_DEVICE_NAME, CONSOLE_DEVICE_STACK, ConsoleDevice);
}
