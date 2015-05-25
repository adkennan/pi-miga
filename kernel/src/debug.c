
#include "debug.h"
#include "type.h"
#include "stdarg.h"
#include "gpu.h"

#include "uart.h"

void DebugPutChar(char c) {

	PutChar(c);
}

void DebugPrintf(const char* fmt, ...) {

	va_list va;
	va_start(va, fmt);

	DebugPrintArg(fmt, va);
}

void DebugPrintArg(const char* fmt, va_list va) 
{
	char ch;
	while(  (ch = *(fmt++)) ) {
		if( ch != '%' ) {
			DebugPutChar(ch);
		} else {
			ch = *(fmt++);
			if( ! ch ) {
				goto abort;	
			}

			switch( ch ) {
				case '%':
					DebugPutChar('%');
					break;
					
				case 's': {
					const char* s = va_arg(va, const char*);
					while( *s ) {
						DebugPutChar(*(s++));
					}
					break;
				}

				case 'u':
				case 'U':
				case 'x':
				case 'X':
					DebugPutChar('0');
					DebugPutChar('x');
					uint32 n = va_arg(va, uint32);
					for( uint32 d = 8; d > 0; d-- ) {
						uint8 v = (uint8)((n >> ((d - 1) * 4)) & 0xF);
						if( v < 10 ) {
							DebugPutChar('0' + v);
						} else {
							DebugPutChar('A' + (v - 10));
						}
					}
					break;

				default:
					DebugPutChar('%');
					DebugPutChar(ch);
					break;
			}
		}
	}	
abort:

	va_end(va);
}
