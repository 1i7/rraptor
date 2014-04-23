#ifndef STEPPER_H
#define STEPPER_H

/**
 * Структура - шаговый двигатель.
 */
typedef struct {
    /* Имя шагового мотора */
    char* name;
    
    /* Информация о подключение через драйвер Step-dir */
    int PIN_PULSE;
    int PIN_DIR;   
    int PIN_EN;
    
    /**
     * Инверсия направления: 
     * при dir_inv=1 и PIN_DIR=1 направление 0->1, 
     * при dir_inv=-1 и PIN_DIR=1 направление 1->0 
     */
    int dir_inv;
    
    /* Задержка между импульсами, микросекунды */
    int pulse_delay;
    
    /* Информация о движении координаты, подключенной к мотору */
    
    /* Расстояние, проходимое координатой за шаг, микрометры */
    float distance_per_step;
    
    /* Максимальное значение положения координаты */
    int MAX_POS;
    /* Текущее положение координаты */
    int current_pos;
} stepper;

/**
 * Инициировать шаговый мотор необходимыми значениями.
 */
void init_stepper(stepper* sm,  char* name, 
        int pin_pulse, int pin_dir, int pin_en,
        int dir_inv, int pulse_delay,
        float distance_per_step, int max_pos);

#endif // STEPPER_H

