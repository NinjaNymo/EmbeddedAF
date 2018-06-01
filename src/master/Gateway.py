from digi.xbee.devices import ZigBeeDevice
from queue import Queue
import sys
from XBeeHandler import XBeeHandler
from NodeClient import NodeClient
from MSG import MSG
from Constants import *


#-------------------#
#   MQTT-SN Macros  #
#-------------------#
#TYPE_i    = 1 # Index of type field in all frames
#NODEID_i  = 6 # Index of Node ID in CONNECT frames

#CONNECT = 0x04 # CONNECT MsgType

DEBUG = True

class Gateway:
    def __init__(self, uartPort):
        self.inQ   = Queue()  # Holds incoming RF messages
        self.outQ  = Queue()  # Holds outgoing RF messages

        self.clientList = [] # List of client nodes

        # Initialize XBee device handler class:
        self.XBH = XBeeHandler(self.inQ, self.outQ, uartPort)
        self.XBH.start()

    def start(self):
        try:
            while(True):
                msg = self.inQ.get() # Blocking call


                # Handle CONNECT messages:
                if msg.data[TYPE_i] is CONNECT:
                    nodeID = msg.data[NODEID_i]
                    # See if client already exists for the node:
                    for i in range(0, len(self.clientList)):
                        if self.clientList[i].nodeID is nodeID:
                            # Remove previous client thread (clean session):
                            self.clientList[i].terminate()
                            self.clientList.pop(i)
                            if DEBUG:print('GW - Removed client for Node ID %s.' % str(nodeID))
                    
                    # Create new client thread:
                    c = NodeClient(msg.sender, self.outQ, nodeID)
                    c.start()
                    self.clientList.append(c)
                    if DEBUG:print('GW - Added client for Node ID %s.' % str(nodeID))
                # Forward msg to respective client thread:
                for i in range(0, len(self.clientList)):
                    if self.clientList[i].device.get_64bit_addr() is msg.sender.get_64bit_addr():
                        if msg.data[TYPE_i] is DISCONNECT:
                            self.clientList[i].terminate()
                            self.clientList.pop(i)
                            if DEBUG:print('GW - Removed client for Node ID %s.' % str(nodeID))
                        else:
                            c.put(msg)
        except KeyboardInterrupt:
            self.stop()
    
    def stop(self):
        self.XBH.terminate() # Gracefully shut down XBee handler thread
        for c in self.clientList:
            c.terminate() # Gracefully shut down all client threads
        if DEBUG:print('GW - terminated clients and XBH, shutting down GW.')


def main():
    inQ  = Queue()  # Holds incoming RF messages
    outQ = Queue()  # Holds outgoing RF messages
    clientList = [] # Holds list of clients  

    # Start the XBee handler thread
    XBH  = XBeeHandler(inQ, outQ, '/dev/ttyAMA0')
    XBH.start()

    try:
        while(True):
            msg = inQ.get() # Blocking call

            if msg.data[TYPE_i] is CONNECT:
                nodeID = msg.data[NODEID_i]
                inList = False
                for c in clientList:
                    if c.nodeID is msg.data[NODEID_i]:
                        inList = True
                if not inList:
                    c = NodeClient(msg.sender, outQ, msg.data[NODEID_i])
                    c.start()
                    clientList.append(c)
                    print('Main - Created new connection')
            for c in clientList:
                if c.xDevice.get_64bit_addr() is msg.sender.get_64bit_addr():
                    c.put(msg)
    except KeyboardInterrupt:
        XBH.terminate()
        for c in clientList:
            c.terminate()
        return

GW = Gateway('/dev/ttyAMA0') # '/dev/ttyAMA0' on RPi
GW.start()