#include<stdio.h>
#include<math.h>

int main() {
    // радиус окружности, мм
    double r = 10;
    // скорость перемещения, мм/с
    double f = 3;
    // начнем движение из крайней правой точки против часовой стрелки
    double x_0=r, y_0=0;
    
    // шаг по x
    double dx = 0.1;
    // шаг по y
    double dy = 0.1;
    
    // завершим движение в крайней левой точке окружности
    //double x_1=-r, y_1=0;
    
    // целевое значение x и y
    double x, y;
    // время до достижения целевого x и y
    double tx, ty;
    
    // тест 1:
    // - пробежимся по всем точкам для x и y
    // - посчитаем время достижения каждой из 
    // точек из стартовой точки
    printf("###################### test1 ######################\n");
    for(x = r; x >= -r; x-=dx) {
        y = sqrt(r*r - x*x);
        
        tx = r/f*acos(x/r);
        ty = r/f*asin(y/r);
        printf("x=%f, tx=%f; y=%f, ty=%f\n", x, tx, y, ty);
    }
    
    //
    double tx_prev = 0, ty_prev = 0;
    
    // тест 2 
    printf("###################### test2 ######################\n");
    for(x = r; x >= -r; x-=dx) {        
        tx = r/f*acos(x/r);
        printf("x=%f, tx=%f\n", x, tx-tx_prev);
        tx_prev = tx;
    }
    for(y = 0; y <= r; y+=dy) {        
        ty = r/f*asin(y/r);
        printf("y=%f, ty=%f\n", y, ty-ty_prev);
        ty_prev = ty;
    }
    
    return 0;
}

