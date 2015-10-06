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
 * Подготовить линейное перемещение из текущей позиции в заданную точку с заданной скоростью,
 * для одной координаты.
 *
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
    step_delay = dt / mod_steps;
    
    Serial.print("step_delay=");
    Serial.print(step_delay, DEC);
    Serial.println("us");

    step_delay = step_delay >= sm->pulse_delay ? step_delay : 0;
    
    prepare_steps(sm, steps, step_delay);
}

/**
 * Подготовить линейное перемещение из текущей позиции в заданную точку с заданной скоростью,
 * для двух координат.
 *
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
    step_delay_sm1 = dt / mod_steps_sm1;    
    step_delay_sm2 = dt / mod_steps_sm2;
    
    Serial.print("step_delay_x(1)=");
    Serial.print(step_delay_sm1, DEC);
    Serial.print(", step_delay_y(1)=");
    Serial.print(step_delay_sm2, DEC);
    Serial.println();

    step_delay_sm1 = step_delay_sm1 >= sm1->pulse_delay ? step_delay_sm1 : 0;
    step_delay_sm2 = step_delay_sm2 >= sm2->pulse_delay ? step_delay_sm2 : 0;
    
    Serial.print("step_delay_x=");
    Serial.print(step_delay_sm1, DEC);
    Serial.print(", step_delay_y=");
    Serial.print(step_delay_sm2, DEC);
    Serial.println();
    
    prepare_steps(sm1, steps_sm1, step_delay_sm1);
    prepare_steps(sm2, steps_sm2, step_delay_sm2);
}

typedef struct {
    // радиус окружности, мм
    double r;
    
    // скорость перемещения, мм/с
    double f;
    
    // стартовая дочка дуги
    double x_0;
    double y_0;
    
    // целевая точка дуги
    double x_1;
    double y_1;
    
    // шаг по x, микрометры
    double dx;
    // шаг по y, микрометры
    double dy;
    
    ////
    // Вычисляемые значения
    // радиус в микрометрах
    double r_mkm; // = r * 1000;
    // коэффициент для рассчета времени на прохождение пути по координате, микросекунды
    double k; // = r / f * 1000000;
    
    ////
    // Динамические значения
    // значение tx на предыдущей итерации
    double tx_prev;
    // значение ty на предыдущей итерации
    double ty_prev;    
} circle_context_t;

int next_step_delay_circle_x(int curr_step, void* circle_context) {
    circle_context_t* context = (circle_context_t*)circle_context;
    
    // время до достижения целевого x из положения a=0 (x=r, y=0)
    double tx;
    // время до достижения целевого x из текущего положения
    double dtx;
    
    // целовое положение координаты x
    double x = context->r_mkm - context->dx*(curr_step + 1);
      
    // сколько ехать до новой точки из положения a=0
    tx = context->k*acos(x/context->r_mkm);

    // сколько ехать до новой точки из текущего положения
    dtx = tx - context->tx_prev;
    
    // сохраним для следующей итерации (чтобы не вычислять два раза)
    context->tx_prev = tx;
    
    /*
    Serial.print("X: curr_step=");
    Serial.print(curr_step);
    Serial.print(", x=");
    Serial.print(x);
    Serial.print(", x/context->r_mkm=");
    Serial.print(x/context->r_mkm);
    Serial.print(", k=");
    Serial.print(context->k);
    Serial.print(", acos(x/context->r_mkm)=");
    Serial.print(acos(x/context->r_mkm));
    Serial.print(", tx=");
    Serial.print(tx);
    Serial.print(", dtx=");
    Serial.println(dtx);*/
    
    return dtx;
}

int next_step_delay_circle_y(int curr_step, void* circle_context) {
    circle_context_t* context = (circle_context_t*)circle_context;
    
    // время до достижения целевого y из положения a=0 (x=r, y=0)
    double ty;
    // время до достижения целевого y из текущего положения
    double dty;
    
    // целевое положение координаты y, микрометры
    double y = context->dy*(curr_step + 1);
      
    // сколько ехать до новой точки из положения a=0
    ty = context->k*asin(y/context->r_mkm);

    // сколько ехать до новой точки из текущего положения
    dty = ty - context->ty_prev;
    
    // сохраним для следующей итерации (чтобы не вычислять два раза)
    context->ty_prev = ty;
    
    /*Serial.print("Y: curr_step=");
    Serial.print(curr_step);
    Serial.print(", y=");
    Serial.print(y);
    Serial.print(", y/context->r_mkm=");
    Serial.print(y/context->r_mkm);
    Serial.print(", k=");
    Serial.print(context->k);
    Serial.print(", asin(y/context->r_mkm)=");
    Serial.print(asin(y/context->r_mkm));
    Serial.print(", ty=");
    Serial.print(ty);
    Serial.print(", dty=");
    Serial.println(dty);*/
    
    return dty;
}

circle_context_t circle_context;

void prepare_circle(stepper *sm1, stepper *sm2, double center_c1, double center_c2, double spd) {
}

/**
 * @param target_c1 - целевое значение координаты 1, мм
 * @param target_c2 - целевое значение координаты 2, мм
 * @param spd - скорость перемещения, мм/с, 0 для максимальное скорости
 */
void prepare_arc(stepper *sm1, stepper *sm2, double target_c1, double target_c2, double center_c1, double center_c2, double spd) {
}

/**
 * @param target_c1 - целевое значение координаты 1, мм
 * @param target_c2 - целевое значение координаты 2, мм
 * @param spd - скорость перемещения, мм/с, 0 для максимальное скорости
 */
void prepare_arc(stepper *sm1, stepper *sm2, double target_c1, double target_c2, double radius, double spd) {
    Serial.print("prepare arc:");
    Serial.print(" (");
    Serial.print(sm1->name);
    Serial.print("1=");
    Serial.print(sm1->current_pos / 1000, DEC);
    Serial.print("mm, ");
    Serial.print(sm2->name);
    Serial.print("1=");
    Serial.print(sm2->current_pos / 1000, DEC);
    Serial.print("mm) -> (");
    Serial.print(sm1->name);
    Serial.print("2=");
    Serial.print(target_c1, DEC);
    Serial.print("mm, ");
    Serial.print(sm2->name);
    Serial.print("2=");
    Serial.print(target_c2, DEC);
    Serial.print("mm); speed=");
    Serial.print(spd, DEC);
    Serial.println("mm/s");
      
    // радиус окружности, мм
    circle_context.r = radius;
    
    // скорость перемещения, мм/с
    circle_context.f = spd;
    
    // начнем движение из крайней правой точки против часовой стрелки, мм
    circle_context.x_0 = circle_context.r;
    circle_context.y_0 = 0;
    
    // завершим движение в верхней точке окружности, мм
    circle_context.x_1 = 0;
    circle_context.y_1 = circle_context.r;
    
    // шаг по x и y, микрометры
    circle_context.dx = sm1->distance_per_step;
    circle_context.dy = sm2->distance_per_step;
        
    // 
    // вычисляемые значения
    // радиус в микрометрах
    circle_context.r_mkm = circle_context.r * 1000;
    // коэффициент для вычисления задежки, микросекунды
    circle_context.k = 1000000*circle_context.r/circle_context.f;
    
    // обнулим динамические значения
    circle_context.tx_prev = 0;
    circle_context.ty_prev = 0;
    
    ///
    // количество шагов по координате
    int steps_sm1 = circle_context.r_mkm / sm1->distance_per_step;
    int steps_sm2 = circle_context.r_mkm / sm2->distance_per_step;
    
    
    Serial.print("steps-sm1=");
    Serial.print(steps_sm1);
    Serial.print(", steps-sm2=");
    Serial.println(steps_sm2);
    
    // колбэки для вычисления переменных промежутков между шагами
    int (*next_step_delay_sm1)(int, void*) = &next_step_delay_circle_x;
    int (*next_step_delay_sm2)(int, void*) = &next_step_delay_circle_y;
    
    // подготовим шаги с переменной скоростью
    prepare_curved_steps(sm1, steps_sm1, &circle_context, next_step_delay_sm1);
    prepare_curved_steps(sm2, steps_sm2, &circle_context, next_step_delay_sm2);
}


