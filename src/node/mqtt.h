//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18


#ifndef MQTT_H
#define	MQTT_H

typedef union{
    uint8_t raw[7];
    struct{
        uint8_t length;
        uint8_t msgType;
        uint8_t flags;
        uint8_t protocolID;
        uint16_t duration;
        uint8_t clientID;
    }params;
}mqtt_connectFrame_t;

void mqtt_connect(uint8_t clientID, uint16_t keepAlive);


#endif	/* MQTT_H */

