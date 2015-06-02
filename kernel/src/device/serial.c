
#include <debug.h>
#include <exec.h>
#include <error.h>
#include <device/serial.h>

#include <device/serialdev.h>

#include <mmio.h>

#define DEVICE_NAME "serial.device"
#define PRIORITY 5
#define STACK_SIZE 4096

#define BUFFER_SIZE 8192

enum {
    // The GPIO registers base address.
    GPIO_BASE = 0x20200000,

    // The offsets for reach register.

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),

    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = 0x20201000,

    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

extern void Put32(uint32, uint32);
extern uint32 Get32(uint32);

static void Delay(uint32 count) {
	__asm__ volatile("1: subs %[count], %[count], #1; bne 1b"
					: : [count]"r"(count));
}

void UartPutChar(uint8 byte) {
    while (1) {
        if (!(Get32(UART0_FR) & (1 << 5))) {
	    	break;
		}
    }
    Put32(UART0_DR, byte);
}

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

		if( req->r.length == -1 ) {
			DebugPrintf("Writing NULL terminated string \"%s\"\n", req->r.data);
			uint8* d = req->r.data;
			while( *d ) {
				UartPutChar(*d);
				d++;
			}
		} else {
			uint8* d = req->r.data;
			for( uint32 ix = 0; ix < req->r.length; ix++ ) {
				UartPutChar(*d);
				d++;
			}
		}

		DebugPrintf("Done\n");
		req->r.r.error = 0;
		IExec->ReplyMessage((Message_t*)req);
	}
}

void EnableUart(void) {

    // Mask all interrupts.
    Put32(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) |
		    (1 << 6) | (1 << 7) | (1 << 8) |
		    (1 << 9) | (1 << 10));

    // Enable UART0, receive & transfer part of UART.
    Put32(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void DisableUart(void) {
    // Disable UART0.
    Put32(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    Put32(GPPUD, 0x00000000);
    Delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    Put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    Delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    Put32(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    Put32(UART0_ICR, 0x7FF);
}

void Serial_Init(Device_t* dev) {

	SerialDevice_t* sd = (SerialDevice_t*)dev;

	sd->writer = IExec->CreateTask(DEVICE_NAME, PRIORITY, STACK_SIZE, Writer);
	sd->writerPort = (MessagePort_t*)sd->writer->msgPorts.head;

	sd->unit.unitNum = 0;
	sd->unit.openCount = 0;

	IExec->StartTask(sd->writer);	

	DisableUart();


}

void Serial_Open(uint32 unitNum, IORequest_t* request) {

	if( unitNum != 0 ) {
		request->error = ERR_UNIT_NOT_FOUND;
		return;
	}
	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	IOSerReq_t* req = (IOSerReq_t*)request;
	request->unit = &sd->unit;
	sd->unit.openCount++;

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; 
    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	uint32 div = 300000000 / (16 * req->baud);
	uint32 fra = (((div - ((div / 100) * 100)) * 64) + 50) / 100;
    Put32(UART0_IBRD, div / 100);
    Put32(UART0_FBRD, fra);
	
    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    Put32(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
	
	EnableUart();
}

void Serial_Close(IORequest_t* request) {

	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	
	if( sd->unit.openCount == 0 ) {
		request->error = ERR_DEVICE_NOT_OPEN;
		return;
	}

	sd->unit.openCount--;

	DisableUart();
}

void Serial_Expunge(Device_t* dev) {

	
}

void Serial_BeginIO(IORequest_t* request) {

	SerialDevice_t* sd = (SerialDevice_t*)request->device;
	IOSerReq_t* req = (IOSerReq_t*)request;

	switch( request->command ) {
		case CMD_WRITE:
			request->flags ^= IOF_QUICK;
			IExec->SendMessage(sd->writerPort, (Message_t*)request);
			break;

		case CMD_READ:
			if( sd->readBuf.start == sd->readBuf.end ) {
				req->r.actual = 0;	
			} else if( sd->readBuf.start < sd->readBuf.end ) {
				uint32 bw = 0;
				while( sd->readBuf.start < sd->readBuf.end && bw <= req->r.length  ) {
					
				}
			}
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
