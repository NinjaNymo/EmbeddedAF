//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/nvm.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 01-05-18

#include <xc.h>
#include "nvm.h"
#include "DEE Emulation 16-bit.h"


void nvm_init(){
    DataEEInit();
    dataEEFlags.val = 0;
}


uint16_t nvm_getCycle(){
    uint16_t cycle;
    
    cycle = DataEERead(CYCLEADDR);
    Nop();
    Nop();
    
    // Avoid bugs when memory is not initialized.
    if((cycle >> 8) > 23){
        cycle = 0;
    }
    else if((cycle & 0x00FF) > 23){
        cycle = 0;
    }
    
    return cycle;
}


void nvm_saveCycle(uint8_t sunrise, uint8_t sunset){
    uint16_t cycle = (sunrise << 8) + sunset;
    DataEEWrite(cycle, CYCLEADDR);
    Nop();
    Nop();
}


uint16_t nvm_getSettings(){
    uint16_t settings;
    settings  = DataEERead(SETTINGSADDR);
    return settings;
}


void nvm_saveSettings(uint16_t settings){
    DataEEWrite(settings, SETTINGSADDR);
    Nop();
    Nop();
}


