#!/usr/bin/python
# coding=UTF-8
import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

# Подключение Wifi
cmd_rr_configure_wifi = "rr_configure_wifi ssid=lasto4ka1 password=robotguest static_ip_en=true static_ip=192.168.2.115"
cmd_rr_wifi_restart = "rr_wifi restart"

ser.write(cmd_rr_configure_wifi)
print(ser.readline())
ser.write(cmd_rr_wifi_restart)
print(ser.readline())


