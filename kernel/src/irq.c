
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

#define NUM_TO_IRQ(n) (1 << (n & (IRQ_COUNT_PER_REG - 1)))

void IdentifyIrq(void) {

	Kernel_t* k = GetKernel();

	uint32 pendingBasic = IrqReg->IrqBasicPending;
	if( pendingBasic != 0 ) {
		IrqReg->IrqBasicPending = pendingBasic;
	}
	uint32 pending1 = IrqReg->IrqPending1;
	if( pending1 != 0 ) {
		IrqReg->IrqPending1 = pending1;
	}
	uint32 pending2 = IrqReg->IrqPending2;
	if( pending2 != 0 ) {
		IrqReg->IrqPending2 = pending2;
	}

	IrqHandler_t* h = (IrqHandler_t*)k->irqHandlers.head;
	while( h != NULL ) {
		uint32 irqNum = NUM_TO_IRQ(h->num);

		if( h->num < IRQ_2_BASE && (pending1 & irqNum ) != irqNum) {
			goto cont;
		}
		else if( h->num >= IRQ_2_BASE && h->num < IRQ_BASIC_BASE && ( pending2 & irqNum) != irqNum ) {
			goto cont;
		}
		else if( h->num >= IRQ_BASIC_BASE && (pendingBasic & irqNum) != irqNum ){
			goto cont;
		}

		if( (h->identifier == NULL || h->identifier(h->data) == TRUE) ) {
			h->flags = IRQ_FLAG_PENDING;
		}
cont:
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

#define InvalidateInstructionCache()    \
		                        __asm volatile ("mcr p15, 0, %0, c7, c5,  0" : : "r" (0) : "memory")
#define FlushPrefetchBuffer()   __asm volatile ("mcr p15, 0, %0, c7, c5,  4" : : "r" (0) : "memory")
#define FlushBranchTargetCache()        \
								 __asm volatile ("mcr p15, 0, %0, c7, c5,  6" : : "r" (0) : "memory")
#define InvalidateDataCache()   __asm volatile ("mcr p15, 0, %0, c7, c6,  0" : : "r" (0) : "memory")
#define CleanDataCache()        __asm volatile ("mcr p15, 0, %0, c7, c10, 0" : : "r" (0) : "memory")

#define DataSyncBarrier()       __asm volatile ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define DataMemBarrier()        __asm volatile ("mcr p15, 0, %0, c7, c10, 5" : : "r" (0) : "memory")

void InitInterrupts() {
	
	CleanDataCache ();
	DataSyncBarrier ();

	InvalidateInstructionCache ();
	FlushBranchTargetCache ();
	DataSyncBarrier ();

	FlushPrefetchBuffer ();
		
	DisableInterrupts();

	IrqReg->FiqControl = 0;
	IrqReg->DisableIrq1 = (uint32)-1;
	IrqReg->DisableIrq2 = (uint32)-1;
	IrqReg->DisableBasicIrq = (uint32)-1;

	IrqReg->IrqBasicPending = IrqReg->IrqBasicPending;
	IrqReg->IrqPending1 = IrqReg->IrqPending1;
	IrqReg->IrqPending2 = IrqReg->IrqPending2;

	DataMemBarrier();

	Kernel_t* k = GetKernel();

	InitList(&(k->irqHandlers), NT_IRQ, LF_NONE);

}

void UpdateBasicIrq() {

	uint32 irq1 = 0;
	uint32 irq2 = 0;
	uint32 irqBasic = 0;

	Kernel_t* k = GetKernel();

	IrqHandler_t* h = (IrqHandler_t*)k->irqHandlers.head;
	while( h != NULL ) {

		if( h->num < IRQ_2_BASE ) {
			irq1 |= NUM_TO_IRQ(h->num);
		} else if( h->num < IRQ_BASIC_BASE ) {
			irq2 |= NUM_TO_IRQ(h->num);
		} else {
			irqBasic |= NUM_TO_IRQ(h->num);
		}

		h = (IrqHandler_t*)h->n.next;
	}

	DebugPrintf("IRQ: Update %x %x %x\n", irq1, irq2, irqBasic);

	IrqReg->EnableBasicIrq |= irqBasic;
	IrqReg->EnableIrq1 |= irq1;
	IrqReg->EnableIrq2 |= irq2;
}

void EnableInterrupts() {

//	UpdateBasicIrq();
	_EnableIrq();
}

void DisableInterrupts() {
	_DisableIrq();
}

void RegisterIrqHandler(IrqHandler_t* handler) {

	DebugPrintf("IRQ: Registering handler for %x (%x)...", handler->num, NUM_TO_IRQ(handler->num));

	InitNode(&handler->n, 0, NULL);	
	handler->flags = 0;

	Kernel_t* k = GetKernel();
	Insert(&(k->irqHandlers), (Node_t*)handler, NULL);

//	UpdateBasicIrq();

	if( handler->num < IRQ_2_BASE ) {
		IrqReg->EnableIrq1 |= NUM_TO_IRQ(handler->num);
	} else if( handler->num < IRQ_BASIC_BASE ) {
		IrqReg->EnableIrq2 |= NUM_TO_IRQ(handler->num);
	} else {
		IrqReg->EnableBasicIrq |= NUM_TO_IRQ(handler->num);
	}
	DebugPrintf("Done.\n");
}

void DeregisterIrqHandler(IrqHandler_t* handler) {
	
	Kernel_t* k = GetKernel();
	RemoveNode(&(k->irqHandlers), (Node_t*)handler);

	if( handler->num < IRQ_2_BASE ) {
		IrqReg->DisableIrq1 |= NUM_TO_IRQ(handler->num);
	} else if( handler->num < IRQ_BASIC_BASE ) {
		IrqReg->DisableIrq2 |= NUM_TO_IRQ(handler->num);
	} else {
		IrqReg->DisableBasicIrq |= NUM_TO_IRQ(handler->num);
	}
	//UpdateBasicIrq();
}


