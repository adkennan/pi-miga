
#include "kernel.h"
#include "mem.h"
#include "uart.h"
#include "timer.h"
#include "irq.h"
#include "msgport.h"

extern uint32 _end;


void Pinger() {

	uart_puts("Starting Pinger\n");

		Sleep(1000000);

	Task_t* o = FindTask("Ponger");

	MessagePort_t* op = (MessagePort_t*)o->msgPorts.head;
	Message_t* m = AllocMessage(6);
	SetUserData(m, (void*)"Ping!", 6);
	
	MessagePort_t* mp = (MessagePort_t*)GetKernel()->currentTask->msgPorts.head;
	while(TRUE) {
		
		SendMessage(op, m);

		Sleep(1000000);

		Message_t* mm = WaitMessage(mp);
		if( mm != NULL ) {
			const char* mc = (const char *)GetUserData(mm);
			uart_puts("Pinger: ");
			uart_puts(mc);
			uart_putc('\n');
		}
	}
}

void Ponger() {

	uart_puts("Starting Ponger\n");

		Sleep(1000000);

	Task_t* o = FindTask("Pinger");
	MessagePort_t* op = (MessagePort_t*)o->msgPorts.head;
	Message_t* m = AllocMessage(6);
	SetUserData(m, (void*)"Pong!", 6);
	
	MessagePort_t* mp = (MessagePort_t*)GetKernel()->currentTask->msgPorts.head;
	while(TRUE) {
		
		Sleep(1000000);
		
		SendMessage(op, m);

		Message_t* mm = WaitMessage(mp);
		if( mm != NULL ) {
			const char* mc = (const char *)GetUserData(mm);
			uart_puts("Ponger: ");
			uart_puts(mc);
			uart_putc('\n');
		}
	}
}

int main(int argc, char* argv[]) {

	uart_init();

	uart_puts("\nInit\n\n");

	InitKernel((uint8*)&_end);

	uart_puts("\nReady\n");

	EnableInterrupts();
	
	Task_t* t = CreateTask("Pinger", 0, 1024, Pinger);
	StartTask(t);
	
	t = CreateTask("Ponger", 0, 1024, Ponger);
	StartTask(t);

	while(1) {
		Sleep(1000000);
	}
	return 0;
}
