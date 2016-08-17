#!/usr/bin/python
# coding=UTF-8

from kivy.app import App
from kivy.uix.gridlayout import GridLayout
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button

import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)

class AppScreen(GridLayout):

    def __init__(self, **kwargs):
        super(AppScreen, self).__init__(**kwargs)
        self.cols = 2
        self.add_widget(Label(text='wifi helen'))
        self.configure_wifi = Button(text='configure wifi')
        self.configure_wifi.bind(on_release=self._configure_wifi)
        self.add_widget(self.configure_wifi)

    def _configure_wifi(self, obj):
        # Подключение Wifi
        cmd_rr_configure_wifi = "rr_configure_wifi ssid=helen password=13april1987 static_ip_en=true static_ip=192.168.1.115"
        cmd_rr_wifi_restart = "rr_wifi restart"

        print(cmd_rr_configure_wifi)
        ser.write(cmd_rr_configure_wifi)
        print(ser.readline())
        
        print(cmd_rr_wifi_restart)
        ser.write(cmd_rr_wifi_restart)
        print(ser.readline())

class RraptorApp(App):
    def build(self):
        return AppScreen()

RraptorApp().run()

