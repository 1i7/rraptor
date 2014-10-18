#ifndef RRAPTOR_CONFIG_H
#define RRAPTOR_CONFIG_H

static const char* DEVICE_NAME = "Anton's Rraptor";
static const char* DEVICE_MODEL = "Rraptor";
static const char* DEVICE_SERIAL_NUMBER = "00000003";
static const char* DEVICE_DESCRIPTION = "Multifunctional CNC unit";
static const char* DEVICE_VERSION = "0.2-devel";
static const char* DEVICE_MANUFACTURER = "Rraptor team";
static const char* DEVICE_URI = "http://rraptor.lasto4ka.su";

#define DEBUG_SERIAL

// Включить/выключить нужные каналы связи
// Последовательный порт Serial (при машинном чтении ответов не 
// использовать вместе с DEBUG_SERIAL)
#define RR_SERIAL
// Прямое управление через беспроводной канал WiFi
#define RR_TCP
// Сервер Роботов
//#define RR_ROBOT_SERVER
// Аксессуар USB Android
//#define RR_USB_ACCESSORY

#endif // RRAPTOR_CONFIG_H

