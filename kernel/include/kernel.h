
#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "exec.h"

Task_t* CreateTask(const char* name, int8 priority, uint32 stackSize, TaskStart_t start);

Task_t* FindTask(const char* name);

void StartTask(Task_t* task);

void InitKernel();

Kernel_t* GetKernel();

void InitExecLibrary(void);

#endif // __KERNEL_H__
