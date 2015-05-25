
#ifndef __EXEC_TIMER_H__
#define __EXEC_TIMER_H__

typedef void (*TimerCallback_t)(void*);

typedef enum {
	TM_SINGLE,
	TM_REPEAT
} TimerType_t;

typedef struct {
	Node_t n;
	uint32	frequency;
	TimerCallback_t callBack;
	TimerType_t timerType;
	void* data;
	uint64 lastCall;
} Timer_t;

#endif // __EXEC_TIMER_H__

