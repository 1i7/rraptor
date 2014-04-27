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
 * @param dx - сдвиг по x, мм
 * @param dy - сдвиг по y, мм
 * @param dt - время на перемещение, секунды.
 * 
 */
void prepare_line(stepper *sm_x, stepper *sm_y, float dx, float dy, float dt) {
    Serial.print("prepare line:");
    Serial.print(" dx=");
    Serial.print(dx, DEC);
    Serial.print(", dy=");
    Serial.print(dy, DEC);
    Serial.print(", dt=");
    Serial.print(dt, DEC);
    Serial.println();
    
    int steps_x;
    int steps_y;
    int mod_steps_x;
    int mod_steps_y;
    int step_delay_x;
    int step_delay_y;
    
    steps_x = dx * 1000 / sm_x->distance_per_step;
    steps_y = dy * 1000 / sm_y->distance_per_step;
    
    mod_steps_x = steps_x >= 0 ? steps_x : -steps_x;
    mod_steps_y = steps_y >= 0 ? steps_y : -steps_y;
    
    Serial.print("steps_x=");
    Serial.print(steps_x, DEC);
    Serial.print(", steps_y=");
    Serial.print(steps_y, DEC);
    Serial.println();
    
    step_delay_x = dt * 1000000 / mod_steps_x - sm_x->pulse_delay;    
    step_delay_y = dt * 1000000 / mod_steps_y - sm_y->pulse_delay;
    
    Serial.print("step_delay_x(1)=");
    Serial.print(step_delay_x, DEC);
    Serial.print(", step_delay_y(1)=");
    Serial.print(step_delay_y, DEC);
    Serial.println();

    step_delay_x = step_delay_x > 0 ? step_delay_x : 0;
    step_delay_y = step_delay_y > 0 ? step_delay_y : 0;
    
    Serial.print("step_delay_x=");
    Serial.print(step_delay_x, DEC);
    Serial.print(", step_delay_y=");
    Serial.print(step_delay_y, DEC);
    Serial.println();
    
    prepare_steps(sm_x, steps_x, step_delay_x);
    prepare_steps(sm_y, steps_y, step_delay_y);
}

