/* uart.h - UART initialization & communication */

#ifndef UART_H
#define UART_H

#include <type.h>

/*
 * Initialize UART0.
 */
void uart_init();

/*
 * Transmit a byte via UART0.
 * uint8_t Byte: byte to send.
 */
void uart_putc(uint8_t byte);

/*
 * Receive a byte via UART0.
 *
 * Returns:
 * uint8_t: byte received.
 */
uint8_t uart_getc();

void uart_clear_int();

#endif // #ifndef UART_H

