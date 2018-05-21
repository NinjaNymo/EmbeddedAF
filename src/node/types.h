//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/types.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 08-02-18

#ifndef TYPES_H
#define	TYPES_H

typedef enum {ENABLED, DISCONNECTED, CONNECTED, DISABLED} mqttState_t;
typedef enum {ON, OFF, SUS_ON, SUS_OFF} lampState_t;
typedef enum {UP, DOWN, LEFT, RIGHT, NONE} dir_t;
typedef enum {INIT, CONNECTING, IDLE, REFRESH, PUB_VALS, PUB_STATE, HANDLE_MSG, BROWSE} loopState_t;

typedef struct{ 
    uint8_t sec; 
    uint8_t min; 
    uint8_t hour; 
    uint8_t wDay; 
    uint8_t mDay; 
    uint8_t month; 
    uint8_t year;
}dateTime_t; 
 
typedef struct{ 
    uint8_t sec; 
    uint8_t min; 
    uint8_t hour; 
}time_t; 

typedef struct{
    volatile uint8_t timeOut;
    volatile uint8_t rxPacket;
    volatile uint8_t bChange;
}flags_t;

typedef union{
    uint16_t raw;
    struct{
        uint8_t        nodeID;
        uint8_t   mqttSetting;
    }fields;
}settings_t;

typedef struct{
    loopState_t loopState;
    uint8_t sunrise;
    uint8_t sunset;
    settings_t settings;
    time_t time;
    uint16_t boardTemp;
    uint16_t plantTemp;
    uint8_t waterLevel;
    lampState_t lampState;
    mqttState_t mqttState;
    flags_t flags;
    dir_t bDir;
}systemState_t;





#endif	/* TYPES_H */

