
#ifndef __TIMER_H__
#define __TIMER_H__

#include "type.h"
#include "exec.h"

void InitTimer();

void AddTimer(Timer_t *timer);

void RemoveTimer(Timer_t *timer);

uint64 GetTicks();

void Sleep(uint64 microseconds);

#endif // __TIMER_H__
