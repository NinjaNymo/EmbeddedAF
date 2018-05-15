//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/temp.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 02-03-18
//
//  Driver for DS18B20

#include <xc.h>
#include <stdio.h>
#include "dow.h"

const uint8_t wireProbe[8] = {0x28, 0x6F, 0x4B, 0x17, 0x07, 0x00, 0x00, 0xA4};
const uint8_t probe[8]     = {0x28, 0x8B, 0xA5, 0x00, 0x04, 0x00, 0x00, 0x73};


void temp_startConversion(){
    dow_reset(); // Reset bus
    dow_writeByte(0xCC); // Skip ROM command
    dow_writeByte(0x44); // Convert T command
    // Wait until conversion completes:
    while(1){
        if(dow_readBit()){
            break;
        }
    }
}


uint16_t temp_getTemp(const uint8_t* address){
    uint8_t tempH, tempL;
    dow_reset();         // Reset bus
    dow_writeByte(0x55); // Match ROM command
    // Write the address:
    for(uint8_t i = 0; i < 8; i ++){
        dow_writeByte(address[i]);
    }
    dow_writeByte(0xBE); // Read scratchpad command
    tempL = dow_readByte();
    tempH = dow_readByte();
    dow_reset();
    // 4 LSBs are decimal values
    // Divide by 16 to get rid of those
    return (tempH * 256 + tempL);
}

void temp_printAddress(){
    dow_reset();
    unsigned char address[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    dow_writeByte(0x33); // Read ROM command
    for(uint8_t i = 0; i < 8; i ++){
        address[i] = dow_readByte();
    }
    printf("Address of device present on DOW bus:");
    for(uint8_t i = 0; i < 8; i ++){
        printf("0x%x ", address[i]);
    }
    printf(". ");
}