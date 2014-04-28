#include <WiFiShieldOrPmodWiFi_G.h>

#include <DNETcK.h>
#include <DWIFIcK.h>

// Глобальные настройки для станка

/* Минимальное количество циклов внутри одной "ступеньки" 
 * для линейной интерполяции G01 */
#define G01_PRECISION 100

/* Количество циклов в одной интерации команды "go" */
#define CMD_GO_CYCLE_COUNT 300

/* Количество циклов в одной интерации команды "calibrate" */
#define CMD_CALIBRATE_CYCLE_COUNT 50

namespace rraptor {
  int GLOBAL_DISABLE_MOTORS = 0;
  
  /* Остановить все моторы */
  const char* CMD_STOP = "stop";
  /* Повернуть заданный мотор на заданное количество шагов */
  const char* CMD_STEP = "step";
  /* Переместить заданную координату в заданное положение */
  const char* CMD_MOVE = "move";
  /* Сдвинуть заданную координату на заданное расстояние */
  const char* CMD_SHIFT = "shift";
  /* Запустить мотор с заданной скоростью на непрерывное вращение */
  const char* CMD_GO = "go";
  /** 
   * Калибровать координату - запустить мотор с заданной скоростью на непрерывное вращение в режиме калибровки - 
   * не проверяя выход за границы рабочей области и сбрасывая значение текущей позиции в 0.
   */
  const char* CMD_CALIBRATE = "calibrate";
  
  /* Команда G-code G01 - прямая линия */
  const char* CMD_GCODE_G01 = "G01";
  /* Команда G-code G02 - дуга по часовой стрелке */
  const char* CMD_GCODE_G02 = "G02";
  /* Команда G-code G03 - дуга против часовой стрелки */
  const char* CMD_GCODE_G03 = "G03";
  
  const int REPLY_STATUS_OK = 0;
  const int REPLY_STATUS_ERROR = 1;
  
/**
 * Структура - шаговый двигатель.
 */
typedef struct {
    /* Motor name */
    char* name;
    /* Movement distance per movement cycle, micrometre */
    int distance_per_cycle;
    /* Time spent for movement cycle, microseconds */
    int time_per_cycle;
  
    int PULSE_PIN;
    int DIR_PIN;   
    int EN_PIN;
    
    /* With dir_inv=1 and dir_pin=1 direction is 0->1, with dir_inv=-1 and dir_pin=1 direction 1->0 */
    int dir_inv;

    /* Задержка между 2 шагами, микросекунды */
    int step_delay;
    
    /* Максимальное значение положения */
    int MAX_POS;
    /* Текущее положение */
    int current_pos;
} smotor;

smotor sm_x, sm_y, sm_z;

/** 
 * Текущая скорость моторов, запущенных командой go:
 * 1 - вперед
 * 0 - стоит на месте
 * -1 - назад
 */
int speed_x = 0;
int speed_y = 0;
int speed_z = 0;

/** 
 * calibrate:
 * 1 - в режиме калибровки не проверяется выход за границы рабочей области.
 * 0 - обычный режим, при перемещении рабочего блока перемещаются границы рабочей области.
 */
int calibrate_mode = 0;

int reply_status = REPLY_STATUS_OK;


void init_smotor(smotor* sm, char* name, int distance_per_cycle, int time_per_cycle,
    int step_delay, int pulse_pin, int dir_pin, int en_pin, int max_pos, int dir_inv) {
  
  sm->name = name;
  sm->distance_per_cycle = distance_per_cycle;
  sm->time_per_cycle = time_per_cycle;
  sm->step_delay = step_delay;  
  
  sm->PULSE_PIN = pulse_pin;
  sm->DIR_PIN = dir_pin;
  sm->EN_PIN = en_pin;
  
  sm->MAX_POS = max_pos;
  sm->dir_inv = dir_inv;
}

/**
 * Получить шаговый двигатель по уникальному имени.
 */
smotor* smotor_by_id(char* id) {
  if(strcmp("x", id) == 0) {
    return &sm_x;
  } else if(strcmp("y", id) == 0) {
    return &sm_y;
  } else if(strcmp("z", id) == 0) {
    return &sm_z;
  } else {
    return 0;
  }
}

/**
 * Установить начальную точку (0,0,0) в текущем положении.
 */
void reset_start_pos() {
  sm_x.current_pos = 0;
  sm_y.current_pos = 0;
  sm_z.current_pos = 0;
}

void enable_motors() {
  GLOBAL_DISABLE_MOTORS = 0;
}

void disable_motors() {
  GLOBAL_DISABLE_MOTORS = 1;
}

/**
 * Повернуть двигатель на нужное количество циклов с нужной скоростью.
 * cnum - количество циклов - знаковое целое: если > 0, шагаем вперед, если < 0 - назад.
 * cdelay - время выполнения цикла, микросекунды.  
 */
void step_motor(smotor* sm, int cnum, int cdelay) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", cycle count=" + cnum + ", cycle delay=" + cdelay + "us" + "]: ");
    Serial.print(String("") + "step=" + sm->PULSE_PIN + ", dir=" + sm->DIR_PIN + ",  ");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm->DIR_PIN, (dir * sm->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    int tdelay = (cdelay - sm->time_per_cycle) / 1000;
    if(tdelay < 0) {
        if(cdelay != 0) {
          Serial.println(String("") + "Waring: timer delay < 0 [motor=" + sm->name + "], reset to 0");
        }
        tdelay=0;
    }
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги и не забываем проверять выход за границы рабочей области
    while (i < cnum && !GLOBAL_DISABLE_MOTORS && 
            (calibrate_mode ? 1 : 
            (dir > 0 ? sm->current_pos + sm->distance_per_cycle <= sm->MAX_POS : 
                       sm->current_pos - sm->distance_per_cycle >= 0))) {
        // пусть будет 2 шага в одном цикле
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        if(calibrate_mode) {
          sm->current_pos = 0;
        } else {
          if(dir > 0) {
            sm->current_pos += sm->distance_per_cycle;
          } else {
            sm->current_pos -= sm->distance_per_cycle;
          }
        }
        
        delay(tdelay);
        i++;
    }
    
    Serial.println(String("") + "Current position=" + sm->current_pos + ", motor=" + sm->name);
}

/**
 * Сдвинуть текущее положение координаты на указанное расстояние с нужной скоростью.
 * dl - сдвиг относительно текущего положения, микрометры - знаковое целое: если > 0, сдвиг вперед, если < 0 - назад.
 * dt - время сдвига, микросекунды.
 */
void shift_coord_um(smotor* sm, int dl, int dt) {
    //Serial.print(String("") + "Info: Shifting coord [name=" + sm->name + ", dl=" + dl + "um, dt=" + dt + "us" + "]: ");
    if(dl == 0 || abs(dl) < sm->distance_per_cycle) {
    //    Serial.println("skip");
        return;
    }
    
    // calculate number of steps
    int cnum = abs(dl) / sm->distance_per_cycle;
    //Serial.print(String("") + "cycle count=" + cnum);
    // calculate cycle delay
    int cdelay = dt / cnum;

    //debug_printf("cycle delay=%dus, ", cdelay);
    // calculate timer delay - in millis
    // time_per_cycle = step_delay * 4
    int tdelay = (cdelay - sm->time_per_cycle) / 1000;
    //Serial.println(String("") + ", timer delay=" + tdelay);
    if(tdelay < 0) {
        if(cdelay != 0) {
      //      Serial.println(String("") + "Waring: timer delay < 0 [motor=" + sm->name + "], reset to 0");
        }
        tdelay=0;
    }
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    //Serial.println(String("") + ", estimated time=" + est_time + "us");
  
    // задать направление в зависиомости от знака
    int dir = (dl > 0 ? 1 : -1);
    digitalWrite(sm->DIR_PIN, (dir * sm->dir_inv > 0 ? HIGH : LOW));

    int i=0;
    // Считаем шаги и не забываем проверять выход за границы рабочей области
    while (i < cnum && !GLOBAL_DISABLE_MOTORS && 
            (calibrate_mode ? 1 :
            (dir > 0 ? sm->current_pos + sm->distance_per_cycle <= sm->MAX_POS : 
                       sm->current_pos - sm->distance_per_cycle >= 0))) {
      // пусть будет 2 шага в одном цикле
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        if(calibrate_mode) {
          sm->current_pos = 0;
        } else {
          if(dir > 0) {
            sm->current_pos += sm->distance_per_cycle;
          } else {
            sm->current_pos -= sm->distance_per_cycle;
          }
        }
        
        delay(tdelay);
        i++;
    }
    
    //Serial.println(String("") + "Current position=" + sm->current_pos + ", motor=" + sm->name);
}

/**
 * Переместить текущее положение координаты в указанное положение с указанной скоростью.
 * dl - сдвиг относительно текущего положения, микрометры - знаковое целое: если > 0, сдвиг вперед, если < 0 - назад.
 * dt - время сдвига, микросекунды.
 */
void move_coord_um(smotor* sm, int pos) {
    Serial.print(String("") + "Info: Moving coord [name=" + sm->name + ", pos=" + pos + "um" + "]: ");
    
    int dl = pos - sm->current_pos;
    Serial.println(String("") + "shift=" + dl + "um");
    shift_coord_um(sm, dl, 0);
}

/**
 * Реализация команды g-code G01 - переместить текущее положение рабочего блока в 
 * указанную позицию по прямой линии с заданной скоростью.
 */
void gcode_g01(smotor* _sm_x, smotor* _sm_y, smotor* _sm_z, int x, int y, int z, int f) {
    Serial.print(String("") + "Info: Exec G01 [" 
        + "dest pos=(" + x + "um" + ", " + y + "um" + ", " + z + "um)" + ", f=" + f + "" + "]: ");
    Serial.println(String("") + 
         + " current pos=(" + _sm_x->current_pos + "um, " + _sm_y->current_pos + "um" + ", " + _sm_z->current_pos + "um)");
         
    // В этой реализации перемещение по координате z обрабатывается отдельно от x и y - за один шаг, 
    // перемещение по плоскости (x,y) осуществляется маленькими шажками по алгоритму Брезенхама.
    // TODO: добавить реализацию работы со скоростью.
    
    // Сначала переместимся по Z:
    if(z != _sm_z->current_pos) {
      int dl_z = z - _sm_z->current_pos;
      Serial.println(String("") + "Move Z, shift=" + dl_z + "um");
      shift_coord_um(_sm_z, dl_z, 0);
    }
    
    // Переместимся по плоскости (x,y) по лесенке.
    
    int dl_x= x - _sm_x->current_pos;
    int dl_y= y - _sm_y->current_pos;
    
    // Проверить крайние ситуации
    if(abs(dl_x) < (_sm_x->distance_per_cycle * G01_PRECISION) ||
        abs(dl_y) < (_sm_y->distance_per_cycle * G01_PRECISION)) {
      shift_coord_um(_sm_x, dl_x, 0);
      shift_coord_um(_sm_y, dl_y, 0);
    } else {
      // пошли лесенкой
      int stairstep_count = abs(dl_x) < abs(dl_y) ? 
          abs(dl_x) / (_sm_x->distance_per_cycle * G01_PRECISION) :
          abs(dl_y) / (_sm_y->distance_per_cycle * G01_PRECISION);
      Serial.println(String("") + "Go X-Y stairs, stair step count=" + stairstep_count);
    
      // длина ступеньки - мкм
      int stairstep_x = dl_x / stairstep_count;
      int stairstep_y = dl_y / stairstep_count;
    
      for(int i = 0; i < stairstep_count; i++) {
        shift_coord_um(_sm_x, stairstep_x, 0);
        shift_coord_um(_sm_y, stairstep_y, 0);
      }
      
      // добить оставшийся путь
      shift_coord_um(_sm_x, x - _sm_x->current_pos, 0);
      shift_coord_um(_sm_y, y - _sm_y->current_pos, 0);
    }
}

/**
 * Реализация команды g-code G01 - переместить текущее положение рабочего блока в 
 * указанную позицию по прямой линии с заданной скоростью.
 */
void gcode_g01_v2(smotor* _sm_x, smotor* _sm_y, smotor* _sm_z, int x, int y, int z, int f) {
    Serial.print(String("") + "Info: Exec G01 [" 
        + "dest pos=(" + x + "um" + ", " + y + "um" + ", " + z + "um)" + ", f=" + f + "" + "]: ");
    Serial.println(String("") + 
         + " current pos=(" + _sm_x->current_pos + "um, " + _sm_y->current_pos + "um" + ", " + _sm_z->current_pos + "um)");
         
    // В этой реализации перемещение по координате z обрабатывается отдельно от x и y - за один шаг, 
    // перемещение по плоскости (x,y) осуществляется маленькими шажками по алгоритму Брезенхама.
    // TODO: добавить реализацию работы со скоростью.
    
    // Сначала переместимся по Z:
    if(z != _sm_z->current_pos) {
      int dl_z = z - _sm_z->current_pos;
      Serial.println(String("") + "Move Z, shift=" + dl_z + "um");
      shift_coord_um(_sm_z, dl_z, 0);
    }
    
    // Переместимся по плоскости (x,y) по лесенке.
    
    int dl_x= x - _sm_x->current_pos;
    int dl_y= y - _sm_y->current_pos;
    
    // Проверить крайние ситуации
    if(abs(dl_x) < (_sm_x->distance_per_cycle * G01_PRECISION) ||
        abs(dl_y) < (_sm_y->distance_per_cycle * G01_PRECISION)) {
      shift_coord_um(_sm_x, dl_x, 0);
      shift_coord_um(_sm_y, dl_y, 0);
    } else {
      // пошли лесенкой
      int stairstep_count = abs(dl_x) < abs(dl_y) ? 
          abs(dl_x) / (_sm_x->distance_per_cycle * G01_PRECISION) :
          abs(dl_y) / (_sm_y->distance_per_cycle * G01_PRECISION);
      Serial.println(String("") + "Go X-Y stairs, stair step count=" + stairstep_count);
    
      // длина ступеньки - мкм
      int stairstep_x = dl_x / stairstep_count;
      int stairstep_y = dl_y / stairstep_count;
      
      int cnum_x = abs(stairstep_x) / _sm_x->distance_per_cycle;
      int cnum_y = abs(stairstep_y) / _sm_y->distance_per_cycle;
      
      // задать направление в зависиомости от знака
      int dir_x = (stairstep_x > 0 ? 1 : -1);
      digitalWrite(_sm_x->DIR_PIN, (dir_x * _sm_x->dir_inv > 0 ? HIGH : LOW));
      
      // задать направление в зависиомости от знака
      int dir_y = (stairstep_y > 0 ? 1 : -1);
      digitalWrite(_sm_y->DIR_PIN, (dir_y * _sm_y->dir_inv > 0 ? HIGH : LOW));
    
      for(int j = 0; j < stairstep_count; j++) {
        //shift_coord_um(_sm_x, stairstep_x, 0);
        //shift_coord_um(_sm_y, stairstep_y, 0);
        
        int i=0;
        // Считаем шаги и не забываем проверять выход за границы рабочей области
        while (i < cnum_x && !GLOBAL_DISABLE_MOTORS && 
                (calibrate_mode ? 1 :
                (dir_x > 0 ? _sm_x->current_pos + _sm_x->distance_per_cycle <= _sm_x->MAX_POS : 
                           _sm_x->current_pos - _sm_x->distance_per_cycle >= 0))) {
          // пусть будет 2 шага в одном цикле
            digitalWrite(_sm_x->PULSE_PIN, HIGH);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_x->PULSE_PIN, LOW);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_x->PULSE_PIN, HIGH);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_x->PULSE_PIN, LOW);
            delayMicroseconds(_sm_x->step_delay);
            
            if(calibrate_mode) {
              _sm_x->current_pos = 0;
            } else {
              if(dir_x > 0) {
                _sm_x->current_pos += _sm_x->distance_per_cycle;
              } else {
                _sm_x->current_pos -= _sm_x->distance_per_cycle;
              }
            }            
            //delay(tdelay);
            i++;
        }
        
        
        // Считаем шаги и не забываем проверять выход за границы рабочей области
        while (i < cnum_y && !GLOBAL_DISABLE_MOTORS && 
                (calibrate_mode ? 1 :
                (dir_y > 0 ? _sm_y->current_pos + _sm_y->distance_per_cycle <= _sm_y->MAX_POS : 
                           _sm_y->current_pos - _sm_y->distance_per_cycle >= 0))) {
          // пусть будет 2 шага в одном цикле
            digitalWrite(_sm_y->PULSE_PIN, HIGH);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_y->PULSE_PIN, LOW);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_y->PULSE_PIN, HIGH);
            delayMicroseconds(_sm_x->step_delay);
            digitalWrite(_sm_y->PULSE_PIN, LOW);
            delayMicroseconds(_sm_y->step_delay);
            
            if(calibrate_mode) {
              _sm_y->current_pos = 0;
            } else {
              if(dir_y > 0) {
                _sm_y->current_pos += _sm_y->distance_per_cycle;
              } else {
                _sm_y->current_pos -= _sm_y->distance_per_cycle;
              }
            }            
            //delay(tdelay);
            i++;
        }        
      }
      
      // добить оставшийся путь
      shift_coord_um(_sm_x, x - _sm_x->current_pos, 0);
      shift_coord_um(_sm_y, y - _sm_y->current_pos, 0);
    }
}


void go_cycle_motor(smotor* sm, int spd) {
  if(spd != 0) {
    int cnum = 0;
    int cdelay = 0;
    if(spd > 0) {
      cnum = (calibrate_mode ? CMD_CALIBRATE_CYCLE_COUNT : CMD_GO_CYCLE_COUNT);
    } else if (spd < 0) {
      cnum = -1 * (calibrate_mode ? CMD_CALIBRATE_CYCLE_COUNT : CMD_GO_CYCLE_COUNT);
    }
    step_motor(sm, cnum, cdelay);
  }
}

/**
 * Обработать поведение режима постоянной работы моторов,
 * устанавлемоего командой go. Для моторов с ненулевой скоростью,
 * повернуть на один цикл в заданном направлении.
 */
void handle_go() {
  go_cycle_motor(&sm_x, speed_x);
  go_cycle_motor(&sm_y, speed_y);
  go_cycle_motor(&sm_z, speed_z);
}

/**
 * Остановить все моторы, запущенные командой go.
 */
void stop_motors() {
  speed_x = 0;
  speed_y = 0;
  speed_z = 0;
  
  calibrate_mode = 0;
}


int handle_command(char* cmd_line) {
  // новая команда остановит моторы, находящиеся в режиме 
  // постоянной работы и сбросит режим калибровки
  stop_motors();  
  
  String cmd_line_str = String(cmd_line).trim();
  
  int success = 0;
  if (cmd_line_str.startsWith(CMD_STOP)) {
    Serial.println("Handle command: stop");
    success = 1;
    
    stop_motors();
  } else if(cmd_line_str.startsWith(CMD_STEP)) {
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

        smotor* sm = smotor_by_id(motor_name_chars);
        if(sm != 0) {
          step_motor(sm, cnum, cdelay);
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_SHIFT)) {
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

        smotor* sm = smotor_by_id(motor_name_chars);
        if(sm != 0) {
          shift_coord_um(sm, dl, dt);
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_MOVE)) {
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

        smotor* sm = smotor_by_id(motor_name_chars);
        if(sm != 0) {
          move_coord_um(sm, pos);
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_GO)) {
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
        if(motor_name.equalsIgnoreCase("x")) {
          speed_x = spd;
        } else if(motor_name.equalsIgnoreCase("y")) {
          speed_y = spd;
        } else if(motor_name.equalsIgnoreCase("z")) {
          speed_z = spd;
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_CALIBRATE)) {
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
        calibrate_mode = 1;
        if(motor_name.equalsIgnoreCase("x")) {
          speed_x = spd;
        } else if(motor_name.equalsIgnoreCase("y")) {
          speed_y = spd;
        } else if(motor_name.equalsIgnoreCase("z")) {
          speed_z = spd;
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
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
          Serial.println(String("") + "Handle command: [cmd=G01, dest pos=(" + x + ", " + y + ", " + z + ")");
          
          // Выполнить команду
          gcode_g01(&sm_x, &sm_y, &sm_z, x, y, z, f);        
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_GCODE_G02)) {
    // TODO: implement G02
  } else if(cmd_line_str.startsWith(CMD_GCODE_G03)) {
    // TODO: implement G03
  }
  
  if(!success) {
    Serial.println("Can't handle command: " + cmd_line_str);
    return REPLY_STATUS_ERROR;
  } else {
    return REPLY_STATUS_OK;
  }
}

} // namespace rraptor

// WiFi
IPv4 ipServer = {192,168,43,191};
unsigned short portServer = DNETcK::iPersonalPorts44 + 300;

const char * szSsid = "lasto4ka";

#define WiFiConnectMacro() DWIFIcK::connect(szSsid, &status)

typedef enum {
    NONE = 0,
    INITIALIZE,
    LISTEN,
    ISLISTENING,
    AVAILABLECLIENT,
    ACCEPTCLIENT,
    READ,
    WRITE,
    CLOSE,
    EXIT,
    DONE
} STATE;

STATE state = INITIALIZE;

unsigned tStart = 0;
// connection reset timeout
unsigned tWait = 10000;

TcpServer tcpServer;
TcpClient tcpClient;

byte rgbRead[1024];
char tcpReadLine[1024];
int cbRead = 0;
int count = 0;

DNETcK::STATUS status;
/* ***************************************************** */
// Runtime

void printIP(void) {
    Serial.print("IP Address assigned: ");
    Serial.print((int)ipServer.rgbIP[0]);
    Serial.print(".");
    Serial.print((int)ipServer.rgbIP[1]);
    Serial.print(".");
    Serial.print((int)ipServer.rgbIP[2]);
    Serial.print(".");
    Serial.println((int)ipServer.rgbIP[3]);
}
void setup() {
    // start serial port at 9600 bps:
    Serial.begin(9600);
    
    // init stepper motors
    int step_delay = 500;
    // with step_delay=250 1 cycle=1 mls
    //  init_smotor(smotor* sm, char* name, int distance_per_cycle, int time_per_cycle,
    //    int step_delay, int pulse_pin, int dir_pin, int en_pin, int max_pos, int dir_inv)
    rraptor::init_smotor(&rraptor::sm_x, "X", 15, step_delay * 4, step_delay, 3, 5, 6, 216000, 1); // X - синий драйвер
    rraptor::init_smotor(&rraptor::sm_y, "Y", 15, step_delay * 4, step_delay, 26, 27, 28, 300000, -1); // Y - желтый драйвер
    rraptor::init_smotor(&rraptor::sm_z, "Z", 15, step_delay * 4, step_delay, 31, 32, 33, 100000, -1); // Z - черный драйвер
    
    pinMode(rraptor::sm_x.DIR_PIN, OUTPUT);
    pinMode(rraptor::sm_x.PULSE_PIN, OUTPUT);
    pinMode(rraptor::sm_y.DIR_PIN, OUTPUT);
    pinMode(rraptor::sm_y.PULSE_PIN, OUTPUT);
    pinMode(rraptor::sm_z.DIR_PIN, OUTPUT);
    pinMode(rraptor::sm_z.PULSE_PIN, OUTPUT);
    
    // Wifi
    int conID = DWIFIcK::INVALID_CONNECTION_ID;

    Serial.println("WiFiTCPEchoServer 1.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID) {
        Serial.print("Connection Created, ConID = ");
        Serial.println(conID, DEC);
        printIP();
        state = INITIALIZE;
    } else {
        Serial.print("Unable to connection, status: ");
        Serial.println(status, DEC);
        state = EXIT;
    }

    DNETcK::begin(ipServer);
}

void loop() {
  //rraptor::handle_command("move x -10000");
  //rraptor::handle_command("step x 1000");
  
  // Провернуть моторы, находящием в режиме постоянной работы.
  rraptor::handle_go();
  
    switch(state) {
    case INITIALIZE:
        if(DNETcK::isInitialized(&status)) {
            Serial.println("IP Stack Initialized");
            state = LISTEN;
        } else if(DNETcK::isStatusAnError(status)) {
            Serial.print("Error in initializing, status: ");
            Serial.println(status, DEC);
            state = EXIT;
        }
        break;

    case LISTEN:
        if(tcpServer.startListening(portServer)) {
            Serial.println("Started Listening");
            state = ISLISTENING;
        } else {
            state = EXIT;
        }
        break;

    case ISLISTENING:
        if(tcpServer.isListening(&status)) {  
            Serial.print("Listening on port: ");
            Serial.print(portServer, DEC);
            Serial.println("");
            
            state = AVAILABLECLIENT;
        } else if(DNETcK::isStatusAnError(status)) {
            state = EXIT;
        }
        break;

    case AVAILABLECLIENT:
        if((count = tcpServer.availableClients()) > 0) {
            Serial.print("Got ");
            Serial.print(count, DEC);
            Serial.println(" clients pending");
            state = ACCEPTCLIENT;
        }
        break;

    case ACCEPTCLIENT:
        
        tcpClient.close(); 

        if(tcpServer.acceptClient(&tcpClient)) {
            Serial.println("Got a Connection");
            state = READ;
            tStart = (unsigned) millis();
        } else {
            state = CLOSE;
        }
        break;

    case READ:
        if((cbRead = tcpClient.available()) > 0) {
            cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
            cbRead = tcpClient.readStream(rgbRead, cbRead);

            Serial.print("Got ");
            Serial.print(cbRead, DEC);
            Serial.println(" bytes");
            
            strncpy(tcpReadLine, (char*)rgbRead, cbRead);
            tcpReadLine[cbRead] = 0; // ensure 0-terminated         
            rraptor::reply_status = rraptor::handle_command(tcpReadLine);
            
            state = WRITE;
        } else if( (((unsigned) millis()) - tStart) > tWait ) {
            state = CLOSE;
        }
        break;

    case WRITE:
        if(tcpClient.isConnected()) {               
            Serial.println("Writing: ");
            Serial.println(rraptor::reply_status, DEC);  

            tcpClient.writeByte((byte)rraptor::reply_status);
            state = READ;
            tStart = (unsigned) millis();
        } else {
            Serial.println("Unable to write back.");  
            state = CLOSE;
        }
        break;
        
    case CLOSE:
        tcpClient.close();
        Serial.println("Closing TcpClient");
        Serial.println("");
        rraptor::stop_motors();
        state = ISLISTENING;
        break;

    case EXIT:
        tcpClient.close();
        tcpServer.close();
        Serial.println("Something went wrong, sketch is done.");  
        rraptor::stop_motors();
        state = DONE;
        break;

    case DONE:
    default:
        break;
    }
    
    DNETcK::periodicTasks(); 
}


