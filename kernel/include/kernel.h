
#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "exec.h"

typedef struct {
	uint8* heapBase;
	List_t mem;
	List_t irqHandlers;
	List_t timers;
	List_t msgPorts;
	List_t tasks;
	List_t libraries;
	List_t devices;
	Timer_t schedTimer;
	Task_t* currentTask;
	uint32 nextTaskId;
	uint32 kernelState;
} Kernel_t;

Task_t* CreateTask(const char* name, int8 priority, uint32 stackSize, TaskStart_t start);

Task_t* FindTask(const char* name);

void StartTask(Task_t* task);

void InitKernel();

Kernel_t* GetKernel();

void InitExecLibrary(void);

void EnableSchedule(void);

void DisableSchedule(void);

#endif // __KERNEL_H__
