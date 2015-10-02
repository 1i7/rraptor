namespace rraptor {

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
 * calibrate:
 * 1 - в режиме калибровки не проверяется выход за границы рабочей области.
 * 0 - обычный режим, при перемещении рабочего блока перемещаются границы рабочей области.
 */
int calibrate_mode = 1;

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
 * Повернуть двигатель на нужное количество циклов с нужной скоростью.
 * cnum - количество циклов - знаковое целое: если > 0, шагаем вперед, если < 0 - назад.
 * tdelay - пауза между циклами, миллисекунды.  
 */
void step_motor(smotor* sm, int cnum, int tdelay) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", cycle count=" + cnum + "]: ");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm->DIR_PIN, (dir * sm->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги и не забываем проверять выход за границы рабочей области
    while (i < cnum) {
        // пусть будет 2 шага в одном цикле
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        delay(tdelay);
        i++;
    }
    
    Serial.println(String("") + "Current position=" + sm->current_pos + ", motor=" + sm->name);
}

/**
 * Повернуть двигатель на нужное количество циклов с нужной скоростью.
 * cnum - количество циклов - знаковое целое: если > 0, шагаем вперед, если < 0 - назад.
 * tdelay - пауза между циклами, миллисекунды.  
 */
void step_motor_1(smotor* sm, int cnum, int tdelay) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", cycle count=" + cnum + "]: ");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm->DIR_PIN, (dir * sm->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги и не забываем проверять выход за границы рабочей области
    while (i < cnum) {
        // 1 шаг в одном цикле
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        delay(tdelay);
        i++;
    }
    
    Serial.println(String("") + "Current position=" + sm->current_pos + ", motor=" + sm->name);
}

/**
 * Повернуть двигатель на нужное количество циклов с нужной скоростью.
 * cnum - количество циклов - знаковое целое: если > 0, шагаем вперед, если < 0 - назад.
 * tdelay - пауза между циклами, миллисекунды.  
 */
void step_motor_2(smotor* sm, int cnum, int tdelay) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", cycle count=" + cnum + "]: ");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm->DIR_PIN, (dir * sm->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги и не забываем проверять выход за границы рабочей области
    while (i < cnum) {
        // 1 шаг в одном цикле
        digitalWrite(sm->PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay + tdelay*500);
        digitalWrite(sm->PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay + tdelay*500);
        
        i++;
    }
    
    Serial.println(String("") + "Current position=" + sm->current_pos + ", motor=" + sm->name);
}

void step_2motor(smotor* sm1, smotor* sm2, int cnum) {
    Serial.print(String("") + "Info: Stepping 2 motors [name1=" + sm1->name + ", name2=" + sm2->name + ", cycle count=" + cnum + "]");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm1->DIR_PIN, (dir * sm1->dir_inv > 0 ? HIGH : LOW));
    digitalWrite(sm2->DIR_PIN, (dir * sm2->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    // millis
    int tdelay = 0;
    
    // Ожидаемое время (для отладки):
    int est_time = (sm1->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги
    while (i < cnum) {
        // Оба мотора шагают одновременно
      
        // пусть будет 2 шага в одном цикле
        digitalWrite(sm1->PULSE_PIN, HIGH);
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, LOW);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, HIGH);
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, LOW);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);

        delay(tdelay);
        i++;
    }
}

void step_2motor2(smotor* sm1, smotor* sm2, int cnum) {
    Serial.print(String("") + "Info: Stepping 2 motors [name1=" + sm1->name + ", name2=" + sm2->name + ", cycle count=" + cnum + "]");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm1->DIR_PIN, (dir * sm1->dir_inv > 0 ? HIGH : LOW));
    digitalWrite(sm2->DIR_PIN, (dir * sm2->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    // millis
    int tdelay = 0;
    
    // Ожидаемое время (для отладки):
    int est_time = (sm1->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги
    while (i < cnum) {
        // Моторы шарают поочереди
      
        // пусть будет 2 шага в одном цикле
        // Мотор1
        digitalWrite(sm1->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        digitalWrite(sm1->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);
        digitalWrite(sm1->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        digitalWrite(sm1->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);
        
        // Мотор2
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm2->step_delay);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm2->step_delay);
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm2->step_delay);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm2->step_delay);

        delay(tdelay);
        i++;
    }
}

void step_2motor2_1(smotor* sm1, smotor* sm2, int cnum) {
    Serial.print(String("") + "Info: Stepping 2 motors [name1=" + sm1->name + ", name2=" + sm2->name + ", cycle count=" + cnum + "]");
    // задать направление в зависиомости от знака
    int dir = (cnum > 0 ? 1 : -1);
    digitalWrite(sm1->DIR_PIN, (dir * sm1->dir_inv > 0 ? HIGH : LOW));
    digitalWrite(sm2->DIR_PIN, (dir * sm2->dir_inv > 0 ? HIGH : LOW));
    
    cnum = abs(cnum);
    
    // millis
    int tdelay = 0;
    
    // Ожидаемое время (для отладки):
    int est_time = (sm1->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    // Считаем шаги
    while (i < cnum) {
        // Моторы шарают поочереди
      
        // 1 шаг в одном цикле
        // Мотор1
        digitalWrite(sm1->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        digitalWrite(sm1->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);
        
        // Мотор2
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm2->step_delay);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm2->step_delay);

        delay(tdelay);
        i++;
    }
}


void step_2motor3(smotor* sm1, smotor* sm2, int cnum1, int cnum2) {
    Serial.print(String("") + "Info: Stepping 2 motors [name1=" + sm1->name + ", name2=" + sm2->name + ", cycle count1=" + cnum1 + ", cycle count2=" + cnum2 + "]");
    // задать направление в зависиомости от знака
    int dir1 = (cnum1 > 0 ? 1 : -1);
    digitalWrite(sm1->DIR_PIN, (dir1 * sm1->dir_inv > 0 ? HIGH : LOW));
    
    int dir2 = (cnum2 > 0 ? 1 : -1);
    digitalWrite(sm2->DIR_PIN, (dir2 * sm2->dir_inv > 0 ? HIGH : LOW));
    
    cnum1 = abs(cnum1);
    cnum2 = abs(cnum2);
    
    // millis
    int tdelay = 0;
    
    int i=0;
    // Считаем шаги
    while (i < cnum1) {
        // пусть будет 2 шага в одном цикле
        digitalWrite(sm1->PULSE_PIN, HIGH);
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, LOW);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, HIGH);
        digitalWrite(sm2->PULSE_PIN, HIGH);
        delayMicroseconds(sm1->step_delay);
        
        digitalWrite(sm1->PULSE_PIN, LOW);
        digitalWrite(sm2->PULSE_PIN, LOW);
        delayMicroseconds(sm1->step_delay);

        delay(tdelay);
        i++;
    }
}

void step_2motor4(smotor* sm1, smotor* sm2, int cnum1, int cnum2, int steps_per_cycle1, int steps_per_cycle2) {
    Serial.print(String("") + "Info: Stepping 2 motors [name1=" + sm1->name + ", name2=" + sm2->name + ", cycle count1=" + cnum1 + ", cycle count2=" + cnum2 + "]");
    // задать направление в зависиомости от знака
    int dir1 = (cnum1 > 0 ? 1 : -1);
    digitalWrite(sm1->DIR_PIN, (dir1 * sm1->dir_inv > 0 ? HIGH : LOW));
    
    int dir2 = (cnum2 > 0 ? 1 : -1);
    digitalWrite(sm2->DIR_PIN, (dir2 * sm2->dir_inv > 0 ? HIGH : LOW));
    
    cnum1 = abs(cnum1);
    cnum2 = abs(cnum2);

    int cycle_count = cnum1;
    
    // millis
    int tdelay = 0;
    
    int i=0;
    // Считаем шаги
    while (i < cycle_count) {
        // Мотор1
        for(int j1 = 0; j1 < steps_per_cycle1; j1++) {
            digitalWrite(sm1->PULSE_PIN, HIGH);
            delayMicroseconds(sm1->step_delay);
            digitalWrite(sm1->PULSE_PIN, LOW);
            delayMicroseconds(sm1->step_delay);
        }
        
        // Мотор2
        for(int j2 = 0; j2 < steps_per_cycle2; j2++) {
            digitalWrite(sm2->PULSE_PIN, HIGH);
            delayMicroseconds(sm2->step_delay);
            digitalWrite(sm2->PULSE_PIN, LOW);
            delayMicroseconds(sm2->step_delay);
        }

        delay(tdelay);
        i++;
    }
}

}

void setup() {
    // start serial port at 9600 bps:
    Serial.begin(9600);
    
    // init stepper motors
    // 500-2000 microseconds optimal
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
}

void test1_coords(int tdelay) {
    rraptor::step_motor(&rraptor::sm_x, 1000, tdelay);
    rraptor::step_motor(&rraptor::sm_x, -1000, tdelay);
    
    rraptor::step_motor(&rraptor::sm_y, 1000, tdelay);
    rraptor::step_motor(&rraptor::sm_y, -1000, tdelay);
    
    //rraptor::step_motor(&rraptor::sm_z, 1000, tdelay);
    //rraptor::step_motor(&rraptor::sm_z, -1000, tdelay);
}

void test2_square() {
    rraptor::step_motor(&rraptor::sm_x, 2000, 0);
    rraptor::step_motor(&rraptor::sm_y, 2000, 0);
    rraptor::step_motor(&rraptor::sm_x, -2000, 0);
    rraptor::step_motor(&rraptor::sm_y, -2000, 0);
}

void test3_line45() {
    rraptor::step_2motor(&rraptor::sm_x, &rraptor::sm_y, 2000);
    rraptor::step_2motor(&rraptor::sm_x, &rraptor::sm_y, -2000);
}

void test4_triangle1() {
    rraptor::step_motor(&rraptor::sm_y, 3000, 0);
    rraptor::step_motor(&rraptor::sm_x, 3000, 0);
    rraptor::step_2motor(&rraptor::sm_x, &rraptor::sm_y, -3000);
}

void test4_triangle2() {
    rraptor::step_motor(&rraptor::sm_x, 4000, 0);
    rraptor::step_motor(&rraptor::sm_y, 4000, 0);
    rraptor::step_2motor(&rraptor::sm_x, &rraptor::sm_y, -4000);  
}

void test5_line45_2() {
    rraptor::step_2motor2(&rraptor::sm_x, &rraptor::sm_y, 2000);
    rraptor::step_2motor2(&rraptor::sm_x, &rraptor::sm_y, -2000);
}

void test6_line45_3() {
    rraptor::step_2motor2_1(&rraptor::sm_x, &rraptor::sm_y, 2000);
    rraptor::step_2motor2_1(&rraptor::sm_x, &rraptor::sm_y, -2000);
}

void test7_triangle3() {
    rraptor::step_2motor3(&rraptor::sm_x, &rraptor::sm_y, 4000, 4000);
    rraptor::step_2motor3(&rraptor::sm_x, &rraptor::sm_y, -4000, 4000);
    rraptor::step_motor(&rraptor::sm_y, -4000, 0);
    rraptor::step_2motor3(&rraptor::sm_x, &rraptor::sm_y, 4000, -4000);
    rraptor::step_motor(&rraptor::sm_x, -4000, 0);
}

void test8_coords_1(int tdelay) {
    rraptor::step_motor_1(&rraptor::sm_x, 1000, tdelay);
    rraptor::step_motor_1(&rraptor::sm_x, -1000, tdelay);
    
    rraptor::step_motor_1(&rraptor::sm_y, 1000, tdelay);
    rraptor::step_motor_1(&rraptor::sm_y, -1000, tdelay);
    
    //rraptor::step_motor_1(&rraptor::sm_z, 1000, tdelay);
    //rraptor::step_motor_1(&rraptor::sm_z, -1000, tdelay);
}

void test9_coords_2(int tdelay) {
    rraptor::step_motor_2(&rraptor::sm_x, 1000, tdelay);
    rraptor::step_motor_2(&rraptor::sm_x, -1000, tdelay);
    
    rraptor::step_motor_2(&rraptor::sm_y, 1000, tdelay);
    rraptor::step_motor_2(&rraptor::sm_y, -1000, tdelay);
    
    //rraptor::step_motor_2(&rraptor::sm_z, 1000, tdelay);
    //rraptor::step_motor_2(&rraptor::sm_z, -1000, tdelay);
}

void test10_line_angles() {  
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, 1000, 1000, 1, 1);
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, -1000, -1000, 1, 1);
    
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, 1000, 1000, 1, 2);
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, -1000, -1000, 1, 2);
    
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, 1000, 1000, 1, 3);
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, -1000, -1000, 1, 3);
    
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, 1000, 1000, 1, 4);
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, -1000, -1000, 1, 4);
    
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, 1000, 1000, 2, 3);
    step_2motor4(&rraptor::sm_x, &rraptor::sm_y, -1000, -1000, 2, 3);
}

void metatest1_demodrawing1() {
    test1_coords(0);
    delay(1000);
    test2_square();
    delay(1000);
    test3_line45();    
    delay(1000);
    test4_triangle1();    
    delay(1000);
    test4_triangle2();    
    delay(1000);
}

void metatest2_demodrawing2() {
    test7_triangle3();
    delay(1000);
}

void metatest3_lines45() {
    test3_line45();    
    delay(1000);
    test5_line45_2();    
    delay(1000);
    test6_line45_3();    
    delay(1000);
}

void metatest4_delay_strategies() {
      // норм
    test1_coords(0);
    delay(1000);
      // так себе - двойной шаг с tdelay
    test1_coords(3);
    delay(1000);
    // норм - одинарный шаг с tdelay
    test8_coords_1(3);
    delay(1000);
    // при tdelay=3 еще норм, т.к. задержка между импульсами получается 500+3*500=2000
    test9_coords_2(3);
    delay(1000);
    // а здесь уже УГ, т.е. играть со step_delay между импульсами плохо - она должна умещаться в диапазон 500-2000 (5 еще норм, но на грани).
    test9_coords_2(7);
    delay(1000);    
    // хотя нифига - test8_coords_1(7); дает ту же хрень, что и test9_coords_2(7) - дело не в шаге между импульсами, а в частоте шагов вообще
    // УГ - одинарный шаг с большим tdelay
    test8_coords_1(7);
}

void loop() {
//    rraptor::step_motor(&rraptor::sm_x, -500, 0);
//    rraptor::step_motor(&rraptor::sm_y, -500, 0);

//    test1_coords(0);
//    delay(1000);
//    test1_coords(3);
//    delay(1000);
//    test2_square();
//    delay(1000);
//    test3_line45();    
//    delay(1000);
//    test4_triangle1();    
//    delay(1000);
//    test4_triangle2();    
//    delay(1000);
//    test5_line45_2();    
//    delay(1000);
//    test6_line45_3();    
//    delay(1000);
//    test7_triangle3();
//    delay(1000);
      test10_line_angles();
      delay(1000);

//    metatest1_demodrawing1();
//    delay(1000);
//    metatest2_demodrawing2();
//    delay(1000);
//    metatest3_lines45();
//    delay(1000);
//    metatest4_delay_strategies();
//    delay(1000);
}
