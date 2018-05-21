//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/xbee.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 22-04-18

#include <xc.h>
#define FCY 4000000L // Define before including libpic
#include <libpic30.h>
#include "uart.h"
#include "xbee.h"



void xbee_transmitRequest(uint8_t* f, uint8_t fLen){
    // Calculating checksum:
    //
    // 1. Add all bytes except start delimiter and length.
    // 2. Keep only he lowest 8 bits.
    // 3. Subtract the lowest 8 bits from 0xFF
    uint16_t cs_temp = 0x10 + 0xFF + 0xFE;
    
    __builtin_disi(0x3FFF); /* disable interrupts */
    
    // API header:
    //
    uart_sendByte(0x7E);      //  1 - Start delimiter
    uart_sendByte(0x00);      //  2 - Length of frame data (MSB)
    uart_sendByte(14 + fLen); //  3 - Length of frame data (LSB)
    // Frame data:
    //
    uart_sendByte(0x10);      //  4 - Frame type = transmit request
    uart_sendByte(0x00);      //  5 - Frame ID = 0, no response expected
    // 64 bit coordinator address:
    for(uint8_t i = 0; i < 8; i++){
        uart_sendByte(0x00);
    }
    uart_sendByte(0xFF);      // 14 - 16 bit dest address (MSB)
    uart_sendByte(0xFE);      // 15 - 16 bit dest address (LSB)
    uart_sendByte(0x00);      // 16 - Number of hops = max
    uart_sendByte(0x00);      // 17 - Options = 0, default
    // RF data:
    //
    for(uint8_t j = 0; j < fLen; j ++){
        cs_temp += f[j];
        uart_sendByte(f[j]);
    }
    // Checksum:
    //
    uint8_t cs = 0xFF - (cs_temp & 0xFF);
    uart_sendByte(cs);
    __delay_ms(50); // Don't know why but this appears to be necessary
    __builtin_disi(0x0000); /* enable interrupts */
}
