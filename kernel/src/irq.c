
#include "list.h"
#include "irq.h"
#include "uart.h"

extern void _EnableIrq();
extern void _DisableIrq();

typedef struct {
	volatile uint32 IrqBasicPending;
	volatile uint32 IrqPending1;
	volatile uint32 IrqPending2;
	volatile uint32 FiqControl;
	volatile uint32 EnableIrq1;
	volatile uint32 EnableIrq2;
	volatile uint32 EnableBasicIrq;
	volatile uint32 DisableIrq1;
	volatile uint32 DisableIrq2;
	volatile uint32 DisableBasicIrq;
} IrqReg_t;

static volatile IrqReg_t* IrqReg = (IrqReg_t*)0x2000B200;

static volatile List_t HandlerList;

#define IRQ_PENDING_1 0x40000000
#define	IRQ_PENDING_2 0x80000000

void IdentifyIrq(void) {
	
	IrqHandler_t* h = (IrqHandler_t*)HandlerList.head;
	while( h != NULL ) {
		if( h->identifier() == TRUE ) {
			h->flags = IRQ_FLAG_PENDING;
		}
		h = (IrqHandler_t*)h->n.next;
	}
}

void HandleIrq(void) {
	

	IrqHandler_t* h = (IrqHandler_t*)HandlerList.head;
	while( h != NULL ) {
		
		if( (h->flags & IRQ_FLAG_PENDING) == IRQ_FLAG_PENDING ) {

			h->flags = 0;
			h->handler();
		}

		h = (IrqHandler_t*)h->n.next;
	}
}

void InitInterrupts() {
	InitList((List_t*)&HandlerList, NT_IRQ, LF_NONE);
}

void EnableInterrupts() {
	
	IrqReg->EnableBasicIrq = 1; //IRQ_TIMER;
	
	_EnableIrq();
}

void DisableInterrupts() {
	_DisableIrq();
}

void RegisterIrqHandler(IrqHandler_t* handler) {

	InitNode(&handler->n, 0, NULL);	
	handler->flags = 0;
	Insert((List_t*)&HandlerList, (Node_t*)handler, NULL);
}

