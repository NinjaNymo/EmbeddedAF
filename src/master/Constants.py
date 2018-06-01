#-------------------#
#   MQTT-SN Macros  #
#-------------------#
TYPE_i        = 1 # Index of type field in all frames
NODEID_i      = 6 # Index of Node ID in CONNECT frames
PUB_TOPIC_U_i = 3 # Index of upper topic ID in PUBLISH frames
PUB_TOPIC_L_i = 4 # Index of lower topic ID in PUBLISH frames
SUB_TOPIC_U_i = 5 # Index of upper topic ID in SUBSCRIBE frames
SUB_TOPIC_L_i = 6 # Index of lower topic ID in SUBSCRIBE frames 

CONNECT    = 0x04 # CONNECT MsgType
CONNACK    = 0x05 # CONNACK MsgType
SUBSCRIBE  = 0x12 # SUBSCRIBE MsgType
SUBACK     = 0x13 # SUBACK MsgType
PUBLISH    = 0x0C # PUBLISH MsgType
DISCONNECT = 0x18 # DISCONNECT MsgType

#-------------------#
#    Topic Names    #
#-------------------#
topicNames = {
    0x01:'BoardTemp', 
    0x02:'PlantTemp', 
    0x03:'Water', 
    0x04:'Sunrise', 
    0x05:'Sunset', 
    0x06:'LampState', 
    0x07:'NewSunrise', 
    0x08:'NewSunset'
}
topicIDs = {
    'BoardTemp':0x01,
    'PlantTemp':0x02,
    'Water':0x03,
    'Sunrise':0x04,
    'Sunset':0x05,
    'LampState':0x06,
    'NewSunrise':0x07,
    'NewSunset':0x08
}
#-------------------#
#    Lamp state     #
#-------------------#
lampStates = {
    0:'ON', 
    1:'OFF', 
    2:'SUSPENDED(ON)', 
    3:'SUSPENDED(OFF)'
}