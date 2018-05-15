//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18

#include <xc.h>
#include "mqtt.h"


void mqtt_connect(uint8_t clientID, uint16_t keepAlive){
    mqtt_connectFrame_t cf = {.params = {7, 0x04, 0b00000100, 0x01, keepAlive, clientID}};
}