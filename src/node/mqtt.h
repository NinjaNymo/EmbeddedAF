//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18


#ifndef MQTT_H
#define	MQTT_H

#define MLEN_I       15
#define MTYPE_I      16
#define PUB_TOP_I    19
#define PUB_DATA_I   22
#define CONNACK    0x05
#define PUBLISH    0x0C
#define DISCONNECT 0x18
#define CA_RC_I      17
#define SUBACK     0x13
#define SA_RC_I      22
#define SUCCESS    0x00
#define BOARDTEMP  0x01
#define PLANTTEMP  0x02
#define WATER      0x03
#define SUNRISE    0x04
#define SUNSET     0x05
#define LAMPSTATE  0x06
#define NEWSUNRISE 0x07
#define NEWSUNSET  0x08


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
        uint8_t time;
    }params;
}mqtt_pubCycleFrame_t;

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
        uint8_t ls;
    }params;
}mqtt_pubLampStateFrame_t;

typedef union{
    uint8_t raw[7];
    struct{
        uint8_t length;
        uint8_t msgType;
        uint8_t flags;
        uint8_t msgID_MSB;
        uint8_t msgID_LSB;
        uint8_t topicID_MSB;
        uint8_t topicID_LSB;
    }params;
}mqtt_subscribeFrame_t;


uint8_t mqtt_connect(systemState_t* state, uint8_t keepAlive, volatile uint8_t* packet);
uint8_t mqtt_subscribe(systemState_t* state, uint8_t topicID, volatile uint8_t* packet);
void mqtt_sendConnectFrame(uint8_t clientID, uint8_t keepAlive);
void mqtt_pubTemp(uint16_t temp, uint8_t nodeID, uint8_t topic);
void mqtt_pubWaterLevel(uint8_t waterLevel, uint8_t nodeID, uint8_t topic);
void mqtt_pubCycle(uint8_t time, uint8_t nodeID, uint8_t topic);
void mqtt_pubLampState(uint8_t lampState, uint8_t nodeID, uint8_t topic);
void mqtt_disconnect();


#endif	/* MQTT_H */

