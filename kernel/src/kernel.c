
#include "kernel.h"
#include "list.h"
#include "timer.h"
#include "irq.h"
#include "mem.h"
#include "signal.h"
#include "msgport.h"
#include "gpu.h"
#include "console.h"
#include "usb.h"

#define EXEC_IFACE_LOC 0x7FFC

#define SCHED_FREQ 100000
#define CONTEXT_WORDS 19

extern void _CtxSwitch(StackPtr_t oldStack, StackPtr_t newStack);
extern StackPtr_t _GetStack(void);
extern uint32	_GetCpsr(void);
extern void _SetStack(StackPtr_t sp);
extern void _Idle(void);

static Kernel_t _kernel;

Task_t* CreateTask(const char* name, int8 priority, uint32 stackSize, TaskStart_t start)
{
	uint32 memSize = stackSize + sizeof(Task_t);

	void* mem = AllocMem(stackSize + sizeof(Task_t)); 

	if( _kernel.currentTask != NULL ) {
		_kernel.currentTask->memUsage -= memSize;
	}

	Task_t* task = (Task_t*)mem;
	InitNode((Node_t*)&task->n, priority, name);
	task->id = _kernel.nextTaskId++;
	task->parent = _kernel.currentTask;
	task->state = TS_READY;
	task->start = start;
	task->sigAlloc = 1;
	task->sigWait = 1;
	task->sigReceived = 0;
	task->memUsage = memSize;

	InitList(&task->msgPorts, NT_MSGPORT, LF_NONE);

	Insert(&task->msgPorts, (Node_t*)CreatePortForTask(task, NULL), NULL);

	StackPtr_t sp = (StackPtr_t)((uint32)mem + stackSize + sizeof(Task_t));
	
	// Make sure the stack is 8 byte aligned
	if( (uint32)sp & 4) {
		--sp;
	}	

	// Make room on the stack for all the registers.
	sp -= CONTEXT_WORDS; 

	// 0 out the entries for all registers.
	for( uint32 i = 0; i <= CONTEXT_WORDS; i++ ) {
		sp[i] = 0;
	}

	sp[CONTEXT_WORDS - 2] = 0x1F | 0x40;

	sp[CONTEXT_WORDS - 3] = (uint32)start;
	
	task->stackPtr = sp;

	return task;
}

Kernel_t* GetKernel() {
	return &_kernel;
}

void StartTask(Task_t* task) {
	Insert(&(_kernel.tasks), (Node_t*)task, NULL);
}

Task_t* FindTask(const char* name) {

	if( name == NULL ) {
		return _kernel.currentTask;
	}	
	return (Task_t*)FindNode(&(_kernel.tasks), name);
}

void Idler() {
	while(TRUE) {
		_Idle();
	}
}

void StoreStack(StackPtr_t sp) {
	Task_t* currTask = _kernel.currentTask;
	if( currTask != NULL ) {
		currTask->stackPtr = sp;
	}
}

StackPtr_t RestoreStack(void) {
	Task_t* currTask = _kernel.currentTask;
	StackPtr_t sp = NULL;
	if( currTask != NULL ) {
		sp = currTask->stackPtr;
	}
	return sp;
}

void Schedule() {

	DisableInterrupts();

	Task_t* t = (Task_t*)_kernel.tasks.head;
	while( t != NULL ) {

		if( t->state == TS_READY  ) {

			uint64 ticks = GetTicks();

			Task_t *currTask = _kernel.currentTask;			
			if( currTask != NULL ) {

				RemoveNode(&(_kernel.tasks), (Node_t*)currTask);
				Insert(&(_kernel.tasks), (Node_t*)currTask, NULL);

				if( currTask->state == TS_RUNNING ) {
					currTask->state = TS_READY;
					currTask->totalTime += ticks - currTask->schedTime;
				}
			}

			t->schedTime = ticks;
			t->state = TS_RUNNING;
	
			_kernel.currentTask = t;

			break;
		}
	
		t = (Task_t*)t->n.next;
	}
	EnableInterrupts();
}

void InitKernel(uint8* heapBase) {

	SetScreenMode(800, 600);
	Fill(0, 0, 800, 600, 0x1f, 0x50, 0xa7);

	_kernel.heapBase = heapBase;
	InitList(&(_kernel.mem), NT_MEMORY, LF_NONE);
	InitList(&(_kernel.msgPorts), NT_MSGPORT, LF_NONE);
	InitList(&(_kernel.tasks), NT_TASK, LF_SORTED);
	InitList(&(_kernel.timers), NT_TIMER, LF_NONE);
	InitList(&(_kernel.libraries), NT_LIBRARY, LF_NONE);
	InitList(&(_kernel.devices), NT_DEVICE, LF_NONE);

	InitInterrupts();

	InitTimer();

	_kernel.schedTimer.frequency = SCHED_FREQ;
	_kernel.schedTimer.timerType = TM_REPEAT;
	_kernel.schedTimer.data = NULL;
	_kernel.schedTimer.callBack = Schedule;

	_kernel.currentTask = NULL;

	InitExecLibrary();

	Task_t *idleTask = CreateTask("Idle", 127, 1024, Idler);
	StartTask(idleTask);

	StartConsole();

	AddTimer(&(_kernel.schedTimer));

	InitUsb();
}

