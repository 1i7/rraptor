
#include "WProgram.h"

#include "rraptor_protocol.h"
#include "stepper.h"


stepper *_sm_x, *_sm_y, *_sm_z;

void init_protocol(stepper *sm_x, stepper *sm_y, stepper *sm_z) {
    _sm_x = sm_x;
    _sm_y = sm_y;
    _sm_z = sm_z;
}

/**
 * Получить шаговый двигатель по уникальному имени.
 */
stepper* stepper_by_id(char* id) {
  if(strcmp("x", id) == 0) {
    return _sm_x;
  } else if(strcmp("y", id) == 0) {
    return _sm_y;
  } else if(strcmp("z", id) == 0) {
    return _sm_z;
  } else {
    return 0;
  }
}

/** 
 * Остановить все моторы.
 */
void cmd_rr_stop() {
    Serial.println("cmd_rr_stop");
    
    finish_stepper_cycle();
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
    
    stepper *sm = stepper_by_id(motor_name);
    int dir = spd > 0 ? 1 : -1;
    int step_delay = spd > 0 ? spd : -spd;
    
    prepare_whirl(sm, dir, 0, true);
    start_stepper_cycle();
}

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
void cmd_rr_calibrate(char* motor_name, int spd) {
    Serial.println("cmd_rr_calibrate");
    
    stepper *sm = stepper_by_id(motor_name);
    int dir = spd > 0 ? 1 : -1;
    int step_delay = spd > 0 ? spd : -spd;
    
    prepare_whirl(sm, dir, 0, false);
    start_stepper_cycle();
}

/** 
 * Команда G-code G01 - прямая линия.
 */
void cmd_gcode_g01(int x, int y, int z, int f) {
    Serial.println("cmd_gcode_g01");
    delay(2000);
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

