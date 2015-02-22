
#ifndef __IRQ_H__
#define __IRQ_H__

#include "list.h"

typedef enum {
	IRQ_FLAG_PENDING = 1
} IrqFlag_t;


typedef void (*IrqHandlerCallback_t)(void);

typedef bool (*IrqIdentifierCallback_t)(void);

typedef struct {
	Node_t n;
	IrqHandlerCallback_t handler;
	IrqIdentifierCallback_t identifier;
	IrqFlag_t flags;
} IrqHandler_t;

void InitInterrupts();

void RegisterIrqHandler(IrqHandler_t* handler);

void EnableInterrupts();

void DisableInterrupts();

#endif // __IRQ_H__
