#   _____ _____ _____
#  |   | |__   |   | | EmbeddedAF/src/master/xbeeTesting.py
#  | | | |   __| | | | Nikolai Nymo
#  |_|___|_____|_|___| 19-04-18
from digi.xbee.devices import ZigBeeDevice
import sys
if sys.version_info[0] < 3 : raise "Must be using Python 3"


#   Instantiate device:
XBee_zig = ZigBeeDevice('/dev/tty.SLAB_USBtoUART14', 9600) # ('port', baud)
XBee_zig.open() # Opens serial interface to device

#   Read device information (more getters are availiable):
addr_64 = XBee_zig.get_64bit_addr()
node_id = XBee_zig.get_node_id()
pan_id  = XBee_zig.get_pan_id()

#   Reset device:
XBee_zig.reset()

#   Close serial interface with device:
XBee_zig.close()

print(addr_64)
print('')
print(node_id)
print('')
print(pan_id)