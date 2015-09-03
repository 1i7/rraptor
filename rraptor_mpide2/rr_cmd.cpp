
#include "WProgram.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

#include "stepper.h"

static const char* device_name;
static const char* device_model;
static const char* device_serial_number;
static const char* device_description;
static const char* device_version;
static const char* device_manufacturer;
static const char* device_uri;

stepper *_sm_x, *_sm_y, *_sm_z;

/**
 * Получить шаговый двигатель по уникальному имени.
 *
 * @param id - имя мотора, состоит из одной буквы, регистр не учитывается.
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
 * Установить информацию об устройстве. 
 */
void init_device_info(const char* name, const char* model, const char* serial_number, 
        const char* description, const char* _version,
        const char* manufacturer, const char* uri) {
    device_name = name;
    device_model = model;
    device_serial_number = serial_number;
    device_description = description;
    device_version = _version;
    device_manufacturer = manufacturer;
    device_uri = uri;
}

/**
 * Установить информацию о подключенных к устройству моторах. 
 */
void init_device_motors(stepper *sm_x, stepper *sm_y, stepper *sm_z) {
    _sm_x = sm_x;
    _sm_y = sm_y;
    _sm_z = sm_z;
}

/** 
 * Вывести список команд.
 */
int cmd_help(char* reply_buffer) {
    // TODO: по умолчанию выводить только список команд без параметров,
    // описание команды и подробности выводить по help cmd_name
    
    sprintf(reply_buffer, "Rraptor commands: \n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_HELP);
    sprintf(reply_buffer+strlen(reply_buffer), "    List available commands\n");  
    
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_NAME);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device name\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_MODEL);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device model\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_SERIAL_NUMBER);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device serial number\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_DESCRIPTION);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device description\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_VERSION);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device version\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_MANUFACTURER);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device manufacturer\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_URI);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device url\n");
    
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_PING);
    sprintf(reply_buffer+strlen(reply_buffer), "    Check device online\n");
    
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_RR_WORKING_AREA_DIM);
    sprintf(reply_buffer+strlen(reply_buffer), "    Working area dimensions\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_RR_STATUS);
    sprintf(reply_buffer+strlen(reply_buffer), "    Device status\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_RR_CURRENT_POSITION);
    sprintf(reply_buffer+strlen(reply_buffer), "    Current position of the tooling\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_MOTOR_INFO, "motor_name [pulse_delay] [distance_per_step]\n        [min_end_strategy] [max_end_strategy]\n        [min_pos] [max_pos] [current_pos]");
    sprintf(reply_buffer+strlen(reply_buffer), "    Info about the motor\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_MOTOR_PIN_INFO, "motor_name");
    sprintf(reply_buffer+strlen(reply_buffer), "    Info about motor pinout\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_CONFIGURE_MOTOR, "motor_name [pulse_delay=us] [distance_per_step=um] \n        [min_end_strategy=CONST/AUTO/INF] [max_end_strategy=CONST/AUTO/INF] \n        [min_pos=um] [max_pos=um] [current_pos=um]");
    sprintf(reply_buffer+strlen(reply_buffer), "    Configure motor\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_CONFIGURE_MOTOR_PINS, "motor_name [pin_step=val] [pin_dir=val] [pin_en=val] [dir_inv=1/-1] \n        [pin_min=val] [pin_max=val]");
    sprintf(reply_buffer+strlen(reply_buffer), "    Configure motor pinout\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s\n", CMD_RR_STOP);
    sprintf(reply_buffer+strlen(reply_buffer), "    Stop motors\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_GO, "motor_name speed");
    sprintf(reply_buffer+strlen(reply_buffer), "    Start selected motor rotation\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_CALIBRATE, "motor_name speed");
    sprintf(reply_buffer+strlen(reply_buffer), "    Calibrate motor\n");
    
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_CONFIGURE_WIFI, "[ssid=val] [password=val] [static_ip=val]");
    sprintf(reply_buffer+strlen(reply_buffer), "    Configure wifi connection\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_RR_WIFI, "info/status/start/stop/restart");
    sprintf(reply_buffer+strlen(reply_buffer), "    Control wifi connection\n");
    
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_GCODE_G0, "[Xv1] [Yv] [Zv3]");
    sprintf(reply_buffer+strlen(reply_buffer), "    Move tooling to provided postion with maximum speed\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_GCODE_G01, "[Xv1] [Yv2] [Zv3] Fv4");
    sprintf(reply_buffer+strlen(reply_buffer), "    Draw line\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_GCODE_G02, "N/A");
    sprintf(reply_buffer+strlen(reply_buffer), "    Draw ark clockwise\n");
    sprintf(reply_buffer+strlen(reply_buffer), "%s %s\n", CMD_GCODE_G03, "N/A");
    sprintf(reply_buffer+strlen(reply_buffer), "    Draw ark counterclockwise\n");
        
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
 * Получить собственное имя устройства.
 */
int cmd_name(char* reply_buffer) {
    strcpy(reply_buffer, device_name);
    return strlen(reply_buffer);
}

/** 
 * Получить модель устройства.
 */
int cmd_model(char* reply_buffer) {
    strcpy(reply_buffer, device_model);
    return strlen(reply_buffer);
}

/** 
 * Получить серийный номер устройства.
 */
int cmd_serial_number(char* reply_buffer) {
    strcpy(reply_buffer, device_serial_number);
    return strlen(reply_buffer);
}

/** 
 * Получить словесное описание устройства. 
 */
int cmd_description(char* reply_buffer) {
    strcpy(reply_buffer, device_description);
    return strlen(reply_buffer);
}

/** 
 * Получить версию прошивки устройства.
 */
int cmd_version(char* reply_buffer) {
    strcpy(reply_buffer, device_version);
    return strlen(reply_buffer);
}

/** 
 * Получить производителя устройства.
 */
int cmd_manufacturer(char* reply_buffer) {
    strcpy(reply_buffer, device_manufacturer);
    return strlen(reply_buffer);
}

/** 
 * Получить ссылку на страницу устройства.
 */
int cmd_uri(char* reply_buffer) {
    strcpy(reply_buffer, device_uri);
    return strlen(reply_buffer);
}



/** 
 * Получить размер рабочей области в формате:
 * max_x max_y max_z
 */
int cmd_rr_working_area_dim(char* reply_buffer) {
    sprintf(reply_buffer, "%f %f %f", _sm_x->max_pos, _sm_y->max_pos, _sm_z->max_pos);
    
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
 * Получить текущее положение печатающего блока в формате:
 * x y z 
 */
int cmd_rr_current_position(char* reply_buffer) {
    sprintf(reply_buffer, "%f %f %f", _sm_x->current_pos, _sm_y->current_pos, _sm_z->current_pos);
    
    return strlen(reply_buffer);
}

/** 
 * Остановить все моторы.
 */
int cmd_rr_stop(char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.println("cmd_rr_stop");
    #endif // DEBUG_SERIAL
    
    finish_stepper_cycle();
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** 
 * Запустить мотор с заданной скоростью на непрерывное вращение.
 */
int cmd_rr_go(char motor_name, int spd, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_go: ");
        Serial.print(motor_name);
        Serial.print(", speed=");
        Serial.print(spd, DEC);
        Serial.println();
    #endif // DEBUG_SERIAL
        
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        
        if(sm != NULL) {
            int dir = spd >= 0 ? 1 : -1;
            int step_delay = spd >= 0 ? spd : -spd;
            
            // подготовить вращение
            prepare_whirl(sm, dir, 0, NONE);
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
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_calibrate: ");
        Serial.print(motor_name);
        Serial.print(", speed=");
        Serial.print(spd, DEC);
        Serial.println();
    #endif // DEBUG_SERIAL
        
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        if(sm != NULL) {
            int dir = spd > 0 ? 1 : -1;
            int step_delay = spd > 0 ? spd : -spd;
            
            // подготовить вращение
            prepare_whirl(sm, dir, 0, CALIBRATE_START_MIN_POS);
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
 * Информация о выбранном моторе.
 * На входе: список запрашиваемых параметров через пробел (не указывать, чтобы получить все параметры).
 * На выходе: список значений запрошенный параметров через пробел в 
 * порядке, указанном на входе.
 *
 * например:
 * запрос: x min_pos max_pos current_pos
 * ответ: 0 300000 12083
 *
 * @param motor_name имя мотора.
 * @param pnames имена запрашиваемых параметров:
 *     pulse_delay
 *     distance_per_step
 *     min_end_strategy
 *     max_end_strategy
 *     min_pos
 *     max_pos
 *     current_pos
 * @param pcount количество параметров (если 0, вывести все параметры).
 * @param reply_buffer ссылка на буфер для записи результата.
 */
int cmd_rr_motor_info(char motor_name, char* pnames[], int pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_motor_info: ");
        Serial.print(motor_name);
        Serial.print(" ");
        
        for(int i = 0; i < pcount; i++) {
            Serial.print(pnames[i]);
            Serial.print(" ");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
    
    stepper *sm = stepper_by_id(motor_name);
    if(sm != NULL) {
      
        // конвертировать min/max_end_strategy в строки
        const char *end_strategy_names[] = { "CONST", "AUTO", "INF" };
            
        if(pcount == 0) {    
            sprintf(reply_buffer, "pulse_delay=%d distance_per_step=%f min_end_strategy=%s max_end_strategy=%s min_pos=%f max_pos=%f current_pos=%f",
                sm->pulse_delay,
                sm->distance_per_step,
                end_strategy_names[sm->min_end_strategy],
                end_strategy_names[sm->max_end_strategy],
                sm->min_pos,
                sm->max_pos,
                sm->current_pos
            );
        } else {
            // запрос отдельных параметров
        
            // проверить корректность имен параметров 
            // (некорректные значения числовых параметров обнулятся при работе atoi и atof)
            bool params_valid = true;
            for(int i = 0; i < pcount && params_valid; i++) {
                if( strcmp(pnames[i], MOTOR_INFO_PARAM_PULSE_DELAY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_DISTANCE_PER_STEP) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_END_STRATEGY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_END_STRATEGY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_POS) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_POS) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_CURRENT_POS) != 0 ) {
                    
                    // неизвестное имя параметра
                    params_valid = false;
                }
            }
            
            // имена параметров верны
            if(params_valid) {
                for(int i = 0; i < pcount && params_valid; i++) {
                    if(i > 0) {
                        sprintf(reply_buffer+strlen(reply_buffer), " ");
                    }
                    
                    if( strcmp(pnames[i], MOTOR_INFO_PARAM_PULSE_DELAY) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "pulse_delay=%d", sm->pulse_delay);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_DISTANCE_PER_STEP) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "distance_per_step=%f", sm->distance_per_step);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_END_STRATEGY) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "min_end_strategy=%", end_strategy_names[sm->min_end_strategy]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_END_STRATEGY) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "max_end_strategy=%s", end_strategy_names[sm->max_end_strategy]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_POS) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "min_pos=%f", sm->min_pos);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_POS) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "max_pos=%f", sm->max_pos);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_CURRENT_POS) == 0 ) {
                        sprintf(reply_buffer+strlen(reply_buffer), "current_pos=%f", sm->current_pos);
                    }
                }
            } else {
                // ошибка - некорректные имена параметров
                strcpy(reply_buffer, REPLY_ERROR);
            }
        }
    } else {
        // ошибка - не нашли нужный мотор
        strcpy(reply_buffer, REPLY_ERROR);
    }
    
    return strlen(reply_buffer);
}

/**
 * Информация о подключении выбранного мотора: 
 * номера ножек драйвера step/dir и концевых датчиков.
 * 
 * Результат - строка в формате: 
 * имя_параметра1=значение_параметра1 [имя_параметра2=значение_параметра2]
 *
 * например:
 * pin_step=5 pin_dir=6 pin_en=7 dir_inv=-1 pin_min=1 pin_max=2
 *
 * @param motor_name имя мотора.
 * @param reply_buffer ссылка на буфер для записи результата.
 */
int cmd_rr_motor_pin_info(char motor_name, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_motor_pin_info: ");
        Serial.print(motor_name);
        Serial.println();
    #endif // DEBUG_SERIAL
    
    stepper *sm = stepper_by_id(motor_name);
    if(sm != NULL) {
        sprintf(reply_buffer, "pin_step=%d pin_dir=%d pin_en=%d dir_inv=%d pin_min=%d pin_max=%d", 
            sm->pin_step,
            sm->pin_dir,
            sm->pin_en,
            sm->dir_inv,
            sm->pin_min,
            sm->pin_max
        );
    } else {
        // ошибка - не нашли нужный мотор
        strcpy(reply_buffer, REPLY_ERROR);
    }
    
    return strlen(reply_buffer);
}

/**
 * Установить параметры для выбранного мотора.
 * 
 * На входе список параметров и значений в формате
 * имя_параметра1=значение_параметра1 [имя_параметра2=значение_параметра2]
 *
 * например:
 * min_pos=0 max_pos=200000
 *
 * @param motor_name имя мотора.
 * @param pnames имена устанавливаемых параметров:
 *     pulse_delay
 *     distance_per_step
 *     min_end_strategy
 *     max_end_strategy
 *     min_pos
 *     max_pos
 *     current_pos
 * @param pvalues значения параметров в виде строк
 * @param pcount количество параметров
 * @param reply_buffer ссылка на буфер для записи результата.
 */
int cmd_rr_configure_motor(char motor_name, char* pnames[], char* pvalues[], int  pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_configure_motor: ");
        Serial.print(motor_name);
        Serial.print(" ");
        
        for(int i = 0; i < pcount; i++) {
            Serial.print(pnames[i]);
            Serial.print("=");
            Serial.print(pvalues[i]);
            Serial.print(" ");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
        
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        
        if(sm != NULL) {
            // мотор нашелся, проверить корректность имен параметров 
            // (некорректные значения числовых параметров обнулятся при работе atoi и atof)
            bool params_valid = true;
            for(int i = 0; i < pcount && params_valid; i++) {
                if( strcmp(pnames[i], MOTOR_INFO_PARAM_PULSE_DELAY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_DISTANCE_PER_STEP) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_END_STRATEGY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_END_STRATEGY) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_POS) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_POS) != 0 &&
                    strcmp(pnames[i], MOTOR_INFO_PARAM_CURRENT_POS) != 0 ) {
                    
                    // неизвестное имя параметра
                    params_valid = false;
                }
            }
            
            // имена параметров верны
            if(params_valid) {
                // установить нужные значения, некорректные значения числовых параметров 
                // обнулятся при работе atoi и atof
                for(int i = 0; i < pcount && params_valid; i++) {
              
                    if( strcmp(pnames[i], MOTOR_INFO_PARAM_PULSE_DELAY) == 0 ) {
                        sm->pulse_delay = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_DISTANCE_PER_STEP) == 0 ) {
                        sm->distance_per_step = atof(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_END_STRATEGY) == 0 ) {
                        // найти значение min/max_end_strategy из строки
                        if( strcmp(pvalues[i], "CONST") == 0 ) {
                            sm->min_end_strategy = CONST;
                        } else if( strcmp(pvalues[i], "AUTO") == 0 ) {
                            sm->min_end_strategy = AUTO;
                        } else if( strcmp(pvalues[i], "INF") == 0 ) {
                            sm->min_end_strategy = INF;
                        }
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_END_STRATEGY) == 0 ) {
                        // найти значение min/max_end_strategy из строки
                        if( strcmp(pvalues[i], "CONST") == 0 ) {
                            sm->max_end_strategy = CONST;
                        } else if( strcmp(pvalues[i], "AUTO") == 0 ) {
                            sm->max_end_strategy = AUTO;
                        } else if( strcmp(pvalues[i], "INF") == 0 ) {
                            sm->max_end_strategy = INF;
                        }
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MIN_POS) == 0 ) {
                        sm->min_pos = atof(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_MAX_POS) == 0 ) {
                        sm->max_pos = atof(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_INFO_PARAM_CURRENT_POS) == 0 ) {
                        sm->current_pos = atof(pvalues[i]);
                    }
                }
                
                // команда выполнена
                strcpy(reply_buffer, REPLY_OK);
            } else {
                // ошибка - некорректные имена параметров
                strcpy(reply_buffer, REPLY_ERROR);
            }
        } else {
            // ошибка - не нашли нужный мотор
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    
    return strlen(reply_buffer);  
}

/**
 * Установить информацию о подключении выбранного мотора: 
 * номера ножек драйвера step/dir и концевых датчиков.
 * 
 * На входе список параметров и значений в формате: 
 * имя_параметра1=значение_параметра1 [имя_параметра2=значение_параметра2]
 *
 * например:
 * pin_step=5 pin_dir=6 pin_en=7 dir_inv=-1 pin_min=1 pin_max=2
 *
 * @param motor_name имя мотора.
 * @param pnames имена устанавливаемых параметров:
 *     pin_step
 *     pin_dir
 *     pin_en
 *     dir_inv
 *     pin_min
 *     pin_max
 * @param pvalues значения параметров в виде строк
 * @param pcount количество параметров
 * @param reply_buffer ссылка на буфер для записи результата.
 */
int cmd_rr_configure_motor_pins(char motor_name, char* pnames[], char* pvalues[], int  pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_configure_motor_pins: ");
        Serial.print(motor_name);
        Serial.print(" ");
        
        for(int i = 0; i < pcount; i++) {
            Serial.print(pnames[i]);
            Serial.print("=");
            Serial.print(pvalues[i]);
            Serial.print(" ");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
    
    if(is_cycle_running()) {
        // устройство занято
        strcpy(reply_buffer, REPLY_BUSY);
    } else {
        stepper *sm = stepper_by_id(motor_name);
        
        if(sm != NULL) {
            // мотор нашелся, проверить корректность имен параметров 
            // (некорректные значения числовых параметров обнулятся при работе atoi и atof)
            bool params_valid = true;
            for(int i = 0; i < pcount && params_valid; i++) {
                if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_STEP) != 0 &&
                    strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_DIR) != 0 &&
                    strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_EN) != 0 &&
                    strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_DIR_INV) != 0 &&
                    strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MIN) != 0 &&
                    strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MAX) != 0 ) {
                    
                    // неизвестное имя параметра
                    params_valid = false;
                }
            }
            
            // имена параметров верны
            if(params_valid) {
                // установить нужные значения, некорректные значения числовых параметров 
                // обнулятся при работе atoi и atof
                for(int i = 0; i < pcount && params_valid; i++) {
              
                    if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_STEP) == 0 ) {
                        sm->pin_step = atoi(pvalues[i]);
                        if(sm->pin_step != -1) {
                            pinMode(sm->pin_step, OUTPUT);
                        }
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_DIR) == 0 ) {
                        sm->pin_dir = atoi(pvalues[i]);
                        if(sm->pin_dir != -1) {
                            pinMode(sm->pin_dir, OUTPUT);
                        }
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_EN) == 0 ) {
                        sm->pin_en = atoi(pvalues[i]);
                        if(sm->pin_en != -1) {
                            pinMode(sm->pin_en, OUTPUT);
                        }
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_DIR_INV) == 0 ) {
                        sm->dir_inv = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MIN) == 0 ) {
                        sm->pin_min = atoi(pvalues[i]);
                        if(sm->pin_min != -1) {
                            pinMode(sm->pin_min, INPUT);
                        }
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MAX) == 0 ) {
                        sm->pin_max = atoi(pvalues[i]);
                        if(sm->pin_max != -1) {
                            pinMode(sm->pin_max, INPUT);
                        }
                    }
                }
                
                // команда выполнена
                strcpy(reply_buffer, REPLY_OK);
            } else {
                // ошибка - некорректные имена параметров
                strcpy(reply_buffer, REPLY_ERROR);
            }
        } else {
            // ошибка - не нашли нужный мотор
            strcpy(reply_buffer, REPLY_ERROR);
        }
    }
    
    return strlen(reply_buffer);
}

