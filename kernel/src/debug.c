
#include "debug.h"
#include "type.h"
#include "uart.h"
#include "stdarg.h"

void DebugPrintf(const char* fmt, ...) {

	va_list va;
	va_start(va, fmt);
}

void DebugPrintArg(const char* fmt, va_list va) 
{
	char ch;
	while(  (ch = *(fmt++)) ) {
		if( ch != '%' ) {
			uart_putc(ch);
		} else {
			ch = *(fmt++);
			if( ! ch ) {
				goto abort;	
			}

			switch( ch ) {
				case '%':
					uart_putc('%');
					break;
					
				case 's': {
					const char* s = va_arg(va, const char*);
					while( *s ) {
						uart_putc(*(s++));
					}
					break;
				}

				case 'x':
				case 'X':
					uart_putc('0');
					uart_putc('x');
					uint32 n = va_arg(va, uint32);
					for( uint32 d = 8; d > 0; d-- ) {
						uint8 v = (uint8)((n >> ((d - 1) * 4)) & 0xF);
						if( v < 10 ) {
							uart_putc('0' + v);
						} else {
							uart_putc('A' + (v - 10));
						}
					}
					break;

				default:
					uart_putc('%');
					uart_putc(ch);
					break;
			}
		}
	}	
abort:

	va_end(va);
}
