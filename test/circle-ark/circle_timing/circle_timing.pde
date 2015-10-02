
void setup() {
    Serial.begin(9600);
    Serial.println("Circle timing calcs test");
}

void loop() {
    double r = 10;
    double x_1=-8, y_1=0;
    double f = 3;
    double tx, ty;
    
    double k = r/f;
    
    // посчитаем время для вычисления
    // времени движения на каждом шаге
    double t1, t2;
    

    for(x_1 = 10; x_1 >= -10; x_1-=0.1) {
        y_1 = sqrt(r*r - x_1*x_1);
        
        // время начала вычисления в микросекундах
        t1 = micros();
        
        // хоть так 13мкс
        //tx = r/f*acos(x_1/r);
        //ty = r/f*asin(y_1/r);
        // хоть эдак - те же 13мкс
        tx = k*acos(x_1/r);
        ty = k*asin(y_1/r);
        
        // так 1-2мкс (вычисление ничего не значит)
        //tx = x_1/5;
        //ty = y_1/10;
        
        // типа концевые датчики (6 на 3 координаты): 
        // 4-5мкс на 6 чтений, 2-3мкс на 2 чтения, 1-2 на одно
        digitalRead(13);
        digitalRead(14);
        digitalRead(15);
        digitalRead(16);
        digitalRead(17);
        digitalRead(18);
        
        // если все вообще закоментить, то время между t1 и t2 будет 1-2мкс
        
        // время конца вычисления в микросекундах 
        // (в миллисекундах на PIC32 время вычисления будет 0)
        t2 = micros();
        
        Serial.print("t1=");
        Serial.print(t1);
        
        Serial.print("; t2=");
        Serial.print(t2);
        
        // на ChipKIT WF32 с PIC32 время вычисления:
        // 13 микросекунд для 2х координат (пики от 9 мкс вниз до 15 мкс вверх) - 
        // в таймер 10мкс уже не умещаемся
        // 16-17 мкс для 2х координат + 6 "концевых датчиков"
        Serial.print(", dt=");
        Serial.println(t2-t1);
        //printf("x=%f, tx=%f; y=%f, ty=%f\n", x_1, tx, y_1, ty);
    }
}

