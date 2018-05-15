//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/dow.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 28-02-18

#ifndef DOW_H
#define	DOW_H

#define DOW_PIN          PORTBbits.RB14   // DOW --> Pin RB14
#define DOW_PIN_DIR      TRISBbits.TRISB14

uint8_t dow_reset();
void dow_writeBit(uint8_t b);
uint8_t dow_readBit();
uint8_t dow_readByte();
void dow_writeByte(uint8_t byte);

#endif	/* DOW_H */
