//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18


#ifndef MQTT_H
#define	MQTT_H

#define MLEN_I  15
#define MTYPE_I 16
#define CONNACK 0x05
#define CA_RC_I 17
#define SUCCESS 0x00
#define BOARDTEMPTOPIC  0x0001
#define PLANTTEMPTOPIC  0x0002
#define WATERLEVELTOPIC 0x0003


typedef union{
    uint8_t raw[7];
    struct{
        uint8_t length;
        uint8_t msgType;
        uint8_t flags;
        uint8_t protocolID;
        uint8_t durationMSB;
        uint8_t durationLSB;
        uint8_t clientID;
    }params;
}mqtt_connectFrame_t;

typedef union{
    uint8_t raw[9];
    struct{
        uint8_t length;
        uint8_t msgType;
        uint8_t flags;
        uint8_t topic_MSB;
        uint8_t topic_LSB;
        uint8_t msgID_MSB;
        uint8_t msgID_LSB;
        uint8_t temp_whole;
        uint8_t temp_decimal;
    }params;
}mqtt_pubTempFrame_t;

typedef union{
    uint8_t raw[8];
    struct{
        uint8_t length;
        uint8_t msgType;
        uint8_t flags;
        uint8_t topic_MSB;
        uint8_t topic_LSB;
        uint8_t msgID_MSB;
        uint8_t msgID_LSB;
        uint8_t waterLevel;
    }params;
}mqtt_pubWaterFrame_t;

void mqtt_connect(uint8_t clientID, uint8_t keepAlive);
void mqtt_pubTemp(uint16_t temp, uint16_t topic);
void mqtt_pubWaterLevel(uint8_t waterLevel, uint16_t topic);


#endif	/* MQTT_H */

