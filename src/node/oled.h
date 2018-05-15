//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/oled.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 21-02-18

#ifndef OLED_H
#define	OLED_H

#define OLED_RST_PIN     LATBbits.LATB8   // OLED_RST --> Pin RB8
#define OLED_RST_PIN_DIR TRISBbits.TRISB8
#define OLED_DC_PIN      LATBbits.LATB9   // OLED_DC  --> Pin RB9
#define OLED_DC_PIN_DIR  TRISBbits.TRISB9

void oled_init();

void oled_goToPage(uint8_t Page);

void oled_goToCol(uint8_t column);

void oled_goToPos(uint8_t page, uint8_t column);

void oled_printByte(uint8_t byte);

void oled_printSymbol(uint8_t s);

void oled_printChar(char c);

void oled_clearPage(uint8_t row);

void oled_clearRestOfPage();

void oled_clear();

void oled_printSerial(uint8_t byte);

void oled_printSplashLogo();


#endif	/* OLED_H */

