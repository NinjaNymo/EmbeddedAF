//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/uart.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18

#include <xc.h>
#include "uart.h"




void uart_init9600(uint8_t channel){
    if(channel == 1){
        // Configure UART 1 module:
        U1MODEbits.STSEL    = 0;  // 1 stop bit
        U1MODEbits.PDSEL    = 0;  // 8-bit data, no parity
        U1BRG               = 25; // FCY / (16*baudrate) - 1
        U1MODEbits.UARTEN   = 1;  // Enable UART 1
        U1MODEbits.WAKE     = 1;  // Wake CPU from sleep when RX
        U1MODEbits.USIDL    = 0;  // Continue operation in IDLE mode
        U1STAbits.UTXEN     = 1;  // Enable UART transmitter
        U1STAbits.URXEN     = 1;  // Enable UART receiver
    }
    else if(channel == 2){
        U2MODEbits.STSEL    = 0;
        U2MODEbits.PDSEL    = 0;
        U2BRG               = 25;
        U2MODEbits.UARTEN   = 1;
        U2STAbits.UTXEN     = 1;
        U2STAbits.URXEN     = 1;
    }
}

void uart_sendChar(unsigned char c, uint8_t channel){
    if(channel == 1){
        while(U1STAbits.UTXBF == 1); // Wait for TX buffer to clear
        U1TXREG = c; // Put char in TX buffer
    }
    else if(channel == 2){
        while(U2STAbits.UTXBF == 1);
        U2TXREG = c;
    }
}

void uart_sendByte(uint8_t byte){
    while(U1STAbits.UTXBF == 1); // Wait for TX buffer to clear
    U1TXREG = byte;              // Put byte in buffer 
}