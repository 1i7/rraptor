
#include "WProgram.h"

#include "rraptor_protocol.h"

/** 
 * Остановить все моторы.
 */
void cmd_rr_stop() {
    Serial.println("cmd_rr_stop");
}

/** 
 * Повернуть заданный мотор на заданное количество шагов.
 */
void cmd_rr_step(char* motor_name, int cnum, int cdelay) {
    Serial.println("cmd_rr_step");
}

/** 
 * Сдвинуть заданную координату на заданное расстояние.
 */
void cmd_rr_shift(char* motor_name, int dl, int dt) {
    Serial.println("cmd_rr_shift");
}

/**
 * Переместить заданную координату в заданное положение.
 */
void cmd_rr_move(char* motor_name, int pos) {
    Serial.println("cmd_rr_move");
}

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
void cmd_rr_go(char* motor_name, int spd) {
    Serial.println("cmd_rr_stop");
}

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
void cmd_rr_calibrate(char* motor_name, int spd) {
    Serial.println("cmd_rr_calibrate");
}

/** 
 * Команда G-code G01 - прямая линия.
 */
void cmd_gcode_g01(int x, int y, int z, int f) {
    Serial.println("cmd_gcode_g01");
}

/** 
 * Команда G-code G02 - дуга по часовой стрелке.
 */
void cmd_gcode_g02() {
    Serial.println("cmd_gcode_g02");
}

/** 
 * Команда G-code G03 - дуга против часовой стрелки.
 */
void cmd_gcode_g03() {
    Serial.println("cmd_gcode_g03");
}

