from threading import Thread
import paho.mqtt.client as mqtt
from queue import Queue
from digi.xbee.devices import ZigBeeDevice
from MSG import MSG
from Constants import *

DEBUG = True



class NodeClient(Thread):
    def __init__(self, device, outQ, nodeID):
        super(NodeClient, self).__init__()
        self.daemon  = True

        self.inQ     = Queue()
        self.outQ    = outQ
        self.stop    = False
        self.nodeID  = nodeID
        self.name    = nodeID

        self.device = device
        self.mClient = mqtt.Client()

        self.mClient.on_connect    = self.connectedCB
        self.mClient.on_message    = self.messageCB
        self.mClient.on_subscribe  = self.subscribedCB 
        self.mClient.on_disconnect = self.disconnectedCB
    
    def run(self):
        while not self.stop:
            msg = self.inQ.get()
            if msg.data[TYPE_i] is CONNECT:
                self.connect((msg.data[4] << 8) + msg.data[5])
            elif msg.data[TYPE_i] is PUBLISH:
                self.publish(msg)
            elif msg.data[TYPE_i] is SUBSCRIBE:
                self.subscribe(msg)

        if DEBUG:print('NC%s - terminating' % str(self.nodeID))

    def connect(self, keepAlive):
        self.mClient.connect("localhost", 1883, keepAlive)
        self.mClient.loop_start()

    def connectedCB(self, client, userdata, flags, rc):
        if DEBUG:print('NC%s - Connected to broker with result code = %s' % (str(self.nodeID), str(rc)))
        if rc is 0:
            rc_sn = 0
        else:
            rc_sn = 1
        data = [3, CONNACK, rc_sn]
        msg = MSG(self.device, bytearray(data))
        self.outQ.put(msg)

    def subscribe(self, msg):
        topic = str(msg.data[SUB_TOPIC_U_i]) + topicNames[msg.data[SUB_TOPIC_L_i]]
        if DEBUG:print('NC%s - Subscribing to %s' % (self.nodeID, topic))
        self.mClient.subscribe(topic)
    
    def subscribedCB(self, client, userdata, mid, granted_qos):
        if DEBUG:print('NC%s - Subscribed with granted QOS %s' % (self.nodeID, granted_qos))
        data = [8, SUBACK, 0, 0, 0, 0, 0, granted_qos[0]]
        msg = MSG(self.device, bytearray(data))
        self.outQ.put(msg)
    
    def messageCB(self, client, userdata, msg):
        topic = msg.topic[len(str(self.nodeID)):]
        topicID = topicIDs[topic]
        if (topicNames[topicID] is 'NewSunrise') or (topicNames[topicID] is 'NewSunset'):
            if DEBUG:print('NC%s - Received message on %s / %s' % (str(self.nodeID), topic, str(topicID)))
            data = [8, PUBLISH, 1, self.nodeID, topicID, 0, 0, int(msg.payload)]
            msg_out = MSG(self.device, bytearray(data))
            self.outQ.put(msg_out)


    def publish(self, msg):
        topicID = topicNames[msg.data[PUB_TOPIC_L_i]]
        nodeID  = str(msg.data[PUB_TOPIC_U_i])
        topic = nodeID + topicID

        if (topicID is 'BoardTemp') or (topicID is 'PlantTemp'):
            temp =  temp = msg.data[7] + ((msg.data[8])/100)
            self.mClient.publish(topic, temp)
        elif topicID is 'LampState':
            ls = lampStates[msg.data[7]]
            self.mClient.publish(topic, ls)
        else:
            self.mClient.publish(topic, msg.data[7])
        if DEBUG:print('NC%s - Published to %s' % (str(self.nodeID), topic))
    
    def disconnect(self):
        self.mClient.disconnect()
        self.mClient.loop_stop()
        
    def disconnectedCB(self, client, userdata, rc):
        if DEBUG:print('NC%s - Disconnected from broker.' % str(self.nodeID))

    def put(self, msg):
        self.inQ.put(msg)
    
    def terminate(self):
        self.disconnect()
        self.stop = True
        if DEBUG:print('NC%s - Shutting down' % str(self.nodeID))

        