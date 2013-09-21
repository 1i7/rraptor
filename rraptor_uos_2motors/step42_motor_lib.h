
#ifndef _step42_motor_lib_
#define _step42_motor_lib_

typedef struct step_data {
    int step;
    float time;
    int dir;
    int MOTOR_PIN1;
    int MOTOR_PIN2;
    int MOTOR_PIN3;
    int MOTOR_PIN4;
    int microstep;
    
}step_data;

#endif
