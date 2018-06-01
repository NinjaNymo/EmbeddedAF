//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/mqtt.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 23-04-18

#include <xc.h>
#include "xbee.h"
#include "types.h"
#include "utilities.h"
#include "mqtt.h"


uint8_t mqtt_connect(systemState_t* state, uint8_t keepAlive, volatile uint8_t* packet){
    mqtt_connectFrame_t cf = {.params = {7, 0x04, 0b00000100, 0x01, 0x00, keepAlive, state->settings.fields.nodeID}};
    xbee_transmitRequest(cf.raw, 7);
    
    // Wait for CONNACK:
    util_startTimer23();
    uint8_t returnCode = 0;
    while(1){
        if(state->flags.rxPacket){
            state->flags.rxPacket = 0;
            // Check for CONNACK:
            if(packet[MTYPE_I] == CONNACK){
                // Check return code:
                if(packet[CA_RC_I] == SUCCESS){
                    returnCode = 1;
                }
                else{
                    returnCode = 0;
                }
            }
            break;
        }
        else if(state->flags.timeOut){
            state->flags.timeOut = 0;
            returnCode = 0; // Connection failed
            break;
        }
        else{
            Idle();
        }
    }
    util_stopTimer23();
    return returnCode;
}



uint8_t mqtt_subscribe(systemState_t* state, uint8_t topicID, volatile uint8_t* packet){
    mqtt_subscribeFrame_t sf = {.params = {7, 0x12, 0b00000001, 0x00, 0x00, state->settings.fields.nodeID, topicID}};
    xbee_transmitRequest(sf.raw, 7);
    
    // Wait for SUBACK:
    util_startTimer23();
    uint8_t returnCode = 0;
    while(1){
        if(state->flags.rxPacket){
            state->flags.rxPacket = 0;
            // Check for CONNACK:
            if(packet[MTYPE_I] == SUBACK){
                // Check return code:
                if(packet[SA_RC_I] == SUCCESS){
                    returnCode = 1;
                }
                else{
                    returnCode = 0;
                }
            }
            break;
        }
        else if(state->flags.timeOut){
            state->flags.timeOut = 0;
            returnCode = 0; // Connection failed
            break;
        }
        else{
            Idle();
        }
    }
    util_stopTimer23();
    return returnCode;
}

void mqtt_disconnect(){
    uint8_t df[2] = {2, DISCONNECT};
    xbee_transmitRequest(df, 2);
}


void mqtt_sendConnectFrame(uint8_t clientID, uint8_t keepAlive){
    mqtt_connectFrame_t cf = {.params = {7, 0x04, 0b00000100, 0x01, 0x00, keepAlive, clientID}};
    xbee_transmitRequest(cf.raw, 7);
}

void mqtt_pubTemp(uint16_t temp, uint8_t nodeID, uint8_t topic){
    uint8_t temWhole       = (temp >> 4) & 0xFF;
    uint8_t temDec         = temp & 0x0F; 
    mqtt_pubTempFrame_t tf = {.params = {9, 0x0C, 0b00000001, nodeID, topic, 0x00, 0x00, temWhole, temDec}};
    xbee_transmitRequest(tf.raw, 9);
}

void mqtt_pubWaterLevel(uint8_t waterLevel, uint8_t nodeID, uint8_t topic){
    mqtt_pubWaterFrame_t wf = {.params = {8, 0x0C, 0b00000001, nodeID, topic, 0x00, 0x00, waterLevel}};
    xbee_transmitRequest(wf.raw, 8);
}

void mqtt_pubCycle(uint8_t time, uint8_t nodeID, uint8_t topic){
    mqtt_pubWaterFrame_t cf = {.params = {8, 0x0C, 0b00000001, nodeID, topic, 0x00, 0x00, time}};
    xbee_transmitRequest(cf.raw, 8);
}

void mqtt_pubLampState(uint8_t lampState, uint8_t nodeID, uint8_t topic){
    mqtt_pubWaterFrame_t lf = {.params = {8, 0x0C, 0b00000001, nodeID, topic, 0x00, 0x00, lampState}};
    xbee_transmitRequest(lf.raw, 8);
}