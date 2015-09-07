#!/usr/bin/python
# coding=UTF-8

import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

# Настройки моторов
# Rraptor 0.2 , 9, 10, 1, 1000, 7.5
# rr_configure_motor motor_name [pulse_delay=us] [distance_per_step=um] 
# rr_configure_motor motor_name [min_end_strategy=CONST/AUTO/INF] [max_end_strategy=CONST/AUTO/INF] 
# rr_configure_motor motor_name [min_pos=um] [max_pos=um] [current_pos=um]
# rr_configure_motor_pins motor_name [pin_step=val] [pin_dir=val] [pin_en=val] [dir_inv=1/-1] [pin_min=val] [pin_max=val]

cmd_conf_x1="rr_configure_motor x pulse_delay=1000 distance_per_step=7.5"
cmd_conf_x2="rr_configure_motor x min_end_strategy=CONST max_end_strategy=CONST"
cmd_conf_x3="rr_configure_motor x min_pos=0 max_pos=300000 current_pos=0"
cmd_conf_x4="rr_configure_motor_pins x pin_step=8 pin_dir=9 pin_en=10 dir_inv=1 pin_min=-1 pin_max=-1"

cmd_conf_y1="rr_configure_motor y pulse_delay=1000 distance_per_step=7.5"
cmd_conf_y2="rr_configure_motor y min_end_strategy=CONST max_end_strategy=CONST"
cmd_conf_y3="rr_configure_motor y min_pos=0 max_pos=216000 current_pos=0"
cmd_conf_y4="rr_configure_motor_pins y pin_step=5 pin_dir=6 pin_en=7 dir_inv=-1 pin_min=-1 pin_max=-1"

cmd_conf_z1="rr_configure_motor z pulse_delay=1000 distance_per_step=7.5"
cmd_conf_z2="rr_configure_motor z min_end_strategy=CONST max_end_strategy=CONST"
cmd_conf_z3="rr_configure_motor z min_pos=0 max_pos=100000 current_pos=0"
cmd_conf_z4="rr_configure_motor_pins z pin_step=2 pin_dir=3 pin_en=4 dir_inv=-1 pin_min=-1 pin_max=-1"

# Configure motor X
print(cmd_conf_x1)
ser.write(cmd_conf_x1)
print(ser.readline())

print(cmd_conf_x2)
ser.write(cmd_conf_x2)
print(ser.readline())

print(cmd_conf_x3)
ser.write(cmd_conf_x3)
print(ser.readline())

print(cmd_conf_x4)
ser.write(cmd_conf_x4)
print(ser.readline())

# Configure motor Y

print(cmd_conf_y1)
ser.write(cmd_conf_y1)
print(ser.readline())

print(cmd_conf_y2)
ser.write(cmd_conf_y2)
print(ser.readline())

print(cmd_conf_y3)
ser.write(cmd_conf_y3)
print(ser.readline())

print(cmd_conf_y4)
ser.write(cmd_conf_y4)
print(ser.readline())

# Configure motor Z

print(cmd_conf_z1)
ser.write(cmd_conf_z1)
print(ser.readline())

print(cmd_conf_z2)
ser.write(cmd_conf_z2)
print(ser.readline())

print(cmd_conf_z3)
ser.write(cmd_conf_z3)
print(ser.readline())

print(cmd_conf_z4)
ser.write(cmd_conf_z4)
print(ser.readline())


