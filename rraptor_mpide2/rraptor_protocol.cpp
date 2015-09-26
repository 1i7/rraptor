#include "WProgram.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

// Размеры буферов для чтения команд и записи ответов 
#ifndef CMD_READ_BUFFER_SIZE
#define CMD_READ_BUFFER_SIZE 128
#endif

#ifndef CMD_WRITE_BUFFER_SIZE
#define CMD_WRITE_BUFFER_SIZE 512
#endif

/**
 * Разбить параметр в формате "имя=значение" на отдельные составляющие "имя" и "значение".
 * 
 * @param param исходный параметр в формате "имя=значение"
 * @param pname имя параметра или NULL
 * @param pvalue значение параметра или NULL
 */
static void parseParam(char* param, char** pname, char** pvalue) {
    // Разобьем команду на куски по пробелам
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    // имя параметра или NULL
    *pname = strtok_r(param, "=", &last);
    // значение параметра или NULL
    *pvalue = strtok_r(NULL, "=", &last);
}

/**
 * Разбить параметр команды G-кода на составляющие:
 * 1й символ - имя параметра,
 * подстрока начиная со 2го символа - значение параметра, число с плавающей точкой.
 * 
 * @param param исходный параметр
 * @param pname указатель на символ - в него будет записано имя параметра
 * @param pvalue указатель на число с плавающей точкой - в него будет записано значение параметра
 */
static void parseGcodeParam(char* param, char* pname, double* pvalue) {
    if(strlen(param) > 1) {
        *pname = param[0];
        *pvalue = atof(param + 1);
    }
}

/**
 * Распознать и выполнить единственную команду.
 */
static int handleCommand(char* buffer, char* reply_buffer) {    
    // по умолчанию обнулим ответ
    reply_buffer[0] = 0;
    
    bool success = false;
    
    int max_tokens = 20;
    char* tokens[max_tokens];
    int tokensNum = 0;
    
    // Разобьем команду на куски по пробелам
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    token = strtok_r(buffer, " ", &last);
    while(token != NULL) {
        tokens[tokensNum] = token;
        tokensNum++;
        
        token = strtok_r(NULL, " ", &last);
    }
    
    // Определим, с какой командой имеем дело    
    if(tokensNum > 0) {
        if(strcmp(tokens[0], CMD_HELP) == 0) {
            // синтаксис:
            //     help
            
            // Команда корректна
            success = true;
            
            // Выполнить команду
            cmd_help(reply_buffer);
        } else if(strcmp(tokens[0], CMD_PING) == 0) {
            // синтаксис:
            //     ping
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_ping(reply_buffer);
        } else if(strcmp(tokens[0], CMD_NAME) == 0) {
            // синтаксис:
            //     name
            
            // Команда корректна
            success = true;
            
            // Выполнить команду
            cmd_name(reply_buffer);
        } else if(strcmp(tokens[0], CMD_MODEL) == 0) {
            // синтаксис:
            //     model
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_model(reply_buffer);
        } else if(strcmp(tokens[0], CMD_SERIAL_NUMBER) == 0) {
            // синтаксис:
            //     sn
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_serial_number(reply_buffer);
        } else if(strcmp(tokens[0], CMD_DESCRIPTION) == 0) {
            // синтаксис:
            //     description
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_description(reply_buffer);
        } else if(strcmp(tokens[0], CMD_VERSION) == 0) {
            // синтаксис:
            //     version
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_version(reply_buffer);
        } else if(strcmp(tokens[0], CMD_MANUFACTURER) == 0) {
            // синтаксис:
            //     manufacturer
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_manufacturer(reply_buffer);
        } else if(strcmp(tokens[0], CMD_URI) == 0) {
            // синтаксис:
            //     uri
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_uri(reply_buffer);
        } else if(strcmp(tokens[0], CMD_RR_WORKING_AREA_DIM) == 0) {
            // синтаксис:
            //     rr_working_area_dim
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_rr_working_area_dim(reply_buffer);
        } else if(strcmp(tokens[0], CMD_RR_STATUS) == 0) {
            // синтаксис:
            //     rr_status
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_rr_status(reply_buffer);
        } else if(strcmp(tokens[0], CMD_RR_CURRENT_POSITION) == 0) {
            // синтаксис:
            //     rr_current_pos
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_rr_current_position(reply_buffer);
        } else if(strcmp(tokens[0], CMD_RR_STOP) == 0) {
            // синтаксис:
            //     rr_stop
            
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_rr_stop(reply_buffer);
        } else if(strcmp(tokens[0], CMD_RR_GO) == 0) {
            // синтаксис:
            //     rr_go motor_name speed
            if(tokensNum >= 3) {
                char motor_name = tokens[1][0];
                int spd = atoi(tokens[2]);
        
                // Команда корректна
                success = true;
                  
                // Выполнить команду           
                cmd_rr_go(motor_name, spd, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_CALIBRATE) == 0) {
            // синтаксис:
            //     rr_calibrate motor_name speed
            if(tokensNum >= 3) {
                char motor_name = tokens[1][0];
                int spd = atoi(tokens[2]);
        
                // Команда корректна
                success = true;
                  
                // Выполнить команду
                cmd_rr_calibrate(motor_name, spd, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_MOTOR_INFO) == 0) {
            // синтаксис:
            //     rr_motor_info motor_name [pulse_delay] [distance_per_step] [min_end_strategy] [max_end_strategy] [min_pos] [max_pos] [current_pos]
            if(tokensNum >= 2) {
                char motor_name = tokens[1][0];
                
                int max_params = 7;
                char* pnames[max_params];
                int pcount = 0;
                
                for(int i = 2; i < tokensNum; i++) {
                    if(pcount < max_params) {
                        pnames[pcount] = tokens[i];
                        pcount++;
                    }
                }
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_motor_info(motor_name, pnames, pcount, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_MOTOR_PIN_INFO) == 0) {
            // синтаксис:
            //     rr_motor_pin_info motor_name
            if(tokensNum >= 2) {
                char motor_name = tokens[1][0];
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_motor_pin_info(motor_name, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_CONFIGURE_MOTOR) == 0) {
            // синтаксис:
            //     rr_configure_motor motor_name [pulse_delay=val_pd] [distance_per_step=val_dps] \
            //                                   [min_end_strategy=val_mes] [max_end_strategy=val_Mes] \
            //                                   [min_pos=val_mp] [max_pos=val_Mp] [current_pos=val_cp]
            if(tokensNum >= 3) {
                char motor_name = tokens[1][0];
                
                int max_params = 7;
                char* pnames[max_params];
                char* pvalues[max_params];
                int pcount = 0;
                
                char* pname;
                char* pvalue;
                for(int i = 2; i < tokensNum; i++) {                
                    parseParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < max_params) {
                        pnames[pcount] = pname;
                        pvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_configure_motor(motor_name, pnames, pvalues, pcount, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_CONFIGURE_MOTOR_PINS) == 0) {
            // синтаксис:
            //     rr_configure_motor_pins motor_name [pin_step=val] [pin_dir=val] [pin_en=val] [dir_inv=val] \
            //                                   [pin_min=val] [pin_max=val]
            if(tokensNum >= 3) {
                char motor_name = tokens[1][0];
                
                int max_params = 6;
                char* pnames[max_params];
                char* pvalues[max_params];
                int pcount = 0;
                
                char* pname;
                char* pvalue;
                for(int i = 2; i < tokensNum; i++) {                
                    parseParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < max_params) {
                        pnames[pcount] = pname;
                        pvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_configure_motor_pins(motor_name, pnames, pvalues, pcount, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_CONFIGURE_WIFI) == 0) {
            // синтаксис:
            //     rr_configure_wifi ssid=val [password=val] [static_ip_en=true/false] [static_ip=ip]
            if(tokensNum >= 2) {
                
                int max_params = 4;
                char* pnames[max_params];
                char* pvalues[max_params];
                int pcount = 0;
                
                char* pname;
                char* pvalue;
                for(int i = 1; i < tokensNum; i++) {                
                    parseParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < max_params) {
                        pnames[pcount] = pname;
                        pvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_configure_wifi(pnames, pvalues, pcount, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_RR_WIFI) == 0) {
            // синтаксис:
            //     rr_wifi info/status/start/stop/restart
            if(tokensNum >= 2) {
                char* wifi_cmd = tokens[1];
                
                // Команда корректна
                success = true;
                
                // Выполнить команду                    
                cmd_rr_wifi(wifi_cmd, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_GCODE_G0) == 0) {
            // синтаксис:
            //     G0 [Xv1] [Yv2] [Zv3]
            // X, Y, Z - значения перемещения для координаты, мм
            if(tokensNum >= 2) {
                char motor_names[3];
                double cvalues[3];
                int pcount = 0;
                                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {                
                    parseGcodeParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < 3) {
                        motor_names[pcount] = pname;
                        cvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                if(pcount > 0) {
                    // Команда корректна
                    success = true;
                    
                    // Выполнить команду                    
                    cmd_gcode_g0(motor_names, cvalues, pcount, reply_buffer);
                }
            }
        } else if(strcmp(tokens[0], CMD_GCODE_G01) == 0) {
            // синтаксис:
            //     G01 [Xv1] [Yv2] [Zv3] Fv4
            // X, Y, Z - значения перемещения для координаты, мм
            // F - скорость перемещения, мм/с
            if(tokensNum >= 3) {
                char motor_names[3];
                double cvalues[3];
                int pcount = 0;
                
                double f = 0;
                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {
                    parseGcodeParam(tokens[i], &pname, &pvalue);
                    
                    if(pname == GCODE_PARAM_F) {
                        f = pvalue;
                    } else {
                        if(pcount < 3) {
                            motor_names[pcount] = pname;
                            cvalues[pcount] = pvalue;
                            pcount++;
                        }
                    }
                }
                
                if(pcount > 0 && f > 0) {
                    // Команда корректна
                    success = true;
                    
                    // Выполнить команду                    
                    cmd_gcode_g01(motor_names, cvalues, pcount, f, reply_buffer);
                }
            }
            
        } else if(strcmp(tokens[0], CMD_GCODE_G02) == 0) {
            // синтаксис:
            //     G02 [Xfval Yfval] [Zfval] Rfval Ffval
            //     G02 [Xfval Yfval] [Zfval] Ifval Jfval Ffval
            // X, Y - координаты точки-назначения (если не указаны, пройти полную окружность), мм
            // Z - если указано, пройти по спирали, смещаясь по оси  Z
            // R - радиус окружности (R>0 - проход по меньшей дуге<180гр, R<0 - проход по большей дуге>180гр), мм
            // I, J - координаты центра окружностиv (если задан R, игнорируются), мм
            // F - скорость перемещения, мм/с
            
            // http://www.cnccookbook.com/CCCNCGCodeArcsG02G03.htm
            // http://cncmaster.org/emc2_g-code_table_g02g03
            
            if(tokensNum >= 8) {
                char pnames[7];
                double pvalues[7];
                int pcount = 0;
                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {
                    parseGcodeParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < 7) {
                        pnames[pcount] = pname;
                        pvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                if(pcount > 0) {
                    // Команда корректна
                    success = true;
                    
                    // Выполнить команду
                    cmd_gcode_g02(pnames, pvalues, pcount, reply_buffer);
                }
            }
        } else if(strcmp(tokens[0], CMD_GCODE_G03) == 0) {
            // синтаксис:
            //     G03 [Xfval Yfval] [Zfval] Rfval Ffval
            //     G03 [Xfval Yfval] [Zfval] Ifval Jfval Ffval
            // X, Y - координаты точки-назначения (если не указаны, пройти полную окружность), мм
            // Z - если указано, пройти по спирали, смещаясь по оси  Z
            // R - радиус окружности (R>0 - проход по меньшей дуге<180гр, R<0 - проход по большей дуге>180гр), мм
            // I, J - координаты центра окружностиv (если задан R, игнорируются), мм
            // F - скорость перемещения, мм/с
            
            // http://www.cnccookbook.com/CCCNCGCodeArcsG02G03.htm
            // http://cncmaster.org/emc2_g-code_table_g02g03
            
            if(tokensNum >= 8) {
                char pnames[7];
                double pvalues[7];
                int pcount = 0;
                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {
                    parseGcodeParam(tokens[i], &pname, &pvalue);
                    
                    if(pcount < 7) {
                        pnames[pcount] = pname;
                        pvalues[pcount] = pvalue;
                        pcount++;
                    }
                }
                
                if(pcount > 0) {
                    // Команда корректна
                    success = true;
                    
                    // Выполнить команду
                    cmd_gcode_g03(pnames, pvalues, pcount, reply_buffer);
                }
            }
        }
    }
    if(!success) {
        #ifdef DEBUG_SERIAL
            Serial.print("Can't handle command: ");
            Serial.println(buffer);
        #endif // DEBUG_SERIAL
        
        // Подготовить ответ
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
    }
    
    return strlen(reply_buffer);
}

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
 * @reply_buffer - ответ: строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
int handleInput(char* buffer, int buffer_size, char* reply_buffer) {
    // добавим к входным данным завершающий ноль, 
    // чтобы рассматривать их как корректную строку
    buffer[buffer_size] = 0;
    
    // обнулим ответ
    reply_buffer[0] = 0;
    
    char cmd_buffer[CMD_READ_BUFFER_SIZE];
    char cmd_reply_buffer[CMD_WRITE_BUFFER_SIZE];
        
    // Разобьем входящую строку на куски по разделителю команд ';'
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    // первая команда
    token = strtok_r(buffer, ";", &last);
    bool firstToken = true;
    while(token != NULL) {
        strcpy(cmd_buffer, token);
        handleCommand(cmd_buffer, cmd_reply_buffer);
        
        // добавлять к ответу предваряющий разделитель ';' для всех команд,
        // кроме первой
        if(!firstToken) {
            strcat(reply_buffer, ";");
        } else {
            firstToken = false;
        }
        
        // добавить ответ от команды в общий список
        // TODO: сделать что-нибудь в случае, если все ответы не умещаются
        // в буфер (это касается и разных других мест, но здесь особенно,
        // т.к. размер ответа от нескольких команд - динамический)
        strcat(reply_buffer, cmd_reply_buffer);
        
        // следующая команда
        token = strtok_r(NULL, ";", &last);
    }
  
    return strlen(reply_buffer);
}


