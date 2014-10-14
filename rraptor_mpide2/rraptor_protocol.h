#ifndef RRAPTOR_PROTOCOL_H
#define RRAPTOR_PROTOCOL_H

// Протокол общения с Пультом управления

#include "stepper.h"

/**************************************/
// Универсальные команды (для Сервера Роботов)

// Постоянные свойства
/** Получить собственное имя устройства */
static const char* CMD_NAME = "name";
/** Получить модель устройства */
static const char* CMD_MODEL = "model";
/** Получить серийный номер устройства */
static const char* CMD_SERIAL_NUMBER = "sn";
/** Получить словесное описание устройства */
static const char* CMD_DESCRIPTION = "description";
/** Получить версию прошивки устройства */
static const char* CMD_VERSION = "version";
/** Получить производителя устройства */
static const char* CMD_MANUFACTURER = "manufacturer";
/** Получить ссылку на страницу устройства */
static const char* CMD_URI = "uri";

// Команды
/** Проверить доступность устройства */
static const char* CMD_PING = "ping";

/**************************************/
// Команды Rraptor

// Постоянные свойства
/** Получить размер рабочей области */
static const char* CMD_RR_WORKING_AREA_DIM = "rr_working_area_dim";

// Динамические свойства
/** Получить статус устройства */
static const char* CMD_RR_STATUS = "rr_status";
/** Получить текущее положение печатающего блока */
static const char* CMD_RR_CURRENT_POS = "rr_current_pos";

// Команды
/** Остановить все моторы */
static const char* CMD_RR_STOP = "rr_stop";
/** Запустить мотор с заданной скоростью на непрерывное вращение */
static const char* CMD_RR_GO = "rr_go";
/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
static const char* CMD_RR_CALIBRATE = "rr_calibrate";
  
/**************************************/
// Команды G-кода

/** Команда G-code G0 - перемещение с максимальной скоростью */
static const char* CMD_GCODE_G0 = "G0";
/** Команда G-code G01 - прямая линия */
static const char* CMD_GCODE_G01 = "G01";
/* Команда G-code G02 - дуга по часовой стрелке */
static const char* CMD_GCODE_G02 = "G02";
/** Команда G-code G03 - дуга против часовой стрелки */
static const char* CMD_GCODE_G03 = "G03";

/**************************************/
// Ответы
static const char* REPLY_OK = "ok";
static const char* REPLY_DONTUNDERSTAND = "dontunderstand";
static const char* REPLY_ERROR = "error";
static const char* REPLY_BUSY = "busy";

static const char* STATUS_IDLE = "idle";
static const char* STATUS_WORKING = "working";

/**************************************/
// Параметры G-кодов
static const char GCODE_PARAM_X = 'X';
static const char GCODE_PARAM_Y = 'Y';
static const char GCODE_PARAM_Z = 'Z';
static const char GCODE_PARAM_F = 'F';

/**
 * Установить информацию об устройстве. 
 */
void init_device_info(const char* name, const char* model, const char* serial_number, 
        const char* description, const char* _version,
        const char* manufacturer, const char* uri);

/**
 * Установить информацию о подключенных к устройству моторах. 
 */
void init_device_motors(stepper *sm_x, stepper *sm_y, stepper *sm_z);

/**************************************/
// Обработчики команд

/** 
 * Получить собственное имя устройства.
 */
int cmd_name(char* reply_buffer);

/** 
 * Получить модель устройства.
 */
int cmd_model(char* reply_buffer);

/** 
 * Получить серийный номер устройства.
 */
int cmd_serial_number(char* reply_buffer);

/** 
 * Получить словесное описание устройства. 
 */
int cmd_description(char* reply_buffer);

/** 
 * Получить версию прошивки устройства.
 */
int cmd_version(char* reply_buffer);

/** 
 * Получить производителя устройства.
 */
int cmd_manufacturer(char* reply_buffer);

/** 
 * Получить ссылку на страницу устройства.
 */
int cmd_uri(char* reply_buffer);

/** 
 * Проверить доступность устройства.
 */
int cmd_ping(char* reply_buffer);

/** 
 * Получить размер рабочей области.
 */
int cmd_rr_working_area_dim(char* reply_buffer);

/** 
 * Получить текущий статус устройства.
 */
int cmd_rr_status(char* reply_buffer);

/** 
 * Получить текущее положение печатающего блока.
 */
int cmd_rr_current_pos(char* reply_buffer);

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
 * Обработать входные данные - разобрать строку, выполнить одну или 
 * несколько команд. 
 * 
 * В случае выполнения нескольких команд, внутри последовательности 
 * отдельные команды разделяются точкой с запятой ';'.
 * Устройство выполняет их одну за одной, результаты выполнения 
 * сохраняются и возвращаются в одной строке в порядке выполнения, 
 * также разделенные точкой с запятой.
 *
 * Например:
 * Вход: name;ping;model
 * Результат: Anton's Rraptor;ok;Rraptor
 *
 * @buffer - входные данные, строка
 * @buffer_size - размер входных данных
 * @reply_buffer - ответ, строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
int handleInput(char* buffer, int buffer_size, char* reply_buffer);

#endif // RRAPTOR_PROTOCOL_H

