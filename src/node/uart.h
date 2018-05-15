//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/uart.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18

#ifndef UART_H
#define	UART_H

void uart_init9600(uint8_t channel);

void uart_sendChar(unsigned char ch, uint8_t channel);

void uart_sendByte(uint8_t byte);

#endif	/* UART_H */

