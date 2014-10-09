#include "WProgram.h"

#include "rraptor_protocol.h"

/**
 * Разбить параметр команды на составляющие:
 * 1й символ - имя параметра,
 * подстрока начиная со 2го символа - значение параметра, число с плавающей точкой.
 */
static void parseParam(char* param, char* pname, double* pvalue) {
    if(strlen(param) > 1) {
        *pname = param[0];
        *pvalue = atof(param + 1);
    }
}

/**
 * Распознать и выполнить единственную команду.
 */
static int handleCommand(char* buffer, char* reply_buffer) {
    reply_buffer[0] = 0;
    
    bool success = false;
    
    char* tokens[8];
    int tokensNum = 0;
    
    // Разобьем команду на куски по пробелам
    char* token;
    token = strtok(buffer, " ");
    while(token != NULL) {
        tokens[tokensNum] = token;
        tokensNum++;
        
        token = strtok(NULL, " ");
    }
    
    // Определим, с какой командой имеем дело    
    if(tokensNum > 0) {
        if(strcmp(tokens[0], CMD_NAME) == 0) {
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
        } else if(strcmp(tokens[0], CMD_PING) == 0) {
            // синтаксис:
            //     ping
            // Команда корректна
            success = true;
          
            // Проверить доступность устройства.
            cmd_ping(reply_buffer);
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
        } else if(strcmp(tokens[0], CMD_RR_CURRENT_POS) == 0) {
            // синтаксис:
            //     rr_current_pos
            // Команда корректна
            success = true;
          
            // Выполнить команду
            cmd_rr_current_pos(reply_buffer);
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
                  
                // Выполнить команду - запустить моторы в постоянную работу 
                // до прихода команды на остановку               
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
                  
                // Выполнить команду - запустить моторы в постоянную работу 
                // до прихода команды на остановку в режиме калибровки                
                cmd_rr_calibrate(motor_name, spd, reply_buffer);
            }
        } else if(strcmp(tokens[0], CMD_GCODE_G0) == 0) {
            // синтаксис:
            //     G0 [Xv1] [Yv] [Zv3]
            // v1, v2, v3 - значения перемещения для координаты, мм
            if(tokensNum >= 2) {
                char motor_names[3];
                double cvalues[3];
                int pcount = 0;
                                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {                
                    parseParam(tokens[i], &pname, &pvalue);
                    
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
            //     G01 [Xv1] [Yv] [Zv3] Fv4
            // v1, v2, v3 - значения перемещения для координаты, мм
            // v4 - скорость перемещения, мм/с
            if(tokensNum >= 3) {
                char motor_names[3];
                double cvalues[3];
                int pcount = 0;
                
                double f = 0;
                
                char pname;
                double pvalue;
                for(int i = 1; i < tokensNum; i++) {                
                    parseParam(tokens[i], &pname, &pvalue);
                    
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
            // TODO: implement G02
            cmd_gcode_g02();
        } else if(strcmp(tokens[0], CMD_GCODE_G03) == 0) {
            // TODO: implement G03
            cmd_gcode_g03();
        }
    }
    if(!success) {
        Serial.print("Can't handle command: ");
        Serial.println(buffer);
        
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
 * @buffer - входные данные, строка, оканчивающаяся нулём.
 * @reply_buffer - ответ, строка, оканчивающая нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ).
 */
int handleInput(char* buffer, char* reply_buffer) {
    reply_buffer[0] = 0;
    
    char cmd_reply_buffer[128];
        
    // Разобьем входящую строку на куски по разделителю команд ';'
    char* token;
    // первая команда
    token = strtok(buffer, ";");
    bool firstToken = true;
    while(token != NULL) {
        handleCommand(buffer, cmd_reply_buffer);
        
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
        token = strtok(NULL, ";");
    }
  
    return strlen(reply_buffer);
}

