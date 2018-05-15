import serial
from time import sleep
port = input("Enter port: ")

ser = serial.Serial(port, 9600)

#sleep(2)
#print('Sending hello world...')

##  7E 00 19 10 01 00 00 00 00 00 00 00 00 FF FE 00 00 48 45 4C 4C 4F 20 57 4F 52 4C 44 D5
#hw = b"\x7E\x00\x19\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFE\x00\x00\x48\x45\x4C\x4C\x4F\x20\x57\x4F\x52\x4C\x44\xD5"
#ser.write(hw)

while True:
    a = ser.read()
    print(hex(int.from_bytes(a,byteorder='little')), end = ' ' )

