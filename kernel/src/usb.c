/*
#include "uspios.h"
#include "uspi.h"

#include "usb.h"

#include "kernel.h"
#include "device.h"
#include "mem.h"
#include "timer.h"
#include "irq.h"
#include "debug.h"
#include <stdarg.h>
#include "gpu.h"

void *malloc(unsigned nSize) {
	return AllocMem((uint32)nSize);
}

void free(void* pBlock) {
	FreeMem(pBlock);
}

void MsDelay(unsigned nMilliSeconds) {
//	DebugPrintf("USB: MsDelay(%x)...", (uint32)nMilliSeconds);
	Sleep((uint64)nMilliSeconds * 1000);
//	DebugPrintf("Done.\n");
}

void usDelay(unsigned nMicroSeconds) {
//	DebugPrintf("USB: usDelay(%x)...", (uint32)nMicroSeconds);
	Sleep((uint64)nMicroSeconds);
//	DebugPrintf("Done.\n");
}

typedef struct {
	Timer_t timer;
	void *pParam;
	void *pContext;
	TKernelTimerHandler* pHandler;
} UsbTimer_t;

void TimerCallback(void* data) {
	UsbTimer_t* t = (UsbTimer_t*)data;
	DebugPrintf("USB: Timer Callback %x\n", data);
	t->pHandler((unsigned)t, t->pParam, t->pContext);
}

unsigned StartKernelTimer( unsigned nHzDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext) {

	DebugPrintf("USB: Starting timer\n");

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

	DebugPrintf("USB: Stopping timer\n");

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

//	DebugPrintf("USB: IRQ Callback!\n");

	UsbIrq_t* irq = (UsbIrq_t*)data;
	irq->pHandler(irq->pParam);
}

bool UsbIrqIdentify(void* data) {
	
	return TRUE;
}

void ConnectInterrupt(unsigned nIRQ, TInterruptHandler *pHandler, void *pParam) {

	DebugPrintf("USB: Connect interrupt %x\n", nIRQ);

	UsbIrq_t* irq = (UsbIrq_t*)AllocMem(sizeof(UsbIrq_t));

	irq->handler.num = nIRQ;//IRQ_1_USB;
	irq->handler.data = (void*)irq;
	irq->handler.identifier = UsbIrqIdentify;
	irq->handler.handler = UsbIrqCallback;
	irq->pHandler = pHandler;
	irq->pParam = pParam;
	
	RegisterIrqHandler(&(irq->handler));
}

int SetPowerStateOn(unsigned nDeviceId) {

	DebugPrintf("USB: Set Power State On %x\n", nDeviceId);

	SetPowerState(nDeviceId, TRUE);

	return 1;
}

int GetMACAddress( unsigned char Buffer[6]) {
	return 0;
}

void LogWrite(const char* pSource, unsigned Severity, const char *pMessage, ...) {

	if( pMessage == NULL ){
		return;
	}

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
	}

	va_list va;
	va_start(va, pMessage);
	DebugPrintArg(pMessage, va);
	DebugPrintf("\n");
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

void KeyPressedHandler(const char* str) {
	DebugPrintf(str);
}

void UsbDevice(void) {

	DisableSchedule();
//	DisableInterrupts();

	if( USPiInitialize() == 0 ) {
		DebugPrintf("USB: Initialization failed\n");

	} else {

		DebugPrintf("USB: Initialized\n");
	}

//	EnableInterrupts();
	EnableSchedule();

	if( USPiKeyboardAvailable () ) {

		DebugPrintf("USB: Keyboard Registered.\n");
		USPiKeyboardRegisterKeyPressedHandler (KeyPressedHandler);
	} else {
		DebugPrintf("USB: No keyboard found.\n");
	}

	while(TRUE) {
		_Idle();
	}
}

#define USB_DEVICE_NAME "usb.device"
#define USB_DEVICE_STACK 0x8000

void InitUsb(void) {

	DebugPrintf("Starting USB Device\n");

	CreateDevice(USB_DEVICE_NAME, USB_DEVICE_STACK, UsbDevice);
}

*/

