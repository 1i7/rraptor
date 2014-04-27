
#include "WProgram.h"

#include "rraptor_protocol.h"


/**
 * Обработать входные данные - разобрать строку, выполнить команду.
 * @return размер ответа в байтах (0, чтобы не отправлять ответ).
 */
int handleInput(char* buffer, int size, char* reply_buffer) {
    int replySize = 0;
    reply_buffer[0] = 0;
    
    String cmd_line_str = String(buffer).trim();
    
    int success = 0;
    if (cmd_line_str.startsWith(CMD_RR_STOP)) {
        // Команда корректна
        success = 1;
        Serial.println("Handle command: stop");
      
        // Выполнить команду
        cmd_rr_stop();
    } else if(cmd_line_str.startsWith(CMD_RR_STEP)) {
        // command 'step' syntax:
        //     step motor_name cnum [cdelay]
        String motor_name;
        String cnum_str;
        String cdelay_str;
        int cnum;
        int cdelay = 0;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                motor_name = cmd_line_str.substring(space1 + 1, space2);
                
                space1 = space2;
                space2 = cmd_line_str.indexOf(' ', space1 + 1);
                if(space2 != -1) {
                    cnum_str = cmd_line_str.substring(space1 + 1, space2);
                } else {
                    cnum_str = cmd_line_str.substring(space1 + 1);
                }
                
                cnum = cnum_str.toInt();//atoi(cnum_str.toCharArray());
                  
                // Необязательный параметр - cdelay
                if(space2 != -1) {
                    cdelay_str = cmd_line_str.substring(space2 + 1);
                    cdelay = cdelay_str.toInt();//atoi(cnum_str.toCharArray());
                }
                  
                // Команда корректна
                success = 1;
                Serial.println("Handle command: [cmd=step, motor=" + motor_name + ", cycle num=" + cnum + ", cycle delay=" + cdelay + "]");
                  
                // Выполнить команду
                char motor_name_chars[motor_name.length() + 1];
                motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);
                
                cmd_rr_step(motor_name_chars, cnum, cdelay);
            }
        }
    } else if(cmd_line_str.startsWith(CMD_RR_SHIFT)) {
        // command 'shift' syntax:
        //     shift motor_name dl [dt]
        String motor_name;
        String dl_str;
        String dt_str;
        int dl;
        int dt = 0;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                motor_name = cmd_line_str.substring(space1 + 1, space2);
                
                space1 = space2;
                space2 = cmd_line_str.indexOf(' ', space1 + 1);
                if(space2 != -1) {
                    dl_str = cmd_line_str.substring(space1 + 1, space2);
                } else {
                    dl_str = cmd_line_str.substring(space1 + 1);
                }
                
                dl = dl_str.toInt();//atoi(cnum_str.toCharArray());
                  
                // Необязательный параметр - dt
                if(space2 != -1) {
                    dt_str = cmd_line_str.substring(space2 + 1);
                    dt = dt_str.toInt();//atoi(cnum_str.toCharArray());
                }
                  
                // Команда корректна
                success = 1;
                Serial.println("Handle command: [cmd=shift, motor=" + motor_name + ", dl=" + dl + ", dt=" + dt + "]");
                  
                // Выполнить команду
                char motor_name_chars[motor_name.length() + 1];
                motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);
                
                cmd_rr_shift(motor_name_chars, dl, dt);
            }
        }
    } else if(cmd_line_str.startsWith(CMD_RR_MOVE)) {
        // command 'move' syntax:
        //     move motor_name pos [dt]
        String motor_name;
        String pos_str;
        int pos;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                motor_name = cmd_line_str.substring(space1 + 1, space2);
                
                space1 = space2;
                space2 = cmd_line_str.indexOf(' ', space1 + 1);
                if(space2 != -1) {
                    pos_str = cmd_line_str.substring(space1 + 1, space2);
                } else {
                    pos_str = cmd_line_str.substring(space1 + 1);
                }
                
                pos = pos_str.toInt();//atoi(cnum_str.toCharArray());
                  
                // Команда корректна
                success = 1;
                Serial.println("Handle command: [cmd=move, motor=" + motor_name + ", pos=" + pos + "]");
                  
                // Выполнить команду
                char motor_name_chars[motor_name.length() + 1];
                motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);
                
                cmd_rr_move(motor_name_chars, pos);
            }
        }
    } else if(cmd_line_str.startsWith(CMD_RR_GO)) {
        // command 'go' syntax:
        //     go motor_name speed
        String motor_name;
        String spd_str;
        int spd;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                motor_name = cmd_line_str.substring(space1 + 1, space2);
                
                spd_str = cmd_line_str.substring(space2 + 1);
                spd = spd_str.toInt();//atoi(cnum_str.toCharArray());
                  
                // Команда корректна
                success = 1;
                Serial.println("Handle command: [cmd=go, motor=" + motor_name + ", speed=" + spd + "]");
                  
                // Выполнить команду - запустить моторы в постоянную работу до прихода команды на остановку
                char motor_name_chars[motor_name.length() + 1];
                motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);
                
                cmd_rr_go(motor_name_chars, spd);
            }
        }
    } else if(cmd_line_str.startsWith(CMD_RR_CALIBRATE)) {
        // command 'calibrate' syntax:
        //     calibrate motor_name speed
        String motor_name;
        String spd_str;
        int spd;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                motor_name = cmd_line_str.substring(space1 + 1, space2);
                
                spd_str = cmd_line_str.substring(space2 + 1);
                spd = spd_str.toInt();//atoi(cnum_str.toCharArray());
                  
                // Команда корректна
                success = 1;
                Serial.println("Handle command: [cmd=calibrate, motor=" + motor_name + ", speed=" + spd + "]");
                  
                // Выполнить команду - запустить моторы в постоянную работу до прихода команды на остановку
                // в режиме калибровки
                char motor_name_chars[motor_name.length() + 1];
                motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);
                
                cmd_rr_calibrate(motor_name_chars, spd);
            }
        }
    } else if(cmd_line_str.startsWith(CMD_GCODE_G01)) {
        // command 'G01' syntax:
        //     G01 x y z [f]
        String x_str;
        String y_str;
        String z_str;
        String f_str;
        int x;
        int y;
        int z;
        int f;
        
        int space1 = -1;
        int space2 = -1;
        
        space1 = cmd_line_str.indexOf(' ');
        if(space1 != -1) {
            space2 = cmd_line_str.indexOf(' ', space1 + 1);
            if(space2 != -1) {
                x_str = cmd_line_str.substring(space1 + 1, space2);
                x = x_str.toInt();//atoi(cnum_str.toCharArray());
                
                space1 = space2;
                space2 = cmd_line_str.indexOf(' ', space1 + 1);
                if(space2 != -1) {
                    y_str = cmd_line_str.substring(space1 + 1, space2);
                    y = y_str.toInt();//atoi(cnum_str.toCharArray());
                    
                    space1 = space2;
                    space2 = cmd_line_str.indexOf(' ', space1 + 1);
                    if(space2 != -1) {
                        z_str = cmd_line_str.substring(space1 + 1, space2);            
                    } else {
                        z_str = cmd_line_str.substring(space1 + 1);
                    }
                    
                    z = z_str.toInt();//atoi(cnum_str.toCharArray());
                    
                    // Необязательный параметр - f
                    if(space2 != -1) {
                        f_str = cmd_line_str.substring(space2 + 1);
                        f = f_str.toInt();//atoi(cnum_str.toCharArray());
                    }
                    
                    // Команда корректна
                    success = 1;
                    Serial.println(String("") + "Handle command: [cmd=G01, dest pos=(" + x + ", " + y + ", " + z + ")]");
                    
                    // Выполнить команду
                    cmd_gcode_g01(x, y, z, f);        
                }
            }
        }
    } else if(cmd_line_str.startsWith(CMD_GCODE_G02)) {
        // TODO: implement G02
        cmd_gcode_g02();
    } else if(cmd_line_str.startsWith(CMD_GCODE_G03)) {
        // TODO: implement G03
        cmd_gcode_g03();
    }
  
    if(!success) {
        Serial.println("Can't handle command: " + cmd_line_str);
        
        // Подготовить ответ
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
        replySize = strlen(reply_buffer) + 1;
    } else {
        // Подготовить ответ
        strcpy(reply_buffer, REPLY_OK);
        replySize = strlen(reply_buffer) + 1;
    }
    
    return replySize;
}

