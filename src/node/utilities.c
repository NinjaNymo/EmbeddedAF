//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/utilities.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18

#include <xc.h>
#include <stdio.h>
#include "utilities.h"

uint16_t maxADC = 2740;
uint16_t minADC = 2250;

void util_initPins(){
    // Disable ADCs:
    ANSA = 0x0000;
    ANSB = 0x0000;
    
    // Pushbuttons:
    //
    // Data direction:
    TRISAbits.TRISA2  = 1; // RA2 = input
    TRISAbits.TRISA3  = 1; // RA3 = input
    TRISAbits.TRISA4  = 1; // RA4 = input
    TRISBbits.TRISB4  = 1; // RB3 = input
    // Pull-ups:
    CNPU2bits.CN30PUE = 1; // Pull-up enable on RA2/CN30
    CNPU2bits.CN29PUE = 1; // Pull-up enable on RA3/CN29
    CNPU1bits.CN0PUE  = 1; // Pull-up enable on RA4/CN0
    CNPU1bits.CN1PUE  = 1; // Pull-up enable on RB4/CN1
    // Input change interrupt:
    CNEN1bits.CN0IE   = 1; // RA4/CN0 triggers CN interrupt
    CNEN1bits.CN1IE   = 1; // RB4/CN1 triggers CN interrupt
    CNEN2bits.CN29IE  = 1; // RA3/CN29 triggers CN interrupt
    CNEN2bits.CN30IE  = 1; // RA2/CN30 triggers CN interrupt
    
    // Lamp control:
    //
    TRISBbits.TRISB13 = 0; // RB13 = output
    
    // Peripheral pin select (PPS):
    //
    __builtin_write_OSCCONL(OSCCON & 0xDF); // to clear IOLOCK
    // Outputs:
    RPOR3bits.RP6R    =  7; // SPI1 DO    --> Pin RB6/RP6
    RPOR3bits.RP7R    =  8; // SPI1 CLK   --> Pin RB7/RP7
    RPOR5bits.RP10R   =  3; // UART 1 TX  --> Pin RB10/RP10
    // Inputs:
    RPINR18bits.U1RXR = 11; // UART 1 RX  --> Pin RB11/RP11
    __builtin_write_OSCCONL(OSCCON | 0x40); // to set IOLOCK  
}

void util_initADC(){
    // NOTE: All ADC register are 0x0000 as default.
    
    // Pin configuration:
    //
    TRISBbits.TRISB12  = 1; // RB12 = input
    ANSBbits.ANSB12    = 1; // RB12 = analog
    
    // ADC configuration:
    //
    AD1CON1bits.MODE12 =          1; // 12-bit A/D operation
    AD1CON1bits.SSRC   =     0b0111; // Auto convert after sampling is done
    AD1CON1bits.FORM   =       0b00; // Output = absolute decimal, unsigned
    AD1CON3bits.ADCS   = 0b00000011; // T_AD = 6 * T_CY
    AD1CON3bits.SAMC   =    0b11111; // Sample time =  31 * T_AD
    AD1CHSbits.CH0SA   =    0b01000; // Sample AN8 on pin RB12
    
    AD1CON1bits.ADON   =          1; // Enable module
}

static uint16_t util_readADC(){
    AD1CON1bits.SAMP = 1;     // Start sampling
    while(!AD1CON1bits.DONE); // Wait until conversion is done
    AD1CON1bits.SAMP = 0;     // Stop sampling
    return ADC1BUF0;          // Return value
}

uint8_t util_getWaterLevel(){
    uint8_t val = ((util_readADC() - minADC) * 100) / (maxADC - minADC);
    if (val > 100){
        val = 100;
    }
    return 100 - val;
}

void util_runLampFSM(systemState_t* state){
    if(state->sunrise == state->sunset){
        state->lampState = OFF;
        return;
    }
    
    // Cus it makes it way more readable:
    uint8_t sr = state->sunrise;
    uint8_t SS = state->sunset;
    uint8_t HH = state->time.hour;
    
    // if(((SS <= HH) && (HH < sr)) || ((sr < HH) && SS <= HH) ){

    switch(state->lampState){
        case ON:
            PORTBbits.RB13 = 1;
            if((SS <= HH) || (HH < sr)){
                PORTBbits.RB13 = 0;
                state->lampState = OFF;
            }    
            break;
        case OFF:
            PORTBbits.RB13 = 0;
            if((sr <= HH) && (HH < SS)){
                PORTBbits.RB13 = 1;
                state->lampState = ON;
            }
            else if(SS < sr){
                if(((HH < SS) && (HH < sr)) || ((SS < HH) && (sr < HH))){
                    PORTBbits.RB13 = 1;
                    state->lampState = ON;
                }
            }
            break;
        case SUS_ON:
            PORTBbits.RB13 = 1;
            break;
        case SUS_OFF:
            PORTBbits.RB13 = 0;
            break;
        default:
            break;
    }
}

void util_initInterrupts(){
    INTCON1bits.NSTDIS = 1; // Disable nested interrupts
    
    // UART 1 RX:
    IFS0bits.U1RXIF = 0; // Clear UART 1 RX interrupt flag
    IEC0bits.U1RXIE = 1; // Enable UART 1 RX interrupt
    
    // Timer 2/3:
    IFS0bits.T3IF   = 0; // Clear Timer 3 interrupt flag
    IEC0bits.T3IE   = 1; // Enable Timer 3 interrupt
    
    // RTC alarm:
    IFS3bits.RTCIF  = 0; // Clear RTC interrupt flag
    IEC3bits.RTCIE  = 1; // Enable RTC 
    
    // Input change (CN):
    IFS1bits.CNIF   = 0; // Clear CN interrupt flag
    IEC1bits.CNIE   = 1; // Enable CN interrupt
}

void util_initTimer23(){
    T2CONbits.T32   =    1; // Set timer to 32-bit
    T2CONbits.TCKPS = 0b11; // 256 clock pre scale
    
    // For 10 seconds, trigger interrupt at 156250 = 0x0002625A
    // For 30 seconds, trigger interrupt at 468750 = 0x0007270E
    // For 60 seconds, trigger interrupt at 937500 = 0x000E4E1C

    PR3 = 0x0007; // Most significant word of compare value
    PR2 = 0x270E; // Least significant word of compare value
}

void util_startTimer23(){
    T2CONbits.TON = 1;
}

void util_stopTimer23(){
    T2CONbits.TON = 0;
    // Reset timer
    TMR2 = 0;
    TMR3 = 0;
}


