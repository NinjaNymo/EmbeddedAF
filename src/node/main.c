//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/main.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18
#include <xc.h>
#define FCY 4000000L // Define before including libpic
#include <libpic30.h>
#include "types.h"
#include "utilities.h"
#include "uart.h"
#include "oled.h"
#include "rtc.h"
#include "temp.h"
#include "ui.h"
#include "nvm.h"
#include "mqtt.h"

#include "stdio.h"
//-----------------------------------------------//
//                    DEFINES                    //                     
//-----------------------------------------------//
#define REFRESHITIME 0b0010 // 0b0 = 1s, 0b10 = 10s, 0b100 = 1h
                            // Count with ISR for more...

#define ISR __attribute__((interrupt, auto_psv))
//-----------------------------------------------//
//                    GLOBALS                    //                     
//-----------------------------------------------//
systemState_t state = {INIT, 0, 0, {0}, {0, 0, 0}, 0, 0, 0, OFF, DISABLED, {0, 0, 0, 0}, NONE};
//-----------------------------------------------//
//               INPUT CHANGE ISR                //                     
//-----------------------------------------------//
void ISR _CNInterrupt(){
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
void ISR _T3Interrupt(){
    state.flags.timeOut = 1; // Set the time-out flag
    IFS0bits.T3IF       = 0; // Clear the flag
}
//-----------------------------------------------//
//               RTC ALARM ISR                   //                     
//-----------------------------------------------//

void ISR _RTCCInterrupt(){
    state.flags.refresh = 1; // Set the time-out flag
    IFS3bits.RTCIF      = 0; // Clear the flag
}
//-----------------------------------------------//
//                 U1 RX ISR                     //                     
//-----------------------------------------------//
volatile uint8_t packet[30];
volatile uint8_t newPacket = 0;
volatile uint16_t pIndex = 0;
volatile uint16_t pLength = 0;
void ISR _U1RXInterrupt(){
    IFS0bits.U1RXIF = 0;
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
        else{
            pIndex ++;
            if(pIndex == (pLength + 4)){
                pIndex = 0;
                pLength += 4;
                state.flags.rxPacket = 1;
            }
        }      
    }
}
//-----------------------------------------------//
//             PRIVATE FUNCTIONS                 //                     
//-----------------------------------------------//
static void refreshState(){            
    state.time = rtc_getTime();
    util_runLampFSM(&state);
    temp_startConversion();
    __delay_ms(10);
    state.boardTemp = temp_getTemp(probe);
    state.plantTemp = temp_getTemp(wireProbe);
    state.waterLevel = util_getWaterLevel();
}

static void initAll(){
    util_initPins();
    util_initADC();
    util_initTimer23();
    uart_init9600(1); // Channel 1
    util_initInterrupts();
    oled_init();
    rtc_init(REFRESHITIME); 
    nvm_init();
    
}

void test(){
    initAll();
    oled_printChar('X');
    util_startTimer23();
    while(1);
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
                state.settings.raw = nvm_getSettings();
                if(state.settings.fields.mqttSetting == 1){
                    state.mqttState = ENABLED;
                }
                
                refreshState();
                ui_printBootMenu();
                __delay_ms(2000);
                ui_printHeader();
                ui_printMenu();
                ui_refreshHeader(&state);
                state.loopState = IDLE;
                break;
            case IDLE:
                if(state.mqttState == ENABLED){
                    state.loopState = CONNECTING;
                }
                else if(state.flags.bChange){
                    state.loopState = BROWSE;
                }
                else if(state.flags.rxPacket == 1){
                    state.flags.rxPacket = 0;
                    state.loopState = HANDLE_MSG;
                }
                else if(state.flags.refresh == 1){
                    state.flags.refresh = 0;
                    state.loopState = REFRESH;
                }
                else{
                    Idle(); // Woken on interrupts
                }
                break;
            case CONNECTING:; // Some weird C99 grammar quirk
                uint8_t connected = mqtt_connect(&state, 120, packet);
                if(connected){
                    state.mqttState = CONNECTED;
                    state.loopState = SUBSCRIBE;
                }
                else{
                    state.mqttState = DISCONNECTED;
                    state.loopState = IDLE;
                }
                break;
            case SUBSCRIBE:; // Some weird C99 grammar quirk
                uint8_t subscribed = 0; 
                subscribed  = mqtt_subscribe(&state, NEWSUNRISE, packet);
                subscribed &= mqtt_subscribe(&state,  NEWSUNSET, packet);
                if(subscribed){
                    state.loopState = PUB_STATE;
                }
                else{
                    state.loopState = SUBSCRIBE;
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
            case BROWSE:; // C99 grammar quirk
                uint8_t printMenu = 0;
                switch(ui_browse(&state)){
                    case NOUPDATES:
                        state.loopState = IDLE;
                        printMenu = 1;
                        break;
                    case UPDATETIME:
                        rtc_setTime(state.time.hour, state.time.min);
                        util_runLampFSM(&state);
                        if(state.mqttState == CONNECTED){
                            state.loopState = PUB_STATE;
                        }
                        else{
                            state.loopState = IDLE;
                        }
                        printMenu = 1;
                        break;
                    case UPDATENODEID:
                        nvm_saveSettings(state.settings.raw);
                        if(state.mqttState == DISABLED){
                            state.loopState = IDLE;
                        }
                        else{
                            mqtt_disconnect();
                            state.loopState = CONNECTING;
                        }
                        printMenu = 1;
                        break;
                    case UPDATECYCLE:
                        nvm_saveCycle(state.sunrise, state.sunset);
                        util_runLampFSM(&state);
                        if(state.mqttState == CONNECTED){
                            state.loopState = PUB_STATE;
                        }
                        else{
                            state.loopState = IDLE;
                        }
                        printMenu = 1;
                        break;
                    case UPDATEMQTT:
                        if(state.mqttState == DISABLED){
                            mqtt_disconnect();
                            state.loopState = REFRESH;
                        }
                        else{
                            state.loopState = IDLE;
                        }
                        printMenu = 1;
                        break;
                    case UPDATEINDEX:
                        state.loopState = IDLE;
                    default:
                        state.loopState = IDLE;
                        break;    
                }
                state.flags.bChange = 0;
                if(printMenu){
                    ui_printHeader();
                    ui_printMenu();
                    ui_refreshHeader(&state);
                }
                break;
            case PUB_VALS:
                mqtt_pubTemp(state.boardTemp, state.settings.fields.nodeID, BOARDTEMP);
                mqtt_pubTemp(state.plantTemp, state.settings.fields.nodeID, PLANTTEMP);
                mqtt_pubWaterLevel(state.waterLevel, state.settings.fields.nodeID, WATER);
                state.loopState = IDLE;
                break;
            case PUB_STATE:
                mqtt_pubCycle(state.sunrise, state.settings.fields.nodeID, SUNRISE);
                mqtt_pubCycle(state.sunset, state.settings.fields.nodeID, SUNSET);
                mqtt_pubLampState(state.lampState, state.settings.fields.nodeID, LAMPSTATE);
                state.loopState = REFRESH;
                break;
            case HANDLE_MSG:
                state.flags.rxPacket = 0;

                // Look for messages on NEWSUNRISE or NEWSUNSET topics:
                if(packet[MTYPE_I] == PUBLISH){
                    if(packet[PUB_TOP_I] == NEWSUNRISE){
                        state.sunrise = packet[PUB_DATA_I];
                        nvm_saveCycle(state.sunrise, state.sunset);
                        util_runLampFSM(&state);
                    }
                    else if(packet[PUB_TOP_I] == NEWSUNSET){
                        state.sunset = packet[PUB_DATA_I];
                        nvm_saveCycle(state.sunrise, state.sunset);
                        util_runLampFSM(&state);
                    }
                    state.loopState = PUB_STATE;
                }
                else{
                    state.loopState = IDLE;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}