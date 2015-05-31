
#include <exec.h>
#include <error.h>
#include <device/serial.h>

#include <device/serialdev.h>

#define DEVICE_NAME "serial.device"
#define PRIORITY 5
#define STACK_SIZE 4096

#define BUFFER_SIZE 8192

typedef struct {
	Device_t dev;
	Unit_t unit;
	Task_t* writer;
	MessagePort_t* writerPort;
	struct {
		uint32 start;
		uint32 end;
		uint8 buffer[BUFFER_SIZE];
	} readBuf;
} SerialDevice_t;

#define TO_BUFFER(dev) ((ReadBuffer_t*)(dev + sizeof(Device_t)))
#define TO_UNIT(dev) ((Unit_t*)(dev + sizeof(Device_t) + sizeof(ReadBuffer_t)))

void Writer(void) {

	Task_t* task = IExec->FindTask(NULL);
	MessagePort_t* port = (MessagePort_t*)task->msgPorts.head;
	while(TRUE) {
		IOSerReq_t* req = (IOSerReq_t*)IExec->WaitMessage(port);
		if( req == NULL ) {
			continue;
		}

		if( (req->r.r.flags & IOF_ABORT) ) {
			req->r.r.error = ERR_IO_ABORTED;
			continue;
		}

		// TODO: Write bytes

		req->r.r.error = 0;
		IExec->ReplyMessage((Message_t*)req);
	}
}

void Serial_Init(Device_t* dev) {

	SerialDevice_t* sd = (SerialDevice_t*)dev;

	sd->writer = IExec->CreateTask(DEVICE_NAME, PRIORITY, STACK_SIZE, Writer);
	sd->writerPort = (MessagePort_t*)sd->writer->msgPorts.head;

	sd->unit.unitNum = 0;
	sd->unit.openCount = 0;

	IExec->StartTask(sd->writer);	
}

void Serial_Open(uint32 unitNum, IORequest_t* request) {

	if( unitNum != 0 ) {
		request->error = ERR_UNIT_NOT_FOUND;
		return;
	}
	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	request->unit = &sd->unit;
	sd->unit.openCount++;
}

void Serial_Close(IORequest_t* request) {
	
	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	
	if( sd->unit.openCount == 0 ) {
		request->error = ERR_DEVICE_NOT_OPEN;
		return;
	}

	sd->unit.openCount--;
}

void Serial_Expunge(Device_t* dev) {

	
}

void Serial_BeginIO(IORequest_t* request) {

	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	
	switch( request->command ) {
		case CMD_WRITE:
			request->flags ^= IOF_QUICK;
			IExec->SendMessage(sd->writerPort, (Message_t*)request);
			break;

		default:
			request->flags ^= IOF_QUICK;
			request->error = ERR_CMD_NOT_SUPPORTED;
			break;
	}	

}

void Serial_AbortIO(IORequest_t* request) {

}

void CreateSerialDevice() {

	DeviceInterface_t* iface = (DeviceInterface_t*)IExec->AllocMem(sizeof(DeviceInterface_t));
	iface->Init = Serial_Init;
	iface->Open = Serial_Open;
	iface->Close = Serial_Close;
	iface->Expunge = Serial_Expunge;
	iface->BeginIO = Serial_BeginIO;
	iface->AbortIO = Serial_AbortIO;

	SerialDevice_t* dev = (SerialDevice_t*)IExec->CreateDevice(DEVICE_NAME, sizeof(SerialDevice_t), iface);
	dev->dev.type = DEV_STREAM;
	dev->writer = NULL;
	dev->writerPort = NULL;
	dev->readBuf.start = 0;
	dev->readBuf.end = 0;

	IExec->AddDevice((Device_t*)dev);
}
