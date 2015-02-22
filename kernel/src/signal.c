
#include "signal.h"
#include "kernel.h"
#include "uart.h"

extern void _Idle(void);

void Signal(Task_t* task, uint32 signals) {

	task->sigReceived |= (signals & task->sigWait);
	if( task->state == TS_SLEEPING ) {
		task->state = TS_READY;
	}
}

SignalBit_t AllocSignal(SignalBit_t sigBit) {

	Task_t* task = GetKernel()->currentTask;

	return AllocSignalForTask(task, sigBit);
}

SignalBit_t AllocSignalForTask(Task_t* task, SignalBit_t sigBit) {

	if( sigBit > 31 ) {
		return -1;
	}

	uint32 sig = 1;

	if( sigBit < 0 ) {
		while( sig < 31 && task->sigAlloc & (1 << sig) ) {
			sig++;
		}
	}

	if( sig == 32 || task->sigAlloc & (1 << sig) ) {
		return -1;
	}

	task->sigAlloc |= (1 << sig);

	return sig;
}

void DeallocSignal(SignalBit_t sigBit) {

	if( sigBit < 0 || sigBit > 31 ) {
		return;
	}

	Task_t* task = GetKernel()->currentTask;
	task->sigAlloc ^= (1 << sigBit);
}

uint32 GetSignals(void) {

	Task_t* task = GetKernel()->currentTask;
	uint32 sig = task->sigReceived;
	task->sigReceived = 0;

	return sig;
}

void SetSignalMask(uint32 sigMask) {
	
	Task_t* task = GetKernel()->currentTask;
	task->sigWait = sigMask;
}

uint32 GetSignalMask(void) {
	
	Task_t* task = GetKernel()->currentTask;
	return task->sigWait;
}

void Wait(void) {
	
	Task_t* task = GetKernel()->currentTask;
	task->state = TS_SLEEPING;
	while( ! (task->sigWait & task->sigReceived) ) {
		_Idle();
	}
	task->state = TS_READY;
}

