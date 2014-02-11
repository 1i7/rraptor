
int inByte = 0;         // incoming serial byte



/**
 * Структура - шаговый двигатель.
 */
struct smotor {
    int pin1; 
    int pin2; 
    int pin3; 
    int pin4;
} sm_x1, sm_x2, sm_y1, sm_y2, sm_z1, sm_z2, sm_extruder1;

/**
 * Структура - экструдер.
 */
struct extruder {
  int pin_temp;
} extruder1;

void init_smotor(smotor sm, int pin1, int pin2, int pin3, int pin4) {
  sm.pin1 = pin1;
  sm.pin2 = pin2;
  sm.pin3 = pin3;
  sm.pin4 = pin4;
}

void init_extruder(extruder extr, int pin_temp) {
  extr.pin_temp = pin_temp;
}

/**
 * Получить шаговый двигатель по уникальному имени.
 */
smotor smotor_by_id(char* id) {
  if(strcmp("x1", id) == 0) {
    return sm_x1;
  } else if(strcmp("x2", id) == 0) {
    return sm_x2;
  } else if(strcmp("y1", id) == 0) {
    return sm_y1;
  } else if(strcmp("y2", id) == 0) {
    return sm_y2;
  } else if(strcmp("z1", id) == 0) {
    return sm_z1;
  } else if(strcmp("z2", id) == 0) {
    return sm_z2;
  } else if(strcmp("sm_extruder1", id) == 0) {
    return sm_extruder1;
  } 
}

/**
 * Повернуть двигатель на нужное количество шагов с нужной скоростью.
 */
void step_motor(smotor sm, int snum, int sdelay) {
  //sm.pin1;
  //sm.pin2;
  //sm.pin3;
  //sm.pin4;
}

void step_x(int snum, int sdelay) {
  step_motor(sm_x1, snum, sdelay);
  step_motor(sm_x2, snum, sdelay);
}

void step_y(int snum, int sdelay) {
  step_motor(sm_y1, snum, sdelay);
  step_motor(sm_y2, snum, sdelay);
}

void step_z(int snum, int sdelay) {
  step_motor(sm_z1, snum, sdelay);
  step_motor(sm_z2, snum, sdelay);
}

void step_extruder1(int snum, int sdelay) {
  step_motor(sm_extruder1, snum, sdelay);
}

/**
 * Установить температуру для экструдера 1.
 */
void set_extruder1_temp(int temp) {
}

void handle_command(char* cmd_line) {
  char* cmd_name = "move";
  if(strcmp("move", cmd_name) == 0) {
  } else if(strcmp("heat", cmd_name) == 0) {
  
  }
}

void setup()
{
  pinMode(13, OUTPUT);
  // start serial port at 9600 bps:
  Serial.begin(9600);
  //establishContact();  // send a byte to establish contact until receiver responds 
  
  // init serial motors
  init_smotor(sm_x1, 0, 1, 2, 3);
  init_smotor(sm_x2, 4, 5, 6, 7);
  init_smotor(sm_y1, 8, 9, 10, 11);
  init_smotor(sm_y2, 12, 13, 14, 15);
  init_smotor(sm_z1, 16, 17, 18, 19);
  init_smotor(sm_z2, 20, 21, 22, 23);
  init_smotor(sm_extruder1, 24, 25, 26, 27);
  
  init_extruder(extruder1, 28);
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    if(inByte == 3) {
      digitalWrite(13, 0);
    } else {
      digitalWrite(13, 1);
    }    
    delay(10);    
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}

