
#ifndef __IRQ_H__
#define __IRQ_H__

#include "list.h"

typedef enum {
	IRQ_FLAG_PENDING = 1
} IrqFlag_t;

typedef enum {
	IRQ_TIMER,
	IRQ_MAILBOX,
	IRQ_DOORBELL_0,
	IRQ_DOORBELL_1,
	IRQ_GPU_0_HALT,
	IRQ_GPU_1_HALT,
	IRQ_ILLEGAL_ACCESS_1,
	IRQ_ILLEGAL_ACCESS_0,
	IRQ_PENDING_REG_1,
	IRQ_PENDING_REG_2,
	IRQ_GPU_7,
	IRQ_GPU_9,
	IRQ_GPU_10,
	IRQ_GPU_18,
	IRQ_GPU_19,
	IRQ_GPU_53,
	IRQ_GPU_54,
	IRQ_GPU_55,
	IRQ_GPU_56,
	IRQ_GPU_57
} IrqNumber_t;

typedef void (*IrqHandlerCallback_t)(void* data);

typedef bool (*IrqIdentifierCallback_t)(void* data);

typedef struct {
	Node_t n;
	IrqNumber_t num;
	void* data;
	IrqHandlerCallback_t handler;
	IrqIdentifierCallback_t identifier;
	IrqFlag_t flags;
} IrqHandler_t;

void InitInterrupts();

void RegisterIrqHandler(IrqHandler_t* handler);

void DeregisterIrqHandler(IrqHandler_t* handler);

void EnableInterrupts();

void DisableInterrupts();

#endif // __IRQ_H__
