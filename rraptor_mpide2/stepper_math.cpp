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
 * @param sm - мотор на выбранной координате
 * @param dl - сдвиг по указанной оси, мм
 * @param spd - скорость перемещения, мм/с
 */
void prepare_line(stepper *sm, double dl, double spd) {
    Serial.print("prepare line:");
    Serial.print(" dl=");
    Serial.print(dl, DEC);
    Serial.print("mm, speed=");
    Serial.print(spd, DEC);
    Serial.println("mm/s");
    
    int steps;
    int mod_steps;
    int step_delay;
        
    steps = dl * 1000 / sm->distance_per_step;
    mod_steps = steps >= 0 ? steps : -steps;
    
    Serial.print("steps=");
    Serial.print(steps, DEC);
    Serial.println();
    
    // время на прохождение линии - длина делить на скорость, секунды
    double dt = dl / spd;
    // задержка между 2мя шагами, микросекунды
    step_delay = dt * 1000000 / mod_steps - sm->pulse_delay;
    
    Serial.print("step_delay(1)=");
    Serial.print(step_delay, DEC);
    Serial.println();

    step_delay = step_delay > 0 ? step_delay : 0;
    
    prepare_steps(sm, steps, step_delay);
}

/**
 * @param dl1 - сдвиг по оси 1, мм
 * @param dl2 - сдвиг по оси 2, мм
 * @param spd - скорость перемещения, мм/с
 */
void prepare_line_2d(stepper *sm1, stepper *sm2, double dl1, double dl2, double spd) {
    Serial.print("prepare line:");
    Serial.print(" d");
    Serial.print(sm1->name);
    Serial.print("=");
    Serial.print(dl1, DEC);
    Serial.print("mm, d");
    Serial.print(sm2->name);
    Serial.print("=");
    Serial.print(dl2, DEC);
    Serial.print("mm, speed=");
    Serial.print(spd, DEC);
    Serial.println("mm/s");
    
    int steps_sm1;
    int steps_sm2;
    int mod_steps_sm1;
    int mod_steps_sm2;
    int step_delay_sm1;
    int step_delay_sm2;
    
    steps_sm1 = dl1 * 1000 / sm1->distance_per_step;
    steps_sm2 = dl2 * 1000 / sm2->distance_per_step;
    
    mod_steps_sm1 = steps_sm1 >= 0 ? steps_sm1 : -steps_sm1;
    mod_steps_sm2 = steps_sm2 >= 0 ? steps_sm2 : -steps_sm2;
    
    Serial.print("steps_x=");
    Serial.print(steps_sm1, DEC);
    Serial.print(", steps_y=");
    Serial.print(steps_sm2, DEC);
    Serial.println();
    
    // длина гипотенузы
    double dl = sqrt(dl1*dl1 + dl2*dl2);
    // время на прохождение диагонали - длина делить на скорость, секунды
    double dt = dl / spd;
    // задержка между 2мя шагами, микросекунды
    step_delay_sm1 = dt * 1000000 / mod_steps_sm1 - sm1->pulse_delay;    
    step_delay_sm2 = dt * 1000000 / mod_steps_sm2 - sm2->pulse_delay;
    
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

