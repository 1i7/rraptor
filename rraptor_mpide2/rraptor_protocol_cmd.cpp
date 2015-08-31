
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

static stepper *_sm_x, *_sm_y, *_sm_z;

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
 * Получить шаговый двигатель по уникальному имени.
 *
 * @param id - имя мотора, состоит из одной буквы, регистр не учитывается.
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
 * Вывести список команд.
 */
int cmd_help(char* reply_buffer) {
    sprintf(reply_buffer, "Rraptor commands: \n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_HELP); strcat(reply_buffer, "\n"); 
    
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_NAME); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_MODEL); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_SERIAL_NUMBER); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_DESCRIPTION); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_VERSION); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_MANUFACTURER); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_URI); strcat(reply_buffer, "\n");
    
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_PING); strcat(reply_buffer, "\n");
    
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_WORKING_AREA_DIM); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_STATUS); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_CURRENT_POSITION); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_MOTOR_INFO); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_MOTOR_PIN_INFO); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_CONFIGURE_MOTOR); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_CONFIGURE_MOTOR_PINS); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_STOP); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_GO); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_CALIBRATE); strcat(reply_buffer, "\n");
    
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_CONFIGURE_WIFI); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_RR_WIFI); strcat(reply_buffer, "\n");
    
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_GCODE_G0); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_GCODE_G01); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_GCODE_G02); strcat(reply_buffer, "\n");
    strcat(reply_buffer, "    "); strcat(reply_buffer, CMD_GCODE_G03); strcat(reply_buffer, "\n");
        
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
            int dir = spd > 0 ? 1 : -1;
            int step_delay = spd > 0 ? spd : -spd;
            
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
        if(pcount == 0) {
            // конвертировать min/max_end_strategy в строки
            const char *end_strategy_names[] = { "CONST", "AUTO", "INF" };
            
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
            // TODO: реализовать запрос отдельных параметров
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
                    } else {
                        // неизвестное имя параметра
                        params_valid = false;
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
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_DIR) == 0 ) {
                        sm->pin_dir = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_EN) == 0 ) {
                        sm->pin_en = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_DIR_INV) == 0 ) {
                        sm->dir_inv = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MIN) == 0 ) {
                        sm->pin_min = atoi(pvalues[i]);
                    } else if( strcmp(pnames[i], MOTOR_PIN_INFO_PARAM_PIN_MAX) == 0 ) {
                        sm->pin_max = atoi(pvalues[i]);
                    } else {
                        // неизвестное имя параметра
                        params_valid = false;
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
 * Задать настройки подключения Wifi.
 * На входе список параметров и значений в формате: 
 * имя_параметра1=значение_параметра1 [имя_параметра2=значение_параметра2]
 */
int cmd_rr_configure_wifi(char* pnames[], char* pvalues[], int  pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_configure_wifi: ");
        
        for(int i = 0; i < pcount; i++) {
            Serial.print(pnames[i]);
            Serial.print("=");
            Serial.print(pvalues[i]);
            Serial.print(" ");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
    
}

/** 
 * Управление подключением Wifi
 * 
 * @param wifi_cmd дополнительная операция
 *     status: вывести текущий статус подключения
 *     start: подключиться к Wifi
 *     stop:  отключиться от Wifi
 *     restart: перезапустить подключение Wifi
 */
int cmd_rr_wifi(char* wifi_cmd, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_wifi: ");
        Serial.print(wifi_cmd);
        Serial.println();
    #endif // DEBUG_SERIAL
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
    #ifdef DEBUG_SERIAL
        Serial.println("cmd_gcode_g02");
    #endif // DEBUG_SERIAL
}

/** 
 * Команда G-code G03 - дуга против часовой стрелки.
 */
void cmd_gcode_g03() {
    #ifdef DEBUG_SERIAL
        Serial.println("cmd_gcode_g03");
    #endif // DEBUG_SERIAL
}

