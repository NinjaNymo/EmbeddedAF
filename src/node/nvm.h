//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/nvm.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 01-05-18

#ifndef NVSTORAGE_H
#define	NVSTORAGE_H

#define CYCLEADDR    16
#define SETTINGSADDR 32

void nvm_init();
uint16_t nvm_getCycle();
void nvm_saveCycle(uint8_t sunrise, uint8_t sunset);
uint16_t nvm_getSettings();
void nvm_saveSettings(uint16_t settings);


#endif	/* NVSTORAGE_H */

