
#include "WProgram.h"

#include "rraptor_protocol.h"
#include "stepper.h"


static stepper *_sm_x, *_sm_y, *_sm_z;

void init_protocol(stepper *sm_x, stepper *sm_y, stepper *sm_z) {
    _sm_x = sm_x;
    _sm_y = sm_y;
    _sm_z = sm_z;
}

/**
 * Получить шаговый двигатель по уникальному имени.
 */
static stepper* stepper_by_id(char id) {
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
 * Получить собственное имя устройства.
 */
int cmd_name(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement 'name'");
    return strlen(reply_buffer);
}

/** 
 * Получить модель устройства.
 */
int cmd_model(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement 'model'");
    return strlen(reply_buffer);
}

/** 
 * Получить серийный номер устройства.
 */
int cmd_serial_number(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement 'sn'");
    return strlen(reply_buffer);
}

/** 
 * Получить словесное описание устройства. 
 */
int cmd_description(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement 'description'");
    return strlen(reply_buffer);
}

/** 
 * Получить версию прошивки устройства.
 */
int cmd_version(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement 'version'");
    return strlen(reply_buffer);
}

/** 
 * Проверить доступность устройства.
 */
int cmd_ping(char* reply_buffer) {
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** 
 * Получить размер рабочей области.
 */
int cmd_rr_working_area_dim(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement rr_working_area_dim");
    return strlen(reply_buffer);
}

/** 
 * Получить текущий статус устройства.
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
 * Получить текущее положение печатающего блока 
 */
int cmd_rr_current_pos(char* reply_buffer) {
    strcpy(reply_buffer, "todo: implement rr_current_pos");
    return strlen(reply_buffer);
}

/** 
 * Остановить все моторы.
 */
int cmd_rr_stop(char* reply_buffer) {
    Serial.println("cmd_rr_stop");
    
    finish_stepper_cycle();
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
int cmd_rr_go(char motor_name, int spd, char* reply_buffer) {
    Serial.print("cmd_rr_go: ");
    Serial.print(motor_name);
    Serial.print(", speed=");
    Serial.print(spd, DEC);
    Serial.println();
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        
        if(sm != NULL) {
            int dir = spd > 0 ? 1 : -1;
            int step_delay = spd > 0 ? spd : -spd;
            
            // подготовить вращение
            prepare_whirl(sm, dir, 0, true);
            // запустить шаги
            start_stepper_cycle();
            
            // команда выполнена
            strcpy(reply_buffer, REPLY_OK);
        } else {
            // ошибка - не нашли нужный мотор
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    return strlen(reply_buffer);
}

/** 
 * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
 * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
 */
int cmd_rr_calibrate(char motor_name, int spd, char* reply_buffer) {
    Serial.print("cmd_rr_calibrate: ");
    Serial.print(motor_name);
    Serial.print(", speed=");
    Serial.print(spd, DEC);
    Serial.println();
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        if(sm != NULL) {
            int dir = spd > 0 ? 1 : -1;
            int step_delay = spd > 0 ? spd : -spd;
            
            // подготовить вращение
            prepare_whirl(sm, dir, 0, false);
            // запустить шаги
            start_stepper_cycle();
            
            // команда выполнена
            strcpy(reply_buffer, REPLY_OK);
        } else {
            // ошибка - не нашли нужный мотор
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    return strlen(reply_buffer);
}

//static int prevTime1 = 0;

/** 
 * Команда G-code G0 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 */
int cmd_gcode_g0(char motor_names[], double cvalues[], int  pcount, char* reply_buffer) {
    Serial.print("cmd_gcode_g01: ");
    
    for(int i = 0; i < pcount; i++) {
        Serial.print(motor_names[i]);
        Serial.print("=");
        Serial.print(cvalues[i], DEC);
        Serial.print("");
    }
    Serial.println();
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        // Подготовить шаги для моторов
        bool prepared = false;
        
        if(pcount == 1) {
            // один мотор - линия по одной координате
            // с максимальной скоростью
            stepper *sm = stepper_by_id(motor_names[0]);
            if(sm != NULL) {
                prepare_line(sm, cvalues[0], 0);
                prepared = true;
            }
        } else if(pcount >= 2) {
            // два мотора - линия по плоскости в 2х координатах
            // с максимальной скоростью
            stepper *sm1 = stepper_by_id(motor_names[0]);
            stepper *sm2 = stepper_by_id(motor_names[1]);
            if(sm1 != NULL && sm2 != NULL) {
                prepare_line_2d(sm1, sm2, cvalues[0], cvalues[1], 0);
                prepared = true;
            }
        }
        
        if(prepared) {
            // запустить шаги
            start_stepper_cycle();
            
            // команда выполнена
            strcpy(reply_buffer, REPLY_OK);
                    
            // Заблокируем на время рисования (TODO: убрать)
//            int currTime = millis();
//            while(is_cycle_running()) {
//                if( (currTime - prevTime1) >= 1000) {
//                    prevTime1 = currTime;
//                    Serial.print("X.pos=");
//                    Serial.print(_sm_x->current_pos, DEC);
//                    Serial.print(", Y.pos=");
//                    Serial.print(_sm_y->current_pos, DEC);
//                    Serial.print(", Z.pos=");
//                    Serial.print(_sm_z->current_pos, DEC);
//                    Serial.println();
//                }
//            }
        } else {
            // ошибка - не нашли нужные моторы
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    return strlen(reply_buffer);
}

/** 
 * Команда G-code G01 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 * @param f скорость перемещения мм/с.
 */
int cmd_gcode_g01(char motor_names[], double cvalues[], int  pcount, double f, char* reply_buffer) {
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
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
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
            // запустить шаги
            start_stepper_cycle();
            
            // команда выполнена
            strcpy(reply_buffer, REPLY_OK);
                    
            // Заблокируем на время рисования (TODO: убрать)
//            int currTime = millis();
//            while(is_cycle_running()) {
//                if( (currTime - prevTime1) >= 1000) {
//                    prevTime1 = currTime;
//                    Serial.print("X.pos=");
//                    Serial.print(_sm_x->current_pos, DEC);
//                    Serial.print(", Y.pos=");
//                    Serial.print(_sm_y->current_pos, DEC);
//                    Serial.print(", Z.pos=");
//                    Serial.print(_sm_z->current_pos, DEC);
//                    Serial.println();
//                }
//            }
        } else {
            // ошибка - не нашли нужные моторы
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    return strlen(reply_buffer);
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

