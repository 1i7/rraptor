/**
 * stepper.cpp 
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
 * Инициализировать шаговый мотор необходимыми значениями.
 */
void init_stepper(stepper* smotor,  char name, 
        int pin_pulse, int pin_dir, int pin_en,
        int dir_inv, int pulse_delay,
        double distance_per_step, double max_pos) {
  
    smotor->name = name;
    
    smotor->pin_pulse = pin_pulse;
    smotor->pin_dir = pin_dir;
    smotor->pin_en = pin_en;
    
    smotor->dir_inv = dir_inv;
    smotor->pulse_delay = pulse_delay;
    
    smotor->distance_per_step = distance_per_step;
    smotor->max_pos = max_pos;
    
    // задать настройки пинов
    pinMode(pin_pulse, OUTPUT);
    pinMode(pin_dir, OUTPUT);
    pinMode(pin_en, OUTPUT);
    
    // пока выключить мотор
    digitalWrite(pin_en, HIGH);
}

