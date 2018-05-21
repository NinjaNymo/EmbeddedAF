//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18

#include <xc.h>
#include "xbee.h"
#include "mqtt.h"


void mqtt_connect(uint8_t clientID, uint8_t keepAlive){
    mqtt_connectFrame_t cf = {.params = {7, 0x04, 0b00000100, 0x01, 0x00, keepAlive, clientID}};
    xbee_transmitRequest(cf.raw, 7);
}

void mqtt_pubTemp(uint16_t temp, uint16_t topic){
    uint8_t topMSB         = (topic >> 8) & 0xFF;
    uint8_t topLSB         = topic & 0xFF;
    uint8_t temWhole       = (temp >> 4) & 0xFF;
    uint8_t temDec         = temp & 0x0F; 
    mqtt_pubTempFrame_t tf = {.params = {9, 0x0C, 0b00000001, topMSB, topLSB, 0x00, 0x00, temWhole, temDec}};
    xbee_transmitRequest(tf.raw, 9);
}

void mqtt_pubWaterLevel(uint8_t waterLevel, uint16_t topic){
    uint8_t topMSB          = (topic >> 8) & 0xFF;
    uint8_t topLSB          = topic & 0xFF;
    mqtt_pubWaterFrame_t wf = {.params = {8, 0x0C, 0b00000001, topMSB, topLSB, 0x00, 0x00, waterLevel}};
    xbee_transmitRequest(wf.raw, 8);
}