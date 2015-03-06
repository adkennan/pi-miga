
#include "uspios.h"
#include "uspi.h"

#include "usb.h"

#include "device.h"
#include "mem.h"
#include "timer.h"
#include "irq.h"
#include "debug.h"
#include <stdarg.h>

void *malloc(unsigned nSize) {
	return AllocMem((uint32)nSize);
}

void free(void* pBlock) {
	FreeMem(pBlock);
}

void MsDelay(unsigned nMilliSeconds) {
	Sleep((uint64)nMilliSeconds * 1000);
}

void usDelay(unsigned nMicroSeconds) {
	Sleep((uint64)nMicroSeconds);
}

typedef struct {
	Timer_t timer;
	void *pParam;
	void *pContext;
	TKernelTimerHandler* pHandler;
} UsbTimer_t;

void TimerCallback(void* data) {
	UsbTimer_t* t = (UsbTimer_t*)data;
	t->pHandler((unsigned)t, t->pParam, t->pContext);
}

unsigned StartKernelTimer( unsigned nHzDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext) {

	UsbTimer_t* t = (UsbTimer_t*)AllocMem(sizeof(UsbTimer_t));

	t->timer.frequency = nHzDelay;
	t->timer.timerType = TM_REPEAT;
	t->timer.data = (void*)t;
	t->timer.callBack = TimerCallback;
	t->pParam = pParam;
	t->pContext = pContext;
	t->pHandler = pHandler;

	AddTimer(&(t->timer));

	return (unsigned)t;
}

void CancelKernelTimer(unsigned hTimer) {

	UsbTimer_t* t = (UsbTimer_t*)hTimer;

	RemoveTimer(&(t->timer));

	FreeMem(t);
}

typedef struct {
	IrqHandler_t handler;
	TInterruptHandler *pHandler;
	void *pParam;
} UsbIrq_t;

void UsbIrqCallback(void* data) {
	
	UsbIrq_t* irq = (UsbIrq_t*)data;
	irq->pHandler(irq->pParam);
}

void ConnectInterrupt(unsigned nIRQ, TInterruptHandler *pHandler, void *pParam) {

	UsbIrq_t* irq = (UsbIrq_t*)AllocMem(sizeof(UsbIrq_t));

	irq->handler.num = IRQ_GPU_9;
	irq->handler.data = (void*)irq;
	irq->handler.identifier = NULL;
	irq->handler.handler = UsbIrqCallback;
	irq->pHandler = pHandler;
	irq->pParam = pParam;
	
	RegisterIrqHandler(&(irq->handler));
}

int SetPowerStateOn(unsigned nDeviceId) {
	return 1;
}

int GetMACAddress( unsigned char Buffer[6]) {
	return 0;
}

void LogWrite(const char* pSource, unsigned Severity, const char *pMessage, ...) {

	const char* fmt = NULL;
	switch( Severity ) {
		case LOG_ERROR:
			fmt = "ERROR: %s: ";
			break;
		case LOG_WARNING:
			fmt = "WARNING: %s: ";
			break;
		case LOG_NOTICE:
			fmt = "NOTICE: %s: ";
			break;
		case LOG_DEBUG:
			fmt = "DEBUG: %s: ";
			break;
		default:
			break;
	}
	if( fmt != NULL ) {
		DebugPrintf(fmt, pSource);
		DebugPrintf("  \n");
	}

	va_list va;
	va_start(va, pMessage);
	DebugPrintArg(pMessage, va);
	DebugPrintf("  \n");
}

void uspi_assertion_failed(const char *pExpr, const char *pFile, unsigned nLine) {

	DebugPrintf("USPI Assert Failed: %s, %x: %s\n", pFile, nLine, pExpr);

}

void DebugHexdump(const void *pBuffer, unsigned nBufLen, const char *pSource) {

	DebugPrintf("USPI Dump: %s\n", pSource);

	uint32* p = (uint32*)pBuffer;
	for( uint32 n = 0; n < nBufLen / 4; n++ ) {
		DebugPrintf(" %x %x %x %x\n", p++, p++, p++, p++);
	}
}

extern void _Idle(void);

void UsbDevice(void) {

	if( USPiInitialize() == 0 ) {
		DebugPrintf("USB initialization failed\n");
		return;
	}

	DebugPrintf("USB initialized\n");

	while(TRUE) {
		_Idle();
	}
}

#define USB_DEVICE_NAME "usb.device"
#define USB_DEVICE_STACK 4096

void InitUsb(void) {

	CreateDevice(USB_DEVICE_NAME, USB_DEVICE_STACK, UsbDevice);
}


