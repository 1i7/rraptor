# coding=UTF-8

import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)


# Подключения Wifi
#cmd1 = "rr_configure_wifi ssid=lasto4ka password=robotguest static_ip_en=true static_ip=192.168.43.115"
#cmd2 = "rr_wifi restart"

#cmd3 = "rr_configure_wifi ssid=lasto4ka1 password=robotguest static_ip_en=true static_ip=192.168.2.115"
#cmd4 = "rr_wifi restart"

cmd5 = "rr_configure_wifi ssid=helen password=13april1987 static_ip_en=true static_ip=192.168.1.115"
cmd6 = "rr_wifi restart"


#ser.write(cmd5)
#print(ser.readline())
#ser.write(cmd6)
#print(ser.readline())


ser.write("rr_wifi status")
print(ser.readline())
#print(ser.read())

