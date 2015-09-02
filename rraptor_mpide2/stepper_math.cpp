/**
 * stepper_math.cpp 
 *
 * Библиотека управления шаговыми моторами, подключенными через интерфейс 
 * драйвера "step-dir".
 *
 * LGPL, 2014
 *
 * @author Антон Моисеев
 */
 
#include "WProgram.h"
#include "stepper.h"

/**
 * @param sm - мотор на выбранной оси координат
 * @param cvalue - положение на указанной координате, мм
 * @param spd - скорость перемещения, мм/с, 0 для максимальное скорости
 */
void prepare_line(stepper *sm, double cvalue, double spd) {
    Serial.print("prepare line:");
    Serial.print(sm->name);
    Serial.print("1=");
    Serial.print(sm->current_pos/1000, DEC);
    Serial.print("mm, ");
    Serial.print(sm->name);
    Serial.print("2=");
    Serial.print(cvalue, DEC);
    Serial.print("mm, speed=");
    Serial.print(spd, DEC);
    Serial.println("mm/s");
    
    int steps;
    int mod_steps;
    int step_delay;
    
    // сдвиг по оси, микрометры
    double dl = cvalue * 1000 - sm->current_pos;
        
    steps = dl / sm->distance_per_step;
    mod_steps = steps >= 0 ? steps : -steps;
    
    Serial.print("steps=");
    Serial.print(steps, DEC);
    Serial.println();
    
    if(spd == 0) {
        // посчитаем максимально возможнную скорость, мм/с
        spd = 7.5;
    }
    // время на прохождение диагонали - длина делить на скорость, микросекунды
    // мм/с=мкм/млс; мкм/мкм/млс=млс; млс*1000=мкс
    double dt = (dl / spd) * 1000;
    
    // задержка между 2мя шагами, микросекунды
    step_delay = dt / mod_steps - sm->pulse_delay;
    
    Serial.print("step_delay=");
    Serial.print(step_delay, DEC);
    Serial.println("us");

    step_delay = step_delay > 0 ? step_delay : 0;
    
    prepare_steps(sm, steps, step_delay);
}

/**
 * @param cvalue1 - значение координаты 1, мм
 * @param cvalue2 - значение координаты 2, мм
 * @param spd - скорость перемещения, мм/с, 0 для максимальное скорости
 */
void prepare_line_2d(stepper *sm1, stepper *sm2, double cvalue1, double cvalue2, double spd) {
    Serial.print("prepare line:");
    Serial.print(" ");
    Serial.print(sm1->name);
    Serial.print("1=");
    Serial.print(sm1->current_pos / 1000, DEC);
    Serial.print("mm, ");
    Serial.print(sm1->name);
    Serial.print("2=");
    Serial.print(cvalue1, DEC);
    Serial.print("mm; ");
    Serial.print(sm2->name);
    Serial.print("1=");
    Serial.print(sm2->current_pos / 1000, DEC);
    Serial.print("mm, ");
    Serial.print(sm2->name);
    Serial.print("2=");
    Serial.print(cvalue2, DEC);
    Serial.print("mm; speed=");
    Serial.print(spd, DEC);
    Serial.println("mm/s");
    
    int steps_sm1;
    int steps_sm2;
    int mod_steps_sm1;
    int mod_steps_sm2;
    int step_delay_sm1;
    int step_delay_sm2;
    
    // сдвиг по оси, микрометры
    double dl1 = cvalue1 * 1000 - sm1->current_pos;
    double dl2 = cvalue2 * 1000 - sm2->current_pos;
    
    steps_sm1 = dl1 / sm1->distance_per_step;
    steps_sm2 = dl2 / sm2->distance_per_step;
    
    mod_steps_sm1 = steps_sm1 >= 0 ? steps_sm1 : -steps_sm1;
    mod_steps_sm2 = steps_sm2 >= 0 ? steps_sm2 : -steps_sm2;
    
    Serial.print("steps_x=");
    Serial.print(steps_sm1, DEC);
    Serial.print(", steps_y=");
    Serial.print(steps_sm2, DEC);
    Serial.println();
    
    // длина гипотенузы, микрометры
    double dl = sqrt(dl1*dl1 + dl2*dl2);
    if(spd == 0) {
        // посчитаем максимально возможнную скорость, мм/с
        spd = 7.5;
    }
    // время на прохождение диагонали - длина делить на скорость, микросекунды
    // мм/с=мкм/млс; мкм/мкм/млс=млс; млс*1000=мкс
    double dt = (dl / spd) * 1000;
    
    // задержка между 2мя шагами, микросекунды
    step_delay_sm1 = dt / mod_steps_sm1 - sm1->pulse_delay;    
    step_delay_sm2 = dt / mod_steps_sm2 - sm2->pulse_delay;
    
    Serial.print("step_delay_x(1)=");
    Serial.print(step_delay_sm1, DEC);
    Serial.print(", step_delay_y(1)=");
    Serial.print(step_delay_sm2, DEC);
    Serial.println();

    step_delay_sm1 = step_delay_sm1 > 0 ? step_delay_sm1 : 0;
    step_delay_sm2 = step_delay_sm2 > 0 ? step_delay_sm2 : 0;
    
    Serial.print("step_delay_x=");
    Serial.print(step_delay_sm1, DEC);
    Serial.print(", step_delay_y=");
    Serial.print(step_delay_sm2, DEC);
    Serial.println();
    
    prepare_steps(sm1, steps_sm1, step_delay_sm1);
    prepare_steps(sm2, steps_sm2, step_delay_sm2);
}


