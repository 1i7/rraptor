#!/usr/bin/python
# coding=UTF-8

import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

#cmd_G0 = "G0 X130 Y100 Z10 F3"
cmd_G02 = "G02 X100 Y100 R30 F3"

ser.write(cmd_G02)
print(ser.readline())

