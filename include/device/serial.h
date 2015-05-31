
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <exec.h>

#define DEFAULT_BAUD 115200

typedef struct {
	IOStdReq_t r;
	uint32 baud;
} IOSerReq_t;

#endif // __SERIAL_H__
