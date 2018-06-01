from threading import Thread
import logging
from queue import Queue
from time import sleep
from digi.xbee.devices import ZigBeeDevice
from MSG import MSG

DEBUG = True


class XBeeHandler(Thread):
    def __init__(self, inQ, outQ, XBeePort):
        super(XBeeHandler, self).__init__()
        self.daemon = True
        self.name   = 'XBH'
        self.inQ    = inQ
        self.outQ   = outQ

        # Initialize XBee device:
        self.device = ZigBeeDevice(XBeePort, 9600)
        self.device.open()
        self.device.add_data_received_callback(self.receiveCB)

    def run(self):
        while(True):
            msg = self.outQ.get() # Blocking call
            self.device.send_data(msg.sender, msg.data)
            if DEBUG:print('XBH - Sent %s bytes to %s' % (len(msg.data), msg.sender.get_64bit_addr()))
            
            
    def receiveCB(self, xbee_message):
        sender = xbee_message.remote_device
        data = xbee_message.data
        msg  = MSG(sender, data)
        self.inQ.put(msg)
        if DEBUG:print('XBH - Received %s bytes from %s' % (len(msg.data), sender.get_64bit_addr()))
    
    def terminate(self):
        self.device.close()
        if DEBUG:print('XBH - Closed connection with RF device.')
