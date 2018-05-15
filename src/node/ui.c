//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/ui.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 30-04-18

#include <xc.h>
#include <stdio.h>
#include "utilities.h"
#include "ui.h"
#include "oled.h"
#include "rtc.h"
#include "nvm.h"

uint8_t menuIndex = 0;

static void ui_printTemps(uint8_t TL, uint8_t TP){
    char buffer[3];
    
    // Print TL:
    sprintf(buffer, "%3d", TL);
    oled_goToPos(0,35);
    for(uint8_t i = 0; i < 3; i++){
        oled_printChar(buffer[i]);
    }
    sprintf(buffer, "%3d",TP);
    oled_goToPos(1,35);
    for(uint8_t i = 0; i < 3; i++){
        oled_printChar(buffer[i]);
    }
}

static void ui_printTime(uint8_t hh, uint8_t mm){
    char buffer[5];
    sprintf(buffer, "%02d:%02d", hh, mm);
    oled_goToPos(0,0);
    for(uint8_t i = 0; i < 5; i++){
        oled_printChar(buffer[i]);
    }
}

static void ui_printCycle(uint8_t sunrise, uint8_t sunset){
    char buffer[2];
    sprintf(buffer, "%02d", sunrise);
    oled_goToPos(1,0);
    oled_printChar(buffer[0]);
    oled_printChar(buffer[1]);
    sprintf(buffer, "%02d", sunset);
    oled_goToPos(1,12);
    oled_printChar(buffer[0]);
    oled_printChar(buffer[1]);
}

static void ui_printMqttState(mqttState_t ms){
    char* state;
    switch(ms){
        case CONNECTED:
            state = "CONNECTED";
            break;
        case DISCONNECTED:
            state = "DISCONN. ";
            break;
        case DISABLED:
            state = "DISABLED ";
            break;
        default:
            state = "P-ERROR  ";
            break;      
    }
    oled_goToPos(1, 91);
    for(uint8_t i = 0; i < 9; i++){
        oled_printChar(state[i]);
    }
    
    char* mBuf;
    if(ms == DISABLED){
        mBuf = "ENABLE MQTT ";
    }
    else{
        mBuf = "DISABLE MQTT";
    }
    oled_goToPos(6,8);
    for(uint8_t i = 0; i < 12; i++){
        oled_printChar(mBuf[i]);
    }
}

static void ui_printLampState(lampState_t ls){
    char* state;
    switch(ls){
        case ON:
            state = "ON       ";
            break;
        case OFF:
            state = "OFF      ";
            break;
        case SUS_ON:
            state = "SUSPENDED";
            break;
        case SUS_OFF:
            state = "SUSPENDED";
            break;
        default:
            state = "P-ERROR  ";
            break;
    }
    oled_goToPos(0, 91);
    for(uint8_t i = 0; i < 9; i++){
        oled_printChar(state[i]);
    }
    
    char* mBuf;
    if(ls == SUS_ON || ls == SUS_OFF){
        mBuf = "RESUME CYCLE ";
    }
    else{
        mBuf = "SUSPEND CYCLE";
          
    }
    oled_goToPos(2, 8);
    for(uint8_t i = 0; i < 13; i++){
        oled_printChar(mBuf[i]);
    } 
}

static void ui_printWater(uint8_t water){
    uint8_t val;
    if(water >= 100){
        val = 99;
    }
    else{
        val = water;
    }
    char buffer[2];
    sprintf(buffer, "%2d", val);
    oled_goToPos(1,57);
    oled_printChar(buffer[0]);
    oled_printChar(buffer[1]);
}

static void ui_updateMenuIndex(dir_t bDir){
    // Remove old:
    oled_goToPos(menuIndex + 2, 0);
    oled_printChar(' ');
    switch(bDir){
        case UP:
            menuIndex--;
            oled_goToPos(menuIndex + 2, 0);
            oled_printChar('>');
            break;
        case DOWN:
            menuIndex++;
            oled_goToPos(menuIndex + 2, 0);
            oled_printChar('>');
        default:
            break;  
    }
}

static void ui_setTime(systemState_t* state){
    state->flags.bChange = 0;
    oled_clear();
    oled_goToPos(0, 35);
    char* titleBuf = "SET SYSTEM TIME:";
    for(uint8_t i = 0; i < 16; i++){
        oled_printChar(titleBuf[i]);
    }
    uint8_t hh = state->time.hour;
    uint8_t mm = state->time.min;
    
    uint8_t hIndex = 0;
    uint8_t indexLoc[4] = {8, 54, 66, 116};
    char buffer[32];
    oled_goToPos(3, indexLoc[hIndex]);
    oled_printChar('^');
    
    
    while(hIndex < 100){
        
        if(state->flags.bChange){
            state->flags.bChange = 0;
            switch(state->bDir){
                case LEFT:
                    if(hIndex == 0){
                        hIndex = 100;
                    }
                    else{
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar(' ');
                        hIndex--;
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar('^');
                    }
                    break;
                case RIGHT:
                    if (hIndex < 3){
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar(' ');
                        hIndex++;
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar('^');
                    }
                    else{
                        rtc_setTime(hh, mm);
                        hIndex = 100;
                    }
                    break;
                case UP:
                    if(hIndex == 1){
                        if(hh < 23){
                            hh++;
                        }
                        else{
                            hh = 0;
                        }
                    }
                    else if(hIndex == 2){
                        if(mm < 59){
                            mm++;
                        }
                        else{
                            mm = 0;
                        }
                    }
                    break;
                case DOWN:
                    if(hIndex == 1){
                        if(hh > 0){
                            hh--;
                        }
                        else{
                            hh = 23;
                        }
                    }
                    else if(hIndex == 2){
                        if(mm > 0){
                            mm--;
                        }
                        else{
                            mm = 59;
                        }
                    }
                    break;
                default:
                    break; 
            }
        }
        
        oled_goToPos(2,0);
        sprintf(buffer, "CANCEL       %02d:%02d          SET", hh, mm);
        for(uint8_t i = 0; i < 31; i++){
            oled_printChar(buffer[i]);
        }
        Sleep();
    }
    oled_clear(); 
}

static void ui_setCycle(systemState_t* state){
    state->flags.bChange = 0;
    oled_clear();
    oled_goToPos(0, 35);
    char* titleBuf = "SET LAMP CYCLE:";
    for(uint8_t i = 0; i < 15; i++){
        oled_printChar(titleBuf[i]);
    }
    uint8_t sr = state->sunrise;
    uint8_t ss = state->sunset;
    
    uint8_t hIndex = 0;
    uint8_t indexLoc[4] = {8, 54, 66, 116};
    char buffer[32];
    oled_goToPos(3, indexLoc[hIndex]);
    oled_printChar('^');
    
    
    while(hIndex < 100){
        
        if(state->flags.bChange){
            state->flags.bChange = 0;
            switch(state->bDir){
                case LEFT:
                    if(hIndex == 0){
                        hIndex = 100;
                    }
                    else{
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar(' ');
                        hIndex--;
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar('^');
                    }
                    break;
                case RIGHT:
                    if (hIndex < 3){
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar(' ');
                        hIndex++;
                        oled_goToPos(3, indexLoc[hIndex]);
                        oled_printChar('^');
                    }
                    else{
                        state->sunrise = sr;
                        state->sunset = ss;
                        nvm_saveCycle(sr, ss);
                        hIndex = 100;
                    }
                    break;
                case UP:
                    if(hIndex == 1){
                        if(sr < 23){
                            sr++;
                        }
                        else{
                            sr = 0;
                        }
                    }
                    else if(hIndex == 2){
                        if(ss < 23){
                            ss++;
                        }
                        else{
                            ss = 0;
                        }
                    }
                    break;
                case DOWN:
                    if(hIndex == 1){
                        if(sr > 0){
                            sr--;
                        }
                        else{
                            sr = 23;
                        }
                    }
                    else if(hIndex == 2){
                        if(ss > 0){
                            ss--;
                        }
                        else{
                            ss = 23;
                        }
                    }
                    break;
                default:
                    break; 
            }
        }
        
        oled_goToPos(2,0);
        sprintf(buffer, "CANCEL       %02d-%02d          SET", sr, ss);
        for(uint8_t i = 0; i < 31; i++){
            oled_printChar(buffer[i]);
        }
        Sleep();
    }
    oled_clear(); 
}

void ui_printHeader(){
    oled_clear();
    // Separator between sunrise and sunseet:
    oled_goToPos(1,8);
    oled_printChar('-');
    
    // Vertical separation bars:
    oled_goToPos(0, 21);
    oled_printByte(0xFF);
    oled_goToPos(1, 21);
    oled_printByte(0xFF);
    
    // Static temperature:
    char* tempBuffer;
    tempBuffer = "T;=   $C";
    oled_goToPos(0,23);
    for(uint8_t i = 0; i < 8; i++){
        oled_printChar(tempBuffer[i]);
    }
    tempBuffer = "T<=   $C";
    oled_goToPos(1,23);
    for(uint8_t i = 0; i < 8; i++){
        oled_printChar(tempBuffer[i]);
    }
    
    // Vertical separation bars:
    oled_goToPos(0, 55);
    oled_printByte(0xFF);
    oled_goToPos(1, 55);
    oled_printByte(0xFF);
    
    // Water level:
    char* waterBuffer;
    waterBuffer = "H2O";
    oled_goToPos(0, 57);
    for(uint8_t i = 0; i < 3; i++){
        oled_printChar(waterBuffer[i]);
    }
    oled_goToPos(1, 65);
    oled_printChar('%');
    
    // Vertical separation bars:
    oled_goToPos(0, 69);
    oled_printByte(0xFF);
    oled_goToPos(1, 69);
    oled_printByte(0xFF);
    
    // MQTT and LAMP status:
    oled_goToPos(1, 71);
    char* mqttBuffer;
    mqttBuffer = "MQTT:";
    for(uint8_t i = 0; i < 5; i++){
        oled_printChar(mqttBuffer[i]);
    }
    oled_goToPos(0, 71);
    char* lampBuffer;
    lampBuffer = "LAMP:";
    for(uint8_t i = 0; i < 5; i++){
        oled_printChar(lampBuffer[i]);
    }  
}

void ui_refreshHeader(systemState_t* state){
    ui_printTemps((state->boardTemp >> 4) & 0xFF, (state->plantTemp >> 4) & 0xFF);
    ui_printCycle(state->sunrise, state->sunset);
    ui_printTime(state->time.hour, state->time.min);
    ui_printWater(state->waterLevel);
    ui_printMqttState(state->mqttState);
    ui_printLampState(state->lampState);
}

void ui_printBootMenu(){
    oled_clear();
    oled_printSplashLogo();
}

void ui_printMenu(){              
    // Set time:
    oled_goToPos(3, 8);
    char* tBuf = "SET TIME";
    for(uint8_t i = 0; i < 8; i++){
        oled_printChar(tBuf[i]);
    }
    // Set cycle:
    oled_goToPos(4, 8);
    char* cBuf = "SET CYCLE";
    for(uint8_t i = 0; i < 9; i++){
        oled_printChar(cBuf[i]);
    }
    // Set node ID:
    oled_goToPos(5, 8);
    char* nBuf = "SET NODE ID";
    for(uint8_t i = 0; i < 11; i++){
        oled_printChar(nBuf[i]);
    }
    
    oled_goToPos(menuIndex + 2, 0);
    oled_printChar('>');
}

uint8_t ui_browse(systemState_t* state){
    uint8_t update = 0;
    switch(state->bDir){
        case UP:
            if(menuIndex > 0){
                ui_updateMenuIndex(state->bDir);
            }
            update = 0;
            break;
        case DOWN:
            if(menuIndex < 4){
                ui_updateMenuIndex(state->bDir);
            }
            update = 0;
            break;
        case RIGHT:
            switch(menuIndex){
                case 0:
                    switch(state->lampState){
                        case ON:
                            state->lampState = SUS_OFF;
                            break;
                        case OFF:
                            state->lampState = SUS_ON;
                            break;
                        case SUS_OFF:
                            state->lampState = ON;
                            break;
                        case SUS_ON:
                            state->lampState = OFF;
                            break;
                        default:
                            break;
                    }
                    update = 1;
                    break;
                case 1:
                    ui_setTime(state);
                    update = 1;
                    break;
                case 2:
                    ui_setCycle(state);
                    update = 1;
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    return update;
}


