//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/oled.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 21-02-18

#include <xc.h>
#define FCY 4000000L
#include <libpic30.h>
#include <stdio.h>
#include "oled.h"
#include "spi.h"
#include "font_4x6.h"

uint8_t currentPage, currentColumn;

static void sendComand(char cmd){
    // Set DC pin low:
    OLED_DC_PIN = 0;
    
    // Transmit command:
    spi_transmit(cmd);
}

static void sendData(char data){
    // Set DC pin high:
    OLED_DC_PIN = 1;
    
    // Transmit data:
    spi_transmit(data);
}

void oled_init(){
    // Initialize SPI:
    spi_initMaster();
    // Set pin directions:
    OLED_RST_PIN_DIR = 0;
    OLED_DC_PIN_DIR = 0;
    
    // Run reset sequence:
    OLED_RST_PIN = 1;
    __delay_ms(1);
    OLED_RST_PIN = 0;
    __delay_ms(10);
    OLED_RST_PIN = 1;
    __delay_ms(10);
    
    sendComand(0xAE); // Power display off
    
    sendComand(0xA8); // Set multiplex ratio
    sendComand(0b00111111); // 63 = LCD height - 1
    
    sendComand(0xD3); // Set display offset
    sendComand(0x00); // No offset
    
    sendComand(0x40 | 0x0); // Set display start line to 0
    
    sendComand(0x8D); // Set charge pump regulator
    sendComand(0x14); // Enable regulator
    
    sendComand(0x20); // Set addressing mode
    sendComand(0b00000010); // Page addressing
    
    sendComand(0x00); //Set lower column start address as low as possible.
	
    sendComand(0x10); //Set higher column start address as low as possible.
    
    // A BIT UNSURE ABOUT THIS ONE; TEST IT
    sendComand(0xA1); // Segment remap, SEG0 = COL127
    
    sendComand(0xC8); // Output scan direction COM63 -> COM0
    
    sendComand(0xDA); // Set COM pins
    sendComand(0x12);
    
    sendComand(0x81); // Contrast control
    sendComand(0x50); // Set contrast to 80 (1 to 256)
    
    sendComand(0xD9); // Set pre-charge period
    sendComand(0xF1);
    
    sendComand(0xDB); // Set V_COMH deselect level
    sendComand(0x40);
    
    sendComand(0xA4); // Output on OLED mirrors memory content
    
    sendComand(0xA6); // 1 in memory equals pixel on
    
    sendComand(0x2E); // Deactivate scrolling
    
    sendComand(0xAF); // Turn display ON (AE to turn OFF)
    
    oled_clear();
    oled_goToPos(0,0);
}


void oled_goToPage(uint8_t page){
    sendComand(0xB0 + page);
    currentPage = page;
}

void oled_goToCol(uint8_t column){
    uint8_t upperNibble = column & 0b11110000;
    upperNibble /= 0b10000; // Shift nibble 4 bits to the right
    uint8_t lowerNibble = column & 0b00001111;
    
    sendComand(lowerNibble);
    sendComand(upperNibble + 0b00010000);
    
    
    currentColumn = column;
}

void oled_goToPos(uint8_t page, uint8_t column){
    oled_goToPage(page);
    oled_goToCol(column);
}

void oled_printByte(uint8_t byte){
    sendData(byte);
    currentColumn ++;
}

void oled_printSymbol(uint8_t s){
    for(uint8_t i = 0; i < SYMBOLWIDTH; i++){
        char data = symbol_5x6[s][i];
        
        sendData(data);
    }
    
    if((currentColumn + SYMBOLWIDTH) > 127){
        if(currentPage == 7){
            oled_clear();
            oled_goToPos(0,0);
        }
        else{
            oled_goToPos(currentPage + 1, 0);
        }
    }
    else{
        currentColumn += SYMBOLWIDTH;
    }
}

void oled_printChar(char c){
    for(uint8_t i = 0; i < CHARWIDTH; i++){
        char data = font_4x6[c - 32][i];
        
        sendData(data);
    }
    
    if((currentColumn + CHARWIDTH) > 127){
        if(currentPage == 7){
            oled_clear();
            oled_goToPos(0,0);
        }
        else{
            oled_goToPos(currentPage + 1, 0);
        }
    }
    else{
        currentColumn += CHARWIDTH;
    }
}

void oled_clearPage(uint8_t page){
    oled_goToPage(page);
    
    for(uint8_t i = 0; i < 128; i++){
        sendData(0x00);
    }
}

void oled_clearRestOfPage(){
    for(uint8_t i = currentColumn; i < 128; i++){
        sendData(0x00);
    }
}

void oled_clear(){
    for(uint8_t i = 0; i < 8; i++){
        oled_clearPage(i);
    }
}


void oled_printSerial(uint8_t byte){
    char buffer[2];
    //If value is single digit:
    if(byte < 16){
        sprintf(buffer, "0%x", byte);
    }
    else{
        sprintf(buffer, "%x", byte);
    }
    
    // Print the values to OLED:
    for(uint8_t i = 0; i < 2; i ++){
        oled_printChar(buffer[i]);

    }
    if((currentColumn + (2 * CHARWIDTH)) <= 127){
        if(currentColumn != 0){
            oled_printChar(' ');
        }
    }
}

void oled_printSplashLogo(){
    oled_goToPos(2,35);
    uint8_t c = 0;
    uint8_t p = 2;
    for(uint16_t i = 0; i < SPLASHSIZE - 1; i++){
        oled_printByte(splashLogo[i]);
        if(c < SPLASHWIDTH - 1){
            c++;
        }
        else{
            c = 0;
            p++;
            oled_goToPos(p,35);
            
        }
    }
}