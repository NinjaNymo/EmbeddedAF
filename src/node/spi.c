//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/spi.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 14-02-18

#include <xc.h>
#include "spi.h"

void spi_initMaster(){
    SPI1CON1Lbits.SPIEN     = 0; // Disable SPI
    SPI1CON1Lbits.MSTEN     = 1; // Master enable
    SPI1CON1Lbits.CKE       = 1; // Transmit on falling edge
    SPI1CON1Lbits.CKP       = 0;
    SPI1STATLbits.SPIROV    = 0; // Some bit that has to be cleared
    SPI1CON1Lbits.SPIEN     = 1; // SPI enable
    
}

void spi_transmit(uint8_t data){
    SPI1BUFL = data;
    // Wait until transmit buffer is empty:
    while(SPI1STATLbits.SPITBF);
}
