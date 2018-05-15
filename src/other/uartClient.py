#   _____ _____ _____
#  |   | |__   |   | | EmbeddedAF/src/master/uartClient.py
#  | | | |   __| | | | Nikolai Nymo
#  |_|___|_____|_|___| 19-04-18

import serial

ser = serial.Serial('/dev/tty.SLAB_USBtoUART')
print('Serial port in use = ' + ser.name)

def zig_broadcastChar(char):
    f_type = 0x10
    f_id = 0x00
    addr_64 = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF]
    addr_16 = [0xFF, 0xFF]
    radius = 0x00
    options = 0x00
    fData = [f_type, f_id] + addr_64 + addr_16 + [radius, options, ord(char)]
    length_fData = len(fData)
    print('Length of frame data = ', len(fData))
    len_lsb = length_fData & 0xFF
    len_msb = int(length_fData/256)
    cs = 0xFF - (sum(fData) & 0xFF)
    start_delim = 0x7E
    api_frame = [start_delim, len_msb, len_lsb] + fData + [cs]
    print('Length of api frame = ', len(api_frame))
    
    for B in api_frame:
        print(hex(B))
    return api_frame




bytes_sent = ser.write(zig_broadcastChar('A'))

#print('Bytes sent = ', bytes_sent)
ser.close()

# 7E 00 0F 10 00 00 00 00 00 00 00 FF FF FF FF 00 00 41 B2


