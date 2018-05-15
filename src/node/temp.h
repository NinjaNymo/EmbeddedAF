//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/temp.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 02-03-18

#ifndef TEMP_H
#define	TEMP_H

#define ONBOARD  {0x28, 0x85, 0x0F, 0x51, 0x04, 0x00, 0x00, 0xB9}
#define EXTERNAL {0x28, 0x6F, 0x4B, 0x17, 0x07, 0x00, 0x00, 0xA4}

extern const uint8_t wireProbe[8];
extern const uint8_t probe[8];

void temp_startConversion();
uint16_t temp_getTemp(const uint8_t* address);
void temp_printAddress();

#endif	/* TEMP_H */

