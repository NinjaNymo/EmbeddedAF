//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/main.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18
#include <xc.h>
#include "types.h"
#include "utilities.h"
#include "uart.h"
#include "oled.h"
#include "rtc.h"
#include "temp.h"
#include "ui.h"
#include "nvm.h"
#include "xbee.h"

//-----------------------------------------------//
//                    GLOBALS                    //                     
//-----------------------------------------------//
systemState_t state = {INIT, 0, 0, {0}, {0, 0, 0}, 0, 0, 0, OFF, DISABLED, {0, 0, 0}, NONE};
//-----------------------------------------------//
//               INPUT CHANGE ISR                //                     
//-----------------------------------------------//
void _ISRFAST _CNInterrupt(){
    if(PORTAbits.RA2 == 0){
        state.bDir = UP;
        state.flags.bChange = 1;
    }
    else if(PORTAbits.RA3 == 0){
        state.bDir = LEFT;
        state.flags.bChange = 1;
    }
    else if(PORTBbits.RB4 == 0){
        state.bDir = DOWN;
        state.flags.bChange = 1;
    }
    else if(PORTAbits.RA4 == 0){
        state.bDir = RIGHT;
        state.flags.bChange = 1;
    }
    IFS1bits.CNIF = 0; // Clear the flag
}
//-----------------------------------------------//
//               TIMER 2/3 ISR                   //                     
//-----------------------------------------------//
void _ISRFAST _T3Interrupt(){
    state.flags.timeOut = 1; // Set the time-out flag
    IFS0bits.T3IF       = 0; // Clear the flag
}
//-----------------------------------------------//
//                 U1 RX ISR                     //                     
//-----------------------------------------------//
uint8_t packet[35];
uint8_t newPacket = 0;
uint8_t pIndex = 0;
uint16_t pLength;
void _ISRFAST _U1RXInterrupt(){
    uint8_t rx = U1RXREG;
    // Look for start of new message:
    if((rx == 0x7E) && (pIndex == 0)){
        packet[0] = 0x7E;
        pIndex = 1;
    }
    else if(pIndex != 0){
        packet[pIndex] = rx;
        if(pIndex == 1){
            pLength = rx << 8;
            pIndex ++;
        }
        else if(pIndex == 2){
            pLength += rx;
            pIndex ++;
        }
        // Look for end of message:
        else if(pIndex == pLength - 3){
            pIndex = 0;
            pLength = 0;
            state.flags.rxPacket = 1;
        }   
    }
    IFS0bits.U1RXIF = 0; // Clear the flag.
}
//-----------------------------------------------//
//             PRIVATE FUNCTIONS                 //                     
//-----------------------------------------------//
static void refreshState(){            
    state.time = rtc_getTime();
    util_runLampFSM(&state);
    //temp_startConversion();
    //__delay_ms(10);
    //state.boardTemp = temp_getTemp(probe);
    //state.plantTemp = temp_getTemp(wireProbe);
    //state.waterLevel = util_getWaterLevel();
}

static void initAll(){
    util_initPins();
    util_initADC();
    util_initTimer23();
    uart_init9600(1); // Channel 1
    util_initInterrupts();
    oled_init();
    rtc_init(); 
    nvm_init();
    
}

void test(){
    initAll();
    oled_clear();
    uint8_t foo = 0;
    while(1){
        Sleep();
        if(foo == 1){
            oled_printChar('1');
            foo = 0;
        }
        else{
            oled_printChar('0');
            foo = 1;
        }
    
    }
}
//-----------------------------------------------//
//                    MAIN                       //                     
//-----------------------------------------------//
int main(void){
    while(1){
        switch(state.loopState){
            case INIT:
                initAll();
                // Load cycle and settings from memory:
                uint16_t cycle = nvm_getCycle();
                state.sunset = cycle & 0xFF;
                state.sunrise = (cycle >> 8) & 0xFF;
                //state.settings.raw = nvm_getSettings();
                
                refreshState();
                ui_printBootMenu();
                ui_printHeader();
                ui_printMenu();
                ui_refreshHeader(&state);
                state.loopState = IDLE;
                break;
            case CONNECTING:
                // 1. Try to connect to MQTT
                // 2. Wait 
                break;
            case IDLE:
                if(state.flags.bChange){
                    state.loopState = BROWSE;
                }
                else if(state.flags.rxPacket == 1){
                    state.flags.rxPacket = 0;
                    state.loopState = HANDLE_MSG;
                }
                else if(state.flags.timeOut == 1){
                    state.flags.timeOut = 0;
                    state.loopState = REFRESH;
                }
                else{
                    Sleep(); // Woken on interrupts
                }
                break;
            case REFRESH:
                refreshState();
                ui_refreshHeader(&state);
                if(state.mqttState == CONNECTED){
                    state.loopState = PUB_VALS;
                }
                else{
                    state.loopState = IDLE;
                }
                break;
            case BROWSE:
                if(ui_browse(&state)){
                    state.flags.bChange = 0;
                    ui_printHeader();
                    ui_printMenu();
                    ui_refreshHeader(&state);
                    
                    state.loopState = REFRESH;
                }
                else{
                    state.flags.bChange = 0;
                    state.loopState = IDLE;
                }
                break;
            case PUB_VALS:
                state.loopState = IDLE;
                break;
            case PUB_STATE:
                state.loopState = IDLE;
                break;
            case HANDLE_MSG:
                state.loopState = IDLE;
                break;
            default:
                break;
        }
    }
    return 0;
}