
#include "kernel.h"
#include "mem.h"
#include "debug.h"
#include "timer.h"
#include "irq.h"
#include "msgport.h"
#include "atag.h"

#include "uart.h"
#include "gpu.h"

extern uint32 _end;

int main(struct Atag* atags) {

	uart_init();

	InitKernel((uint8*)&_end);
	
	DebugAtags(atags);

	EnableInterrupts();

	while(1) {
		Sleep(1000000);
		//_Idle();
	}
	return 0;
}
