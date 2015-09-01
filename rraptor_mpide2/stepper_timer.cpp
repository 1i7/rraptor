/**
 * stepper_timer.cpp 
 *
 * Библиотека управления шаговыми моторами, подключенными через интерфейс 
 * драйвера "step-dir".
 *
 * LGPL, 2014
 *
 * @author Антон Моисеев
 */
 
#include "WProgram.h"

extern "C"{
    #include "timer_setup.h"
}

#include "stepper.h"

/**
 * Статус цикла вращения мотора
 */
typedef enum {
    /** Ожидает запуска */
    IDLE, 
    
    /** Вращается */
    RUNNING, 
    
    /** Завершил вращение нормально */
    FINISHED_OK, 
    
    /** Завершил вращение из-за срабатывания концевого датчика нижней границы */
    HARD_END_MIN, 
    
    /** Завершил вращение из-за срабатывания концевого датчика верхней границы */
    HARD_END_MAX, 
    
    /** Завершил вращение из-за достижения виртуальной нижней границы */
    SOFT_END_MIN, 
    
    /** Завершил вращение из-за достижения виртуальной верхней границы */
    SOFT_END_MAX} motor_cycle_status_t;

/**
 * Структура - статус текущего цикла мотора.
 */
typedef struct {
//// Настройки для текущего цикла шагов

    /** 
     * Направление движения
     *  1: вперед (увеличение виртуальной координаты curr_pos), 
     * -1: назад (уменьшение виртуальной координаты curr_pos) 
     */
    int dir;
    
    /** true: вращение без остановки, false: использовать step_count */
    bool non_stop;

    /** Количество шагов в текущей серии (если non_stop=false) */
    int step_count;
    
    /**
     * Задержка между 2мя шагами мотора (определяет скорость вращения, 
     * 0 для максимальной скорости) 
     */
    int step_delay;
    
    /** Режим калибровки */
    calibrate_mode_t calibrate_mode;
    
    /**
     * Проверять выход за виртуальные границы координаты при движении (аппаратные проверяются ВСЕГДА).
     * true: останавливать вращение при выходе за допустимые границы координаты (0, max_pos), 
     * false: не проверять границы (сбрасывать current_pos в 0 при каждом шаге).
     */
    //bool check_bounds;    

//// Динамика
    /** Статус цикла */
    motor_cycle_status_t cycle_status;

    /** Счетчик шагов для текущей серии (убывает) */
    int step_counter;
    /** Счетчик микросекунд для текущего шага (убывает) */
    int step_timer;
} motor_cycle_info_t;

#ifndef MAX_STEPPERS
#define MAX_STEPPERS 6
#endif

int stepper_count = 0;
stepper* smotors[MAX_STEPPERS];
motor_cycle_info_t cstatuses[MAX_STEPPERS];

// Частота таймера, мкс
int timer_freq_us;

// Текущий статус цикла
bool cycle_running = false;


/**
 * Подготовить мотор к запуску ограниченной серии шагов - задать нужное количество 
 * шагов и задержку между шагами для регулирования скорости (0 для максимальной скорости).
 * 
 * @param step_count количество шагов, знак задает направление вращения
 * @param step_delay задержка между двумя шагами, микросекунды (0 для максимальной скорости).
 */
void prepare_steps(stepper *smotor, int step_count, int step_delay) {
    // резерв нового места на мотор в списке
    int sm_i = stepper_count;
    stepper_count++;
    
    // ссылка на мотор
    smotors[sm_i] = smotor;
        
    // Подготовить движение
  
    // задать направление
    cstatuses[sm_i].dir = step_count > 0 ? 1 : -1;
    if(cstatuses[sm_i].dir * smotor->dir_inv > 0) {
        digitalWrite(smotors[sm_i]->pin_dir, HIGH); // туда
    } else {
        digitalWrite(smotors[sm_i]->pin_dir, LOW); // обратно
    }
    
    // шагаем ограниченное количество шагов
    cstatuses[sm_i].non_stop = false;
    // сделать step_count положительным
    cstatuses[sm_i].step_count = step_count > 0 ? step_count : -step_count;
    
    // скорость вращения
    cstatuses[sm_i].step_delay = step_delay;
    
    // выключить режим калибровки
    cstatuses[sm_i].calibrate_mode = NONE;
  
    // Взводим счетчики
    cstatuses[sm_i].step_counter = cstatuses[sm_i].step_count;
    cstatuses[sm_i].step_timer = smotors[sm_i]->pulse_delay + cstatuses[sm_i].step_delay;
    
    // ожидаем пуска
    cstatuses[sm_i].cycle_status = IDLE;
}

/**
 * Подготовить мотор к запуску на вращение - задать направление и задержку между
 * шагами для регулирования скорости (0 для максимальной скорости).
 *
 * @param dir направление вращения: 1 - вращать вперед (увеличиваем curr_pos), -1 - назад (уменьшаем curr_pos).
 * @param step_delay задержка между двумя шагами, микросекунды (0 для максимальной скорости).
 * @param calibrate_mode - режим калибровки
 *     NONE: режим калибровки выключен - останавливать вращение при выходе за виртуальные границы 
 *           рабочей области [min_pos, max_pos] (аппаратные проверяются ВСЕГДА);
 *     CALIBRATE_START_MIN_POS: установка начальной позиции (сбрасывать current_pos в min_pos при каждом шаге);
 *     CALIBRATE_BOUNDS_MAX_POS: установка размеров рабочей области (сбрасывать max_pos в current_pos при каждом шаге).
 */
void prepare_whirl(stepper *smotor, int dir, int step_delay, calibrate_mode_t calibrate_mode) {
    // резерв нового места на мотор в списке
    int sm_i = stepper_count;
    stepper_count++;
    
    // ссылка на мотор
    smotors[sm_i] = smotor;
    
    // Подготовить движение
  
    // задать направление
    cstatuses[sm_i].dir = dir;
    if(cstatuses[sm_i].dir * smotor->dir_inv > 0) {
        digitalWrite(smotors[sm_i]->pin_dir, HIGH); // туда
    } else {
        digitalWrite(smotors[sm_i]->pin_dir, LOW); // обратно
    }
    
    // шагаем без остановки
    cstatuses[sm_i].non_stop = true;
    
    // скорость вращения
    cstatuses[sm_i].step_delay = step_delay;
    
    // режим калибровки
    cstatuses[sm_i].calibrate_mode = calibrate_mode;
  
    // взводим счетчики
    cstatuses[sm_i].step_timer = smotors[sm_i]->pulse_delay + cstatuses[sm_i].step_delay;
    
    // на всякий случай обнулим
    cstatuses[sm_i].step_count = 0;
    cstatuses[sm_i].step_counter = 0;
    
    // ожидаем пуска
    cstatuses[sm_i].cycle_status = IDLE;
}

/**
 * Запустить цикл шагов на выполнение - запускаем таймер, обработчик прерываний
 * отрабатывать подготовленную программу.
 */
void start_stepper_cycle() {
    cycle_running = true;
    
    // включить моторы
    for(int i = 0; i < stepper_count; i++) {
        cstatuses[i].cycle_status = RUNNING;
        digitalWrite(smotors[i]->pin_en, LOW);
    }
    
    // для частоты 1 микросекунда (1млн операций в секунду):
    // 80000000/8/1000000=10=0xA
    // (уже подкглючивает)
//    timer_freq_us = 1;
//    initTimerISR(TIMER3, TIMER_PRESCALER_1_8, 0xA);
    
    // для частоты 5 микросекунд (500000 операций в секунду):
    // 80000000/8/500000=20
//    timer_freq_us = 5;
//    initTimerISR(TIMER3, TIMER_PRESCALER_1_8, 20);
    
    // Запустим таймер с периодом 10 микросекунд (100тыс операций в секунду):
    // 80000000/8/100000=100=0x64
    timer_freq_us = 10;
    initTimerISR(TIMER3, TIMER_PRESCALER_1_8, 0x64);
}

/**
 * Завершить цикл шагов - остановить таймер, обнулить список моторов.
 */
void finish_stepper_cycle() {
    // остановим таймер
    stopTimerISR(TIMER3);
        
    // выключим все моторы
    for(int i = 0; i < stepper_count; i++) {
        // выключить мотор
        digitalWrite(smotors[i]->pin_en, HIGH);
    }
    
    // цикл завершился
    cycle_running = false;
    
    // обнулим список моторов
    stepper_count = 0;
}

/**
 * Текущий статус цикла:
 * true - в процессе выполнения,
 * false - ожидает.
 */
bool is_cycle_running() {
    return cycle_running;
}

/**
 * Обработчик прерывания от таймера - дёргается каждые timer_freq_us микросекунд.
 */
void handle_interrupts(int timer) {
    // завершился ли цикл - все моторы закончили движение
    bool finished = true;
    
    for(int i = 0; i < stepper_count; i++) {
        cstatuses[i].step_timer -= timer_freq_us;
        
        
        if( (smotors[i]->pin_min != -1 && digitalRead(smotors[i]->pin_min)) || (smotors[i]->pin_max != -1 && digitalRead(smotors[i]->pin_max)) ) {
            // сработал один из аппаратных концевых датчиков - завершаем вращение для этого мотора
            
            // обновим статус мотора
            if(cstatuses[i].dir < 0) {
                cstatuses[i].cycle_status = HARD_END_MIN;
            } else {
                cstatuses[i].cycle_status = HARD_END_MAX;
            }
            
        } else if( cstatuses[i].calibrate_mode == NONE && 
                (cstatuses[i].dir > 0 ? smotors[i]->current_pos + smotors[i]->distance_per_step > smotors[i]->max_pos :
                                        smotors[i]->current_pos - smotors[i]->distance_per_step < smotors[i]->min_pos) ) {
            // не в режиме калибровки и собираемся выйти за виртуальные границы во время предстоящего шага - завершаем вращение для этого мотора
            
            // обновим статус мотора
            if(cstatuses[i].dir < 0) {
                cstatuses[i].cycle_status = SOFT_END_MIN;
            } else {
                cstatuses[i].cycle_status = SOFT_END_MAX;
            }
            
        } else if( cstatuses[i].calibrate_mode == CALIBRATE_BOUNDS_MAX_POS &&
                cstatuses[i].dir < 0 && smotors[i]->current_pos - smotors[i]->distance_per_step < smotors[i]->min_pos ) {
            // в режиме калибровки размера рабочей области собираемся сместиться ниже нижней виртуальной границы
            // во время предстоящего шага - завершаем вращение для этого мотора
            
            // обновим статус мотора
            cstatuses[i].cycle_status = SOFT_END_MIN;
            
        } else  if( cstatuses[i].non_stop || cstatuses[i].step_counter > 0 ) {
            // если хотя бы у одного мотора остались шаги или он запущен нон-стоп,
            // то мы еще не закончили
            finished = false;
          
            // Шаг происходит по фронту сигнала HIGH>LOW, ширина ступени HIGH при этом не важна.
            // Поэтому сформируем ступень HIGH за один цикл таймера до сброса в LOW
            if(cstatuses[i].step_timer < timer_freq_us * 2 && cstatuses[i].step_timer >= timer_freq_us) {
                // cstatuses[i].step_timer ~ timer_freq_us с учетом погрешности таймера (timer_freq_us) =>
                // импульс1 - готовим шаг
                digitalWrite(smotors[i]->pin_step, HIGH);
            } else if(cstatuses[i].step_timer < timer_freq_us) {
                // cstatuses[i].step_timer ~ 0 с учетом погрешности таймера (timer_freq_us) =>
                // импульс2 (спустя timer_freq_us микросекунд после импульса1) - совершаем шаг
                digitalWrite(smotors[i]->pin_step, LOW);
                
                // посчитаем шаг
                if(!cstatuses[i].non_stop) {
                    cstatuses[i].step_counter--;
                }
                
                if(cstatuses[i].calibrate_mode == NONE || cstatuses[i].calibrate_mode == CALIBRATE_BOUNDS_MAX_POS) {
                    // обновим текущее положение координаты
                    if(cstatuses[i].dir > 0) {
                        smotors[i]->current_pos += smotors[i]->distance_per_step;
                    } else {
                        smotors[i]->current_pos -= smotors[i]->distance_per_step;
                    }
                    
                    // калибруем ширину рабочего поля - сдвинем правую границу в текущее положение
                    if(cstatuses[i].calibrate_mode == CALIBRATE_BOUNDS_MAX_POS) {
                        smotors[i]->max_pos = smotors[i]->current_pos;
                    }
                } else if(cstatuses[i].calibrate_mode == CALIBRATE_START_MIN_POS) {
                    // режим калибровки начального положения - сбрасываем current_pos в min_pos на каждом шаге
                    smotors[i]->current_pos = smotors[i]->min_pos;
                }
                
                // сделали последний шаг - установим статус мотора
                if(!cstatuses[i].non_stop && cstatuses[i].step_counter == 0) {
                    cstatuses[i].cycle_status = FINISHED_OK;
                    
                    #ifdef DEBUG_SERIAL
                        Serial.print("Finished motor=");
                        Serial.print(smotors[i]->name);
                        Serial.print(".pos:");
                        Serial.print(smotors[i]->current_pos);
                        Serial.print("um, curr time=");
                        Serial.print(millis(), DEC);
                        Serial.println("ms");
                    #endif // DEBUG_SERIAL
                }
                
                // взведём таймер на новый шаг
                cstatuses[i].step_timer = smotors[i]->pulse_delay + cstatuses[i].step_delay;
            }
        }
    }
    
    if(finished) {
        // все моторы сделали все шаги, цикл завершился
        finish_stepper_cycle();
    }
}

