//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/dow.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 28-02-18
//
// Timings are as specified in 
// Maxim Integrated - APPLICATION NOTE 126:
// 1-Wire Communication Through Software

#include <xc.h>
#define FCY 4000000L
#include <libpic30.h>
#include "dow.h"

static void pullLow(){
    DOW_PIN_DIR = 0;
    DOW_PIN = 0;
}

static void release(){
    DOW_PIN_DIR = 1;
}

uint8_t dow_reset(){
    uint8_t devicesPresent = 0;
    // Reset sequence:
    pullLow();
    __delay_us(480);
    release();
    // Presence detection:
    __delay_us(70);
    if(DOW_PIN == 0){
        devicesPresent = 1;
    }
    __delay_us(410);
    return devicesPresent;  
}

void dow_writeBit(uint8_t b){
    b = b & 0x01;
    
    if(b){
        // Write '1' bit sequence:
        pullLow();
        __delay_us(6);
        release();
        __delay_us(64);
    }
    else{
        // Write '0' bit sequence:
        pullLow();
        __delay_us(70); // Was 90
        release();
        __delay_us(10); // Was 10
    }
}

uint8_t dow_readBit(){
    int res;
    // Read bit sequence:
    pullLow();
    __delay_us(6);
    release();
    __delay_us(9);
    res = DOW_PIN;
    __delay_us(55);
    return res;
}

uint8_t dow_readByte(){
    // Reads LSB first:
    uint8_t byte = 0x00;
    for(int i = 0; i < 8; i++){
        byte >>= 1;
        if(dow_readBit()){
            byte |= 0x80;
        }
    }
    return byte;
}

void dow_writeByte(uint8_t byte){
    // Writes LSB first:
    for(uint8_t i = 0; i < 8; i++){
        dow_writeBit(byte & 0x01);
        byte >>= 1;
    }
}