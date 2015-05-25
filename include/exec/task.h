
#ifndef __EXEC_TASK_H__
#define __EXEC_TASK_H__ 

typedef enum {
	TS_READY,
	TS_RUNNING,
	TS_SLEEPING
} TaskState_t;

typedef void (*TaskStart_t)(void);

typedef uint32* StackPtr_t;

typedef struct Task_t Task_t;

struct Task_t {
	Node_t n;
	uint32 id;
	Task_t* parent;
	List_t msgPorts;
	uint64 totalTime;
	uint64 schedTime;
	uint32 sliceTime;
	uint32 memUsage;
	TaskState_t state;
	TaskStart_t start;
	StackPtr_t stackPtr;
	uint32 sigAlloc;
	uint32 sigWait;
	uint32 sigReceived; 
};

typedef enum {
	SIG_ANY = -1,
	SIG_BREAK = 0
} SignalBit_t;

#endif // __EXEC_TASK_H__

