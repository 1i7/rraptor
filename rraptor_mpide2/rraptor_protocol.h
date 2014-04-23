#ifndef RRAPTOR_PROTOCOL_H
#define RRAPTOR_PROTOCOL_H

// Протокол общения с Пультом управления

// Команды Rraptor

/* Остановить все моторы */
static char* CMD_RR_STOP = "rr_stop";
/* Повернуть заданный мотор на заданное количество шагов */
static char* CMD_RR_STEP = "rr_step";
/* Сдвинуть заданную координату на заданное расстояние */
static char* CMD_RR_SHIFT = "rr_shift";
/* Переместить заданную координату в заданное положение */
static char* CMD_RR_MOVE = "rr_move";
/* Запустить мотор с заданной скоростью на непрерывное вращение */
static char* CMD_RR_GO = "rr_go";
/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
static char* CMD_RR_CALIBRATE = "rr_calibrate";
  
// Команды G-кода

/* Команда G-code G01 - прямая линия */
static char* CMD_GCODE_G01 = "G01";
/* Команда G-code G02 - дуга по часовой стрелке */
static char* CMD_GCODE_G02 = "G02";
/* Команда G-code G03 - дуга против часовой стрелки */
static char* CMD_GCODE_G03 = "G03";

// Ответы
static char*  REPLY_OK = "ok";
static char*  REPLY_DONTUNDERSTAND = "dontunderstand";

// Обработчики команд

/** 
 * Остановить все моторы.
 */
void cmd_rr_stop();

/** 
 * Повернуть заданный мотор на заданное количество шагов.
 */
void cmd_rr_step(char* motor_name, int cnum, int cdelay=0);

/** 
 * Сдвинуть заданную координату на заданное расстояние.
 */
void cmd_rr_shift(char* motor_name, int dl, int dt=0);

/**
 * Переместить заданную координату в заданное положение.
 */
void cmd_rr_move(char* motor_name, int pos);

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
void cmd_rr_go(char* motor_name, int spd);

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
void cmd_rr_calibrate(char* motor_name, int spd);

/** 
 * Команда G-code G01 - прямая линия.
 */
void cmd_gcode_g01(int x, int y, int z, int f);

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
int handleInput(char* buffer, int size, char* reply_buffer);

#endif // RRAPTOR_PROTOCOL_H

