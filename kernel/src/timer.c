
#include "type.h"
#include "list.h"
#include "timer.h"
#include "kernel.h"
#include "irq.h"
#include "debug.h"
#include "irq.h"

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
	volatile uint32 CounterLo;
	volatile uint32 CounterHi;
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

#define GET_COUNTER ((uint64)CounterReg->CounterLo + ((uint64)CounterReg->CounterHi << 32))

bool TimerIdentifier(void* data) {

	if( TimerReg->MaskedIrq == 1 ) {
			
		TimerReg->IrqClear = 1;
		return TRUE;
	}

	return FALSE;
}

void TimerHandler(void* data) {

	uint64 counter = GET_COUNTER;
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
	TimerIrqHandler.num = IRQ_BASIC_TIMER;
	RegisterIrqHandler(&TimerIrqHandler);
}

void AddTimer(Timer_t* timer) {
	InitNode(&timer->n, 0, NULL);

	timer->lastCall = GET_COUNTER;

	Insert(&(GetKernel()->timers), (Node_t*)timer, NULL);
}

void RemoveTimer(Timer_t* timer) {
	
	RemoveNode(&(GetKernel()->timers), (Node_t*)timer);
}

uint64 GetTicks() {
	return GET_COUNTER;
}

void Sleep(uint64 microseconds) {

	uint64 current = GetTicks();
	uint64 prev = current;
	uint64 target = current + microseconds;
	while( current < target ) {
		current = GetTicks();
		if( current > prev + 1000 ) {
			prev = current;
		}
		//_Idle();
	}
}

