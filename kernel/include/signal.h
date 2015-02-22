
#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include "type.h"
#include "exec.h"

SignalBit_t AllocSignal(SignalBit_t sigBit);

SignalBit_t AllocSignalForTask(Task_t* task, SignalBit_t sigBit);

void DeallocSignal(SignalBit_t sigBit);

void Signal(Task_t* task, uint32 signals);

uint32 GetSignals(void);

void SetSignalMask(uint32 sigMask);

uint32 GetSignalMask(void);

void Wait(void);

#endif // __SIGNAL_H__
