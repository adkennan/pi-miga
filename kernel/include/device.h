
#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "exec.h"

Device_t* CreateDevice(const char* name, uint32 stackSize, TaskStart_t start);

IORequest_t* CreateIORequest(MessagePort_t* port, uint32 size);

void DeleteIORequest(IORequest_t* request);

uint32 OpenDevice(const char *name, uint32 unit, IORequest_t* request);

uint32 CloseDevice(IORequest_t* request);

uint32 DoIO(IORequest_t* request);

void SendIO(IORequest_t* request);

bool CheckIO(IORequest_t* request);

uint32 WaitIO(IORequest_t* request);

uint32 AbortIO(IORequest_t* request);

#endif // __DEVICE_H__

