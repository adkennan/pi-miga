
#include "list.h"
#include "irq.h"
#include "kernel.h"
#include "debug.h"

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

#define IRQ_PENDING_1 0x40000000
#define	IRQ_PENDING_2 0x80000000

void IdentifyIrq(void) {

	Kernel_t* k = GetKernel();

	IrqHandler_t* h = (IrqHandler_t*)k->irqHandlers.head;
	while( h != NULL ) {
		if( (IrqReg->IrqBasicPending & (1 << (h->num - 1))) 
			&& (h->identifier == NULL || h->identifier(h->data) == TRUE) ) {
			h->flags = IRQ_FLAG_PENDING;
		}
		h = (IrqHandler_t*)h->n.next;
	}
}

void HandleIrq(void) {

	Kernel_t* k = GetKernel();

	IrqHandler_t* h = (IrqHandler_t*)k->irqHandlers.head;
	while( h != NULL ) {
		
		if( (h->flags & IRQ_FLAG_PENDING) == IRQ_FLAG_PENDING ) {

			h->flags = 0;
			h->handler(h->data);
		}

		h = (IrqHandler_t*)h->n.next;
	}
}

void InitInterrupts() {

	Kernel_t* k = GetKernel();

	InitList(&(k->irqHandlers), NT_IRQ, LF_NONE);
}

void UpdateBasicIrq() {

	uint32 n = 0;

	Kernel_t* k = GetKernel();

	IrqHandler_t* h = (IrqHandler_t*)k->irqHandlers.head;
	while( h != NULL ) {
		n |= 1 << (h->num - 1);
		
		h = (IrqHandler_t*)h->n.next;
	}

	IrqReg->EnableBasicIrq = n;
}

void EnableInterrupts() {
	
	UpdateBasicIrq();
	_EnableIrq();
}

void DisableInterrupts() {
	_DisableIrq();
}

void RegisterIrqHandler(IrqHandler_t* handler) {

	DebugPrintf("Registering handler for %x\n", handler->num);

	InitNode(&handler->n, 0, NULL);	
	handler->flags = 0;

	Kernel_t* k = GetKernel();
	Insert(&(k->irqHandlers), (Node_t*)handler, NULL);

	UpdateBasicIrq();
}

void DeregisterIrqHandler(IrqHandler_t* handler) {
	
	Kernel_t* k = GetKernel();
	RemoveNode(&(k->irqHandlers), (Node_t*)handler);

	UpdateBasicIrq();
}


