from serial import Serial
import struct
arduinoSerial = Serial('COM4', 9600)
while True:
    if arduinoSerial.read() == b'$':
        data = bytearray(arduinoSerial.read(4))
        rollDegree = struct.unpack('f', data[0:4])[0]
        print(rollDegree)