#ifndef RRAPTOR_PROTOCOL_H
#define RRAPTOR_PROTOCOL_H

// Протокол общения с Пультом управления

#include "stepper.h"


// 
static const char* CMD_PING = "ping";

// Команды Rraptor

/* Получить статус системы */
static const char* CMD_RR_STATUS = "rr_status";
/* Остановить все моторы */
static const char* CMD_RR_STOP = "rr_stop";
/* Запустить мотор с заданной скоростью на непрерывное вращение */
static const char* CMD_RR_GO = "rr_go";
/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
static const char* CMD_RR_CALIBRATE = "rr_calibrate";
  
// Команды G-кода

/* Команда G-code G0 - перемещение с максимальной скоростью */
static const char* CMD_GCODE_G0 = "G0";
/* Команда G-code G01 - прямая линия */
static const char* CMD_GCODE_G01 = "G01";
/* Команда G-code G02 - дуга по часовой стрелке */
static const char* CMD_GCODE_G02 = "G02";
/* Команда G-code G03 - дуга против часовой стрелки */
static const char* CMD_GCODE_G03 = "G03";

// Ответы
static const char* REPLY_OK = "ok";
static const char* REPLY_DONTUNDERSTAND = "dontunderstand";
static const char* REPLY_ERROR = "error";
static const char* REPLY_BUSY = "busy";

static const char* STATUS_IDLE = "idle";
static const char* STATUS_WORKING = "working";

// Параметры G-кодов
static const char GCODE_PARAM_X = 'X';
static const char GCODE_PARAM_Y = 'Y';
static const char GCODE_PARAM_Z = 'Z';
static const char GCODE_PARAM_F = 'F';

void init_protocol(stepper *sm_x, stepper *sm_y, stepper *sm_z);

// Обработчики команд

/** 
 * Проверить доступность устройства.
 */
int cmd_ping(char* reply_buffer);

/** 
 * Получить текущий статус устройства.
 */
int cmd_rr_status(char* reply_buffer);

/** 
 * Остановить все моторы.
 */
int cmd_rr_stop(char* reply_buffer);

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
int cmd_rr_go(char motor_name, int spd, char* reply_buffer);

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
int cmd_rr_calibrate(char motor_name, int spd, char* reply_buffer);

/** 
 * Команда G-code G0 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 */
int cmd_gcode_g0(char motor_names[], double cvalues[], int  pcount, char* reply_buffer);

/** 
 * Команда G-code G01 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 * @param f скорость перемещения мм/с.
 */
int cmd_gcode_g01(char motor_names[], double cvalues[], int  pcount, double f, char* reply_buffer);

/** 
 * Команда G-code G02 - дуга по часовой стрелке.
 */
void cmd_gcode_g02();

/** 
 * Команда G-code G03 - дуга против часовой стрелки.
 */
void cmd_gcode_g03();


/**
 * Обработать входные данные - разобрать строку, выполнить команду.
 * @buffer - входные данные, строка, оканчивающаяся нулём.
 * @reply_buffer - ответ, строка, оканчивающая нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ).
 */
int handleInput(char* buffer, char* reply_buffer);

#endif // RRAPTOR_PROTOCOL_H

