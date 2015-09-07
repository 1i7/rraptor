#!/usr/bin/python
# coding=UTF-8

import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

# Информация о системе и моторах

cmd_wifi_info="rr_wifi info"
cmd_wifi_status="rr_wifi status"

cmd_info_x1="rr_motor_info x"
cmd_info_x2="rr_motor_pin_info x"

cmd_info_y1="rr_motor_info y"
cmd_info_y2="rr_motor_pin_info y"

cmd_info_z1="rr_motor_info z"
cmd_info_z2="rr_motor_pin_info z"

#Wifi
print(cmd_wifi_info)
ser.write(cmd_wifi_info)
print(ser.readline())

print(cmd_wifi_status)
ser.write(cmd_wifi_status)
print(ser.readline())

# Motor X
print(cmd_info_x1)
ser.write(cmd_info_x1)
print(ser.readline())

print(cmd_info_x2)
ser.write(cmd_info_x2)
print(ser.readline())

# Motor Y
print(cmd_info_y1)
ser.write(cmd_info_y1)
print(ser.readline())

print(cmd_info_y2)
ser.write(cmd_info_y2)
print(ser.readline())


# Motor Z
print(cmd_info_z1)
ser.write(cmd_info_z1)
print(ser.readline())

print(cmd_info_z2)
ser.write(cmd_info_z2)
print(ser.readline())


