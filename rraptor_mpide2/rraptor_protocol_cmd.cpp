
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
stepper* stepper_by_id(char id) {
    if(id == 'x' || id == 'X') {
        return _sm_x;
    } else if(id == 'y' || id == 'Y') {
        return _sm_y;
    } else if(id == 'z' || id == 'Z') {
        return _sm_z;
    } else {
        return NULL;
    }
}

/** 
 * Получить текущий статус системы.
 */
int cmd_rr_status(char* reply_buffer) {
    if(is_cycle_running()) {
        strcpy(reply_buffer, STATUS_WORKING);
    } else {
        strcpy(reply_buffer, STATUS_IDLE);
    }
    return strlen(reply_buffer);
}

/** 
 * Остановить все моторы.
 */
void cmd_rr_stop() {
    Serial.println("cmd_rr_stop");
    
    finish_stepper_cycle();
}

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
void cmd_rr_go(char motor_name, int spd) {
    Serial.print("cmd_rr_go: ");
    Serial.print(motor_name);
    Serial.print(", speed=");
    Serial.print(spd, DEC);
    Serial.println();
    
    stepper *sm = stepper_by_id(motor_name);
    
    if(sm != NULL) {
        int dir = spd > 0 ? 1 : -1;
        int step_delay = spd > 0 ? spd : -spd;
        
        prepare_whirl(sm, dir, 0, true);
        start_stepper_cycle();
    }
}

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
void cmd_rr_calibrate(char motor_name, int spd) {
    Serial.print("cmd_rr_calibrate: ");
    Serial.print(motor_name);
    Serial.print(", speed=");
    Serial.print(spd, DEC);
    Serial.println();
    
    stepper *sm = stepper_by_id(motor_name);
    if(sm != NULL) {
        int dir = spd > 0 ? 1 : -1;
        int step_delay = spd > 0 ? spd : -spd;
        
        prepare_whirl(sm, dir, 0, false);
        start_stepper_cycle();
    }
}

/** 
 * Команда G-code G0 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 */
void cmd_gcode_g0(char motor_names[], double cvalues[], int  pcount) {
    Serial.print("cmd_gcode_g01: ");
    
    for(int i = 0; i < pcount; i++) {
        Serial.print(motor_names[i]);
        Serial.print("=");
        Serial.print(cvalues[i], DEC);
        Serial.print("");
    }
    Serial.println();
    
    bool prepared = false;
    
    if(pcount == 1) {
        stepper *sm = stepper_by_id(motor_names[0]);
        if(sm != NULL) {
            prepare_line(sm, cvalues[0], 0);
            prepared = true;
        }
    } else if(pcount >= 2) {
        stepper *sm1 = stepper_by_id(motor_names[0]);
        stepper *sm2 = stepper_by_id(motor_names[1]);
        if(sm1 != NULL && sm2 != NULL) {
            prepare_line_2d(sm1, sm2, cvalues[0], cvalues[1], 0);
            prepared = true;
        }
    }
    
    if(prepared) {
        start_stepper_cycle();
                
        // Заблокируем на время рисования (TODO: убрать)
        while(is_cycle_running()) {
            Serial.print("X.pos=");
            Serial.print(_sm_x->current_pos, DEC);
            Serial.print(", Y.pos=");
            Serial.print(_sm_y->current_pos, DEC);
            Serial.print(", Z.pos=");
            Serial.print(_sm_z->current_pos, DEC);
            Serial.println();
            
            delay(1000);
        }
    }
}

/** 
 * Команда G-code G01 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 * @param f скорость перемещения мм/с.
 */
void cmd_gcode_g01(char motor_names[], double cvalues[], int  pcount, double f) {
    Serial.print("cmd_gcode_g01: ");
    
    for(int i = 0; i < pcount; i++) {
        Serial.print(motor_names[i]);
        Serial.print("=");
        Serial.print(cvalues[i], DEC);
        Serial.print(", ");
    }
    Serial.print("F=");
    Serial.print(f, DEC);
    Serial.println();
    
    bool prepared = false;
    
    if(pcount == 1) {
        stepper *sm = stepper_by_id(motor_names[0]);
        if(sm != NULL) {
            prepare_line(sm, cvalues[0], f);
            prepared = true;
        }
    } else if(pcount >= 2) {
        stepper *sm1 = stepper_by_id(motor_names[0]);
        stepper *sm2 = stepper_by_id(motor_names[1]);
        if(sm1 != NULL && sm2 != NULL) {
            prepare_line_2d(sm1, sm2, cvalues[0], cvalues[1], f);
            prepared = true;
        }
    }
    
    if(prepared) {
        start_stepper_cycle();
                
        // Заблокируем на время рисования (TODO: убрать)
        while(is_cycle_running()) {
            Serial.print("X.pos=");
            Serial.print(_sm_x->current_pos, DEC);
            Serial.print(", Y.pos=");
            Serial.print(_sm_y->current_pos, DEC);
            Serial.print(", Z.pos=");
            Serial.print(_sm_z->current_pos, DEC);
            Serial.println();
            
            delay(1000);
        }
    }
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

