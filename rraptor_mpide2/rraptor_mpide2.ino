#include "rraptor_config.h"
#include "rraptor_protocol.h"

#include "stepper.h"
#include "stepper_math.h"

// без прямого импорта здесь эти библиотеки не находят импорты 
// в других cpp-файлах и при компиляции происходят ошибки
// (походу, они не добавляются к строке компилятора)

// для последовательного порта Serial
#ifdef RR_SERIAL
#include "rraptor_serial.h"
#endif // RR_SERIAL

// для TCP
#ifdef RR_TCP
#include "rraptor_tcp.h"
#endif // RR_TCP

// для Сервера Роботов
#ifdef RR_ROBOT_SERVER
#include "rraptor_robot_server.h"
#endif // RR_ROBOT_SERVER

// для USB Accessory
#ifdef RR_USB_ACCESSORY
#include "rraptor_usb_accessory.h"
#endif // RR_USB_ACCESSORY

// Шаговые моторы
static stepper sm_x, sm_y, sm_z;

static void prepare_line1() {
    //prepare line: dx=30.0000000000, dy=20.0000000000, dt=8.0000000000
    //steps_x=4000, steps_y=2666
    //step_delay_x(1)=1000, step_delay_y(1)=2000
    //step_delay_x=1000, step_delay_y=2000
    //X.pos=15.0000000000, Y.pos=52.5000000000, Z.pos=0.0000000000
    //X.pos=3142.5000000000, Y.pos=2145.0000000000, Z.pos=0.0000000000
    //X.pos=6292.5000000000, Y.pos=4237.5000000000, Z.pos=0.0000000000
    //X.pos=9435.0000000000, Y.pos=6337.5000000000, Z.pos=0.0000000000
    //X.pos=12585.0000000000, Y.pos=8437.5000000000, Z.pos=0.0000000000
    //X.pos=15735.0000000000, Y.pos=10537.5000000000, Z.pos=0.0000000000
    //X.pos=18885.0000000000, Y.pos=12637.5000000000, Z.pos=0.0000000000
    //X.pos=22042.5000000000, Y.pos=14737.5000000000, Z.pos=0.0000000000
    //X.pos=25192.5000000000, Y.pos=16845.0000000000, Z.pos=0.0000000000
    //X.pos=28342.5000000000, Y.pos=18945.0000000000, Z.pos=0.0000000000
    //Finished motor=y: 10410
    //Finished motor=x: 10436
    prepare_line_2d(&sm_x, &sm_y, 30, 20, 8);
}

static void prepare_line2() {
    // 1142*18666+18666000=39982572
    // 14003*2666+2666000 =39997998
    // Finished motor=x: 50924
    // Finished motor=y: 50984
    prepare_line_2d(&sm_x, &sm_y, 140, 20, 40);
}

void setup() {
    #ifdef DEBUG_SERIAL
        Serial.begin(9600);
        Serial.println("Starting Rraptor...");
    #endif // DEBUG_SERIAL
    
    // Информация о текущем устройстве
    init_device_info(DEVICE_NAME, DEVICE_MODEL, DEVICE_SERIAL_NUMBER, 
        DEVICE_DESCRIPTION, DEVICE_VERSION,
        DEVICE_MANUFACTURER, DEVICE_URI);    

    // информация о подключенных моторах
    // init_stepper(stepper* smotor,  char* name, 
    //     int pin_step, int pin_dir, int pin_en,
    //     int dir_inv, int pulse_delay,
    //     float distance_per_step)
//    init_stepper_ends(stepper* smotor,
//        end_strategy min_end_strategy, end_strategy max_end_strategy,
//        double min_pos, double max_pos);
    // Драйвер 1 (X - синий драйвер)
    init_stepper(&sm_x, 'x', 11, 10, NO_PIN, false, 1000, 7.5); 
    init_stepper_ends(&sm_x, NO_PIN, NO_PIN, CONST, CONST, 0, 300000);
    // Драйвер 2 (Y - желтый драйвер)
    init_stepper(&sm_y, 'y', 6, 7, NO_PIN, false, 1000, 7.5);
    init_stepper_ends(&sm_y, NO_PIN, NO_PIN, CONST, CONST, 0, 216000);
    // Драйвер 3 (Z - черный драйвер)
    init_stepper(&sm_z, 'z', 4, 5, NO_PIN, false, 1000, 7.5);
    init_stepper_ends(&sm_z, NO_PIN, NO_PIN, CONST, CONST, 0, 100000);
    
    init_device_motors(&sm_x, &sm_y, &sm_z);
    
    
    // Модули связи
    #ifdef RR_SERIAL
        #ifdef DEBUG_SERIAL
            Serial.println("Enable Serial communication module");
        #endif // DEBUG_SERIAL
        
        rraptorSerialSetup();
    #endif // RR_SERIAL
    
    #ifdef RR_TCP
        #ifdef DEBUG_SERIAL
            Serial.println("Enable TCP communication module");
        #endif // DEBUG_SERIAL
        
        rraptorTcpSetup();
    #endif // RR_TCP
    
    #ifdef RR_ROBOT_SERVER
        #ifdef DEBUG_SERIAL
            Serial.println("Enable Robot Server communication module");
        #endif // DEBUG_SERIAL
        
        rraptorTcpSetup();
    #endif // RR_ROBOT_SERVER

    #ifdef RR_USB_ACCESSORY
        #ifdef DEBUG_SERIAL
            Serial.println("Enable Android USB Accessory communication module");
        #endif // DEBUG_SERIAL
        
        rraptorUSBAccessorySetup();
    #endif // RR_USB_ACCESSORY
        
//    prepare_line1();
//    start_stepper_cycle();
}

void loop1() {

    if(is_stepper_cycle_running()) {
        Serial.print("X.pos=");
        Serial.print(sm_x.current_pos, DEC);
        Serial.print(", Y.pos=");
        Serial.print(sm_y.current_pos, DEC);
        Serial.print(", Z.pos=");
        Serial.print(sm_z.current_pos, DEC);
        Serial.println();
        
        delay(1000);
    }
}

int prevTime = 0;

void loop() {
  
    #ifdef RR_SERIAL
        rraptorSerialTasks();
    #endif // RR_SERIAL
  
    #ifdef RR_TCP
        rraptorTcpTasks();
    #endif // RR_TCP
    
    #ifdef RR_ROBOT_SERVER
        rraptorRobotServerTasks();
    #endif // RR_ROBOT_SERVER

    #ifdef RR_USB_ACCESSORY
        rraptorUSBAccessoryTasks();
    #endif // RR_USB_ACCESSORY

    #ifdef DEBUG_SERIAL    
        // Отладочные сообщение - печатаем текущую позицию печатающего блока
        int currTime = millis();
        if(is_cycle_running() && (currTime - prevTime) >= 1000) {
            prevTime = currTime;
            Serial.print("X.pos=");
            Serial.print(sm_x.current_pos, DEC);
            Serial.print(", Y.pos=");
            Serial.print(sm_y.current_pos, DEC);
            Serial.print(", Z.pos=");
            Serial.print(sm_z.current_pos, DEC);
            Serial.println();
        }
    #endif // DEBUG_SERIAL
}


