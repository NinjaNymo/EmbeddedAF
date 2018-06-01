#   _____ _____ _____
#  |   | |__   |   | | EmbeddedAF/src/master/xbeeTesting.py
#  | | | |   __| | | | Nikolai Nymo
#  |_|___|_____|_|___| 19-04-18
from digi.xbee.devices import ZigBeeDevice
import sys
if sys.version_info[0] < 3 : raise "Must be using Python 3"
from time import sleep


#   Instantiate device:
XBee_zig = ZigBeeDevice('/dev/tty.SLAB_USBtoUART', 9600) # ('port', baud)
XBee_zig.open() # Opens serial interface to device

# Define callback.
def xbRxCallback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    print("Received data from %s: %s" % (address, data))

# Add the callback.
XBee_zig.add_data_received_callback(xbRxCallback)

#   Read device information (more getters are availiable):
addr_64 = XBee_zig.get_64bit_addr()
node_id = XBee_zig.get_node_id()
pan_id  = XBee_zig.get_pan_id()
print('Opened device with addr')
print(addr_64)
print('and node ID')
print(node_id)
print('on PAN ID')
print(pan_id)
print('listening for messages...')

#msg = XBee_zig.read_data(120);

#data = msg.data.decode("utf8")
#print(data)



while(True):
    sleep(10)






#   Reset device:
#XBee_zig.reset()

#   Close serial interface with device:
XBee_zig.close()

print(addr_64)
print('')
print(node_id)
print('')
print(pan_id)