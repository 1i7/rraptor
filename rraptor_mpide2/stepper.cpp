
#include"stepper.h"

void init_stepper(stepper* sm,  char* name, 
        int pin_pulse, int pin_dir, int pin_en,
        int dir_inv, int pulse_delay,
        float distance_per_step, int max_pos) {
  
    sm->name = name;
    
    sm->PIN_PULSE = pin_pulse;
    sm->PIN_DIR = pin_dir;
    sm->PIN_EN = pin_en;
    
    sm->dir_inv = dir_inv;
    sm->pulse_delay = pulse_delay;
    
    sm->distance_per_step = distance_per_step;  
    sm->MAX_POS = max_pos;
}

