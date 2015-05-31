
#include "exec.h"
#include "device.h"
#include "mem.h"
#include "kernel.h"
#include "error.h"
#include "msgport.h"
#include "list.h"
#include "signal.h"

Device_t* CreateDevice(const char* name, uint32 size, DeviceInterface_t* iface)
{
	Device_t* dev = (Device_t*)AllocMem(size);
	InitNode((Node_t*)dev, 0, name);

	dev->iface = (struct DeviceInterface_t*)iface;

	return dev;
}

void AddDevice(Device_t* device) 
{
	device->iface->Init(device);

	Kernel_t* k = GetKernel();
	Insert(&(k->devices), (Node_t*)device, NULL);
}

IORequest_t* CreateIORequest(MessagePort_t* port, uint32 size) {
	
	IORequest_t* r = (IORequest_t*)AllocMem(size);
	r->m.replyPort = port;
	r->m.length = size;

	return r;
}

void DeleteIORequest(IORequest_t* request) {

	FreeMem((void*)request);
}

uint32 OpenDevice(const char *name, uint32 unit, IORequest_t* request) {

	Kernel_t* k = GetKernel();

	Device_t* dev = (Device_t*)FindNode(&(k->devices), name);
	if( dev == NULL ) {
		request->error = ERR_DEVICE_NOT_FOUND;
		return request->error;
	}

	request->device = dev;

	dev->iface->Open(unit, request);

	return request->error;
}

uint32 CloseDevice(IORequest_t* request) {

	request->device->iface->Close(request);

	return request->error;
}

uint32 DoIO(IORequest_t* request) {

	request->flags |= IOF_QUICK;

	request->device->iface->BeginIO(request);
	if( (request->flags & IOF_QUICK) == 0 ) {
		return request->error;
	}
		
	return WaitIO(request);
}

void SendIO(IORequest_t* request) {

	request->flags ^= IOF_QUICK;
	request->device->iface->BeginIO(request);
}

bool CheckIO(IORequest_t* request) {

	if( (request->flags & IOF_QUICK) == IOF_QUICK ) {
		return TRUE;
	}

	SignalBit_t sigs = GetSignals();
	if( sigs & request->m.destPort->signal ) {
		return TRUE;
	}
	return FALSE;
}

uint32 WaitIO(IORequest_t* request) {

	if( (request->flags & IOF_QUICK) == 0 ){

		WaitMessage(request->m.replyPort);
	}

	return request->error;
}

uint32 AbortIO(IORequest_t* request) {

	request->device->iface->AbortIO(request);

	return request->error;
}
