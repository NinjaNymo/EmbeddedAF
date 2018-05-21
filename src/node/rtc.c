//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/rtc.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 08-02-18

#include <xc.h>
#include "rtc.h"

static void setRTCWREN(){
    asm volatile("push w7");
    asm volatile("push w8");
    asm volatile("disi #5");
    asm volatile("mov #0x55, w7");
    asm volatile("mov w7, _NVMKEY");
    asm volatile("mov #0xAA, w8");
    asm volatile("mov w8, _NVMKEY");
    asm volatile("bset _RCFGCAL, #13"); // Set the RTCWREN bit
    asm volatile("pop w8");
    asm volatile("pop w7");
    asm volatile("nop");
    asm volatile("nop");  
}


static void resetTime(){
    __builtin_write_RTCWEN();
    RCFGCALbits.RTCPTR = 3; // Automatically decrements when reading RTCVAL
    RTCVAL = 0x0000;
    RTCVAL = 0x0000;
    RTCVAL = 0x0000;
    RTCVAL = 0x0000;
    RCFGCALbits.RTCWREN = 0;
}

static uint16_t decToBcd(uint16_t value){
    return ((value/10)<<4) + (value%10);
}

uint16_t bcdToDec(uint16_t value){
    return ((value & 0xF0)>> 4) * 10 + (value & 0x0F);
}


void rtc_init(uint8_t rt){
    // OSCCONbits.COSC = 0b101; // OSCILLATOR SETTINGS
    setRTCWREN();
    
    // ALARM:
    ALRMVAL = 0x00; // Just to make sure.
    ALCFGRPTbits.AMASK  = rt; // Alarm period, 0b0 = 1s, 0b10 = 10s 0b100 = 1h
    ALCFGRPTbits.CHIME  = 1;  // Chime enable, alarm goes forever
    ALCFGRPTbits.ARPT   = 1;  // Repeat 255 times
    ALCFGRPTbits.ALRMEN = 1;  // Enable the alarm
    
    
    
    RTCPWCbits.RTCLK = 0b01; // Use LRP clk, '00' for SOSC.
    RCFGCALbits.RTCEN = 1;   // RTCC Enable.
    
    RCFGCALbits.RTCWREN = 0; // Clear the RTCC write enable flag.
}

void rtc_setTime(uint8_t hh, uint8_t mm){
    __builtin_write_RTCWEN();
    RCFGCALbits.RTCPTR = 1;
    RTCVAL = 0x00FF & decToBcd(hh);
    RTCVAL =  decToBcd(0x00FF & mm ) << 8;
    RCFGCALbits.RTCWREN = 0;
}

time_t rtc_getTime(){
    uint16_t min_sec, wDay_hour;
    RCFGCALbits.RTCPTR = 1;
    wDay_hour = RTCVAL;
    min_sec = RTCVAL;
    
    time_t ct;
    
    ct.sec  =  bcdToDec(min_sec) & 0x00FF;
    ct.min  =  bcdToDec((min_sec & 0xFF00) >> 8);
    ct.hour =  bcdToDec(wDay_hour) & 0x00FF;
    
    return ct;
}

dateTime_t rtc_getDateTime(){
    uint16_t min_sec, wDay_hour, month_mDay, year;
    RCFGCALbits.RTCPTR = 3;
    year = RTCVAL;
    month_mDay = RTCVAL;
    wDay_hour = RTCVAL;
    min_sec = RTCVAL;
    
    dateTime_t currentDateTime;
    currentDateTime.sec   =  bcdToDec(min_sec)    & 0x00FF;
    currentDateTime.min   = (bcdToDec(min_sec)    & 0xFF00) >> 8;
    currentDateTime.hour  =  bcdToDec(wDay_hour)  & 0x00FF;
    currentDateTime.wDay  = (bcdToDec(wDay_hour)  & 0xFF00) >> 8;
    currentDateTime.mDay  =  bcdToDec(month_mDay) & 0x00FF;
    currentDateTime.month = (bcdToDec(month_mDay) & 0xFF00) >> 8;
    currentDateTime.year  =   bcdToDec(year)      & 0x00FF;
    
    return currentDateTime;
}



