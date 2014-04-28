#ifndef RRAPTOR_PROTOCOL_H
#define RRAPTOR_PROTOCOL_H

// Протокол общения с Пультом управления

#include "stepper.h"

// Команды Rraptor

/* Получить статус системы */
static char* CMD_RR_STATUS = "rr_status";
/* Остановить все моторы */
static char* CMD_RR_STOP = "rr_stop";
/* Запустить мотор с заданной скоростью на непрерывное вращение */
static char* CMD_RR_GO = "rr_go";
/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
static char* CMD_RR_CALIBRATE = "rr_calibrate";
  
// Команды G-кода

/* Команда G-code G0 - перемещение с максимальной скоростью */
static char* CMD_GCODE_G0 = "G0";
/* Команда G-code G01 - прямая линия */
static char* CMD_GCODE_G01 = "G01";
/* Команда G-code G02 - дуга по часовой стрелке */
static char* CMD_GCODE_G02 = "G02";
/* Команда G-code G03 - дуга против часовой стрелки */
static char* CMD_GCODE_G03 = "G03";

// Ответы
static char*  REPLY_OK = "ok";
static char*  REPLY_DONTUNDERSTAND = "dontunderstand";

static char*  STATUS_IDLE = "idle";
static char*  STATUS_WORKING = "working";

// Параметры G-кодов
static char GCODE_PARAM_X = 'X';
static char GCODE_PARAM_Y = 'Y';
static char GCODE_PARAM_Z = 'Z';
static char GCODE_PARAM_F = 'F';



void init_protocol(stepper *sm_x, stepper *sm_y, stepper *sm_z);

// Обработчики команд

/** 
 * Получить текущий статус системы.
 */
int cmd_rr_status(char* reply_buffer);

/** 
 * Остановить все моторы.
 */
void cmd_rr_stop();

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
void cmd_rr_go(char motor_name, int spd);

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
void cmd_rr_calibrate(char motor_name, int spd);

/** 
 * Команда G-code G0 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 */
void cmd_gcode_g0(char motor_names[], double cvalues[], int  pcount);

/** 
 * Команда G-code G01 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 * @param f скорость перемещения мм/с.
 */
void cmd_gcode_g01(char motor_names[], double cvalues[], int  pcount, double f);

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
 * @return размер ответа в байтах (0, чтобы не отправлять ответ).
 */
int handleInput(char* buffer, char* reply_buffer);

#endif // RRAPTOR_PROTOCOL_H

