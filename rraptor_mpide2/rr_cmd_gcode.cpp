
#include "WProgram.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

#include "stepper.h"


extern stepper *_sm_x, *_sm_y, *_sm_z;
extern stepper* stepper_by_id(char id);


/** 
 * Команда G-code G0 - прямая линия.
 * 
 * @param motor_names имена моторов.
 * @param cvalues значения координат.
 * @param pcount количество параметров (моторов в списке).
 */
int cmd_gcode_g0(char motor_names[], double cvalues[], int pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_gcode_g01: ");
    
        for(int i = 0; i < pcount; i++) {
            Serial.print(motor_names[i]);
            Serial.print("=");
            Serial.print(cvalues[i], DEC);
            Serial.print("");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
        
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
            
            // команда выполнена (но работа моторов только началась!)
            strcpy(reply_buffer, REPLY_OK);
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
int cmd_gcode_g01(char motor_names[], double cvalues[], int pcount, double f, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
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
    #endif // DEBUG_SERIAL
        
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
            
            // команда выполнена (но работа моторов только началась!)
            strcpy(reply_buffer, REPLY_OK);
        } else {
            // ошибка - не нашли нужные моторы
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    return strlen(reply_buffer);
}

/** 
 * Команда G-code G02 - дуга по часовой стрелке.
 * синтаксис:
 *     G02 [Xfval Yfval] [Zfval] Rfval Ffval
 *     G02 [Xfval Yfval] [Zfval] Ifval Jfval Ffval
 *
 * @param pnames имена устанавливаемых параметров:
 *     X, Y - координаты точки-назначения (если не указаны, пройти полную окружность), мм
 *     Z - если указано, пройти по спирали, смещаясь по оси  Z
 *     R - радиус окружности (R>0 - проход по меньшей дуге<180гр, R<0 - проход по большей дуге>180гр), мм
 *     I, J - координаты центра окружностиv (если задан R, игнорируются), мм
 *     F - скорость перемещения, мм/с
 * @param pvalues значения параметров в виде строк
 * @param pcount количество параметров
 * @param reply_buffer ссылка на буфер для записи результата
 */
int cmd_gcode_g02(char pnames[], double pvalues[], int pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.println("cmd_gcode_g02");
    #endif // DEBUG_SERIAL
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        bool prepared = false;
        
        if(pcount >= 2) {
            // рисуем в плоскости X,Y
            stepper *sm1 = stepper_by_id('x');
            stepper *sm2 = stepper_by_id('y');

            double x;
            bool x_def = false;
            double y;
            bool y_def = false;
            double z;
            bool z_def = false;
            
            double r;
            bool r_def = false;
            
            double i;
            bool i_def = false;
            double j;
            bool j_def = false;
            
            double f;
            bool f_def = false;
            
            for(int ii = 0; ii < pcount; ii++) {
                if(pnames[ii] == 'X' || pnames[ii] == 'x') {
                    x = pvalues[ii];
                    x_def = true;
                } else if(pnames[ii] == 'Y' || pnames[ii] == 'y') {
                    y = pvalues[ii];
                    y_def = true;
                } else if(pnames[ii] == 'Z' || pnames[ii] == 'z') {
                    z = pvalues[ii];
                    z_def = true;
                } else if(pnames[ii] == 'R' || pnames[ii] == 'r') {
                    r = pvalues[ii];
                    r_def = true;
                } else if(pnames[ii] == 'I' || pnames[ii] == 'i') {
                    i = pvalues[ii];
                    i_def = true;
                } else if(pnames[ii] == 'J' || pnames[ii] == 'j') {
                    j = pvalues[ii];
                    j_def = true;
                } else if(pnames[ii] == 'F' || pnames[ii] == 'f') {
                    f = pvalues[ii];
                    f_def = true;
                }
            }
            // http://www.cnccookbook.com/CCCNCGCodeArcsG02G03.htm
            
            // без скорости никуда не поедем
            if(f_def) {
                 
                if(r_def && x_def && y_def) {
                    // если задан радиус и заданы координаты назначения, используем радиус
                    
                    prepare_arc(sm1, sm2, x, y, r, f);
                    prepared = true;
                } else if(i_def && j_def) {
                    // иначе используем координаты центра окружности
               
                    if(x_def && y_def) {
                       // идем в точку назначения
                       prepare_arc(sm1, sm2, x, y, i, j, f);
                       prepared = true;
                    } else {
                        // рисуем полную окружность
                       prepare_circle(sm1, sm2, i, j, f);
                       prepared = true;
                    }
                }// else {
                    // информации недостаточно, команда некорректна
                //}
                
                if(z_def) {
                    // у нас еще есть и Z, подготовим линейное движение и по ней
                    // (все вместе получится спуск/подъем по спирали)
                    
                    stepper *sm3 = stepper_by_id('z');
                    //prepare_line(sm3, z, f);
                }
            }
        }
        
        if(prepared) {
            // запустить шаги
            start_stepper_cycle();
            
            // команда выполнена (но работа моторов только началась!)
            strcpy(reply_buffer, REPLY_OK);
        } else {
            // ошибка - не нашли нужные моторы
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    
    return strlen(reply_buffer);
}

/** 
 * Команда G-code G03 - дуга против часовой стрелки.
 * синтаксис:
 *     G03 [Xfval Yfval] [Zfval] Rfval Ffval
 *     G03 [Xfval Yfval] [Zfval] Ifval Jfval Ffval
 *
 * @param pnames имена устанавливаемых параметров:
 *     X, Y - координаты точки-назначения (если не указаны, пройти полную окружность), мм
 *     Z - если указано, пройти по спирали, смещаясь по оси  Z
 *     R - радиус окружности (R>0 - проход по меньшей дуге<180гр, R<0 - проход по большей дуге>180гр), мм
 *     I, J - координаты центра окружностиv (если задан R, игнорируются), мм
 *     F - скорость перемещения, мм/с
 * @param pvalues значения параметров в виде строк
 * @param pcount количество параметров
 * @param reply_buffer ссылка на буфер для записи результата
 */
int cmd_gcode_g03(char pnames[], double pvalues[], int pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.println("cmd_gcode_g03");
    #endif // DEBUG_SERIAL
    
    return 0;
}


