
#include "type.h"
#include "list.h"
#include "timer.h"
#include "uart.h"
#include "kernel.h"
#include "irq.h"

#define IRQ_TIMER 1

#define TMR_CTRL_16BIT 0 << 1
#define TMR_CTRL_23BIT 1 << 1

#define TMR_CTRL_PS1 0 << 2
#define TMR_CTRL_PS16 1 << 2
#define TMR_CTRL_PS256 2 << 2

#define TMR_CTRL_INT_ENABLE 1 << 5
#define TMR_CTRL_INT_DISABLE 0 << 5

#define TMR_CTRL_ENABLE 1 << 7
#define TMR_CTRL_DISABLE 0 << 7

extern void _Idle(void);

typedef struct {
	volatile uint32 CounterStatus;
	volatile uint64 Counter;
	volatile uint32 Compare0;
	volatile uint32 Compare1;
	volatile uint32 Compare2;
	volatile uint32 Compare3;
} CounterReg_t;

typedef struct {
	volatile uint32 Load;
	volatile uint32 Value;
	volatile uint32 Control;
	volatile uint32 IrqClear;
	volatile uint32 RawIrq;
	volatile uint32 MaskedIrq;
	volatile uint32 Reload;
	volatile uint32 PreDivider;
	volatile uint32 Counter;
} TimerReg_t;

static volatile CounterReg_t* CounterReg = (CounterReg_t*)0x20003000;
static volatile TimerReg_t* TimerReg = (TimerReg_t*)0x2000B400;
static IrqHandler_t TimerIrqHandler;

bool TimerIdentifier(void) {


	if( TimerReg->MaskedIrq == 1 ) {
			
		TimerReg->IrqClear = 1;
		return TRUE;
	}

	return FALSE;
}

void TimerHandler(void) {

	uint64 counter = CounterReg->Counter;

	Timer_t* c = (Timer_t*)(GetKernel()->timers).head;
	//
	Timer_t* n = NULL;
	while( c != NULL ) {

		n = (Timer_t*)c->n.next;

		if( c->lastCall + c->frequency <= counter ) {
			
			c->callBack(c->data);

			if( c->timerType == TM_REPEAT ) {
	
				c->lastCall = counter;

			} else {
				RemoveTimer(c);
			}
		}

		c = n;
	}
}

void InitTimer() {

	TimerReg->Load = 10000;
	TimerReg->PreDivider = 0x7d;

	TimerReg->Control = TMR_CTRL_23BIT 
					  //| TMR_CTRL_PS256 
					  | TMR_CTRL_INT_ENABLE 
					  | TMR_CTRL_ENABLE;

	TimerIrqHandler.handler = &TimerHandler;
	TimerIrqHandler.identifier = &TimerIdentifier;
	RegisterIrqHandler(&TimerIrqHandler);
}

void AddTimer(Timer_t* timer) {
	InitNode(&timer->n, 0, NULL);

	timer->lastCall = CounterReg->Counter;

	Insert(&(GetKernel()->timers), (Node_t*)timer, NULL);
}

void RemoveTimer(Timer_t* timer) {
	
	RemoveNode(&(GetKernel()->timers), (Node_t*)timer);
}

uint64 GetTicks() {
	return CounterReg->Counter;
}

void Sleep(uint64 microseconds) {
	uint64 target = GetTicks() + microseconds;
	while( GetTicks() < target ) {
		_Idle();
	}
}

