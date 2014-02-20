#include <WiFiShieldOrPmodWiFi_G.h>

#include <DNETcK.h>
#include <DWIFIcK.h>

namespace rraptor {
  int GLOBAL_DISABLE_MOTORS = 0;
  
  /* Остановить все моторы */
  const char* CMD_STOP = "stop";
  /* Повернуть заданный мотор на заданное количество шагов */
  const char* CMD_STEP = "step";
  /* Переместить заданную координату на заданное расстояние */
  const char* CMD_MOVE = "move";
  /* Запустить мотор с заданной скоростью на непрерывное вращение */
  const char* CMD_GO = "go";
  
/**
 * Структура - шаговый двигатель.
 */
typedef struct {
    /* Motor name */
    char* name;
    /* Movement distance per movement cycle, micrometre */
    int distance_per_cycle;
    /* Time spent for movement cycle, microseconds */
    int time_per_cycle;
  
    int MOTOR_DIR_PIN;
    int MOTOR_PULSE_PIN;
    int MOTOR_EN_PIN;    

    /* Задержка между 2 шагами, микросекунды */
    int step_delay;
} smotor;

smotor sm_x, sm_y, sm_z;
/* Текущая скорость моторо, запущенных командой go:
 * 1 - вперед
 * 0 - стоит на месте
 * -1 - назад
 */
int speed_x = 0;
int speed_y = 0;
int speed_z = 0;

void init_smotor(smotor* sm, char* name, int distance_per_cycle, int time_per_cycle,
    int step_delay, int dir_pin, int pulse_pin, int en_pin) {
  
  sm->name = name;
  sm->distance_per_cycle = distance_per_cycle;
  sm->time_per_cycle = time_per_cycle;
  
  sm->step_delay = step_delay;  
  sm->MOTOR_DIR_PIN = dir_pin;
  sm->MOTOR_PULSE_PIN = pulse_pin;
  sm->MOTOR_EN_PIN = en_pin;
}

/**
 * Получить шаговый двигатель по уникальному имени.
 */
smotor* smotor_by_id(char* id) {
  if(strcmp("x", id) == 0) {
    return &sm_x;
  } else if(strcmp("y", id) == 0) {
    return &sm_y;
  } else if(strcmp("z", id) == 0) {
    return &sm_z;
  } else {
    return 0;
  }
}

void enable_motors() {
  GLOBAL_DISABLE_MOTORS = 0;
}

void disable_motors() {
  GLOBAL_DISABLE_MOTORS = 1;
}

/**
 * Повернуть двигатель на нужное количество циклов с нужной скоростью.
 * cnum - количество циклов - знаковое целое: если > 0, шагаем вперед, если < 0 - назад.
 * cdelay - время выполнения цикла, микросекунды.
 */
void step_motor(smotor* sm, int cnum, int cdelay) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", cycle count=" + cnum + ", cycle delay=" + cdelay + "us" + "]:   ");
    // задать направление в зависиомости от знака
    if(cnum > 0) {
        digitalWrite(sm->MOTOR_DIR_PIN, HIGH);
    } else {
        digitalWrite(sm->MOTOR_DIR_PIN, LOW);
    }
    
    cnum = abs(cnum);
    
    int tdelay = (cdelay - sm->time_per_cycle) / 1000;
    if(tdelay < 0) {
        if(cdelay != 0) {
          Serial.println(String("") + "Waring: timer delay < 0 [motor=" + sm->name + "], reset to 0");
        }
        tdelay=0;
    }
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + "estimated time=" + est_time + "us");

    int i=0;
    while (i < cnum && !GLOBAL_DISABLE_MOTORS) {
      // пусть будет 2 шага в одном цикле
        digitalWrite(sm->MOTOR_PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        delay(tdelay);
        i++;
    }
}

/**
 * Сдвинуть текущее положение координаты на указанное расстояние с нужной скоростью.
 * dl - сдвиг относительно текущего положения, микрометры - знаковое целое: если > 0, сдвиг вперед, если < 0 - назад.
 * dt - время сдвига, микросекунды.
 */
void step_motor_um(smotor* sm, int dl, int dt) {
    Serial.print(String("") + "Info: Stepping motor [name=" + sm->name + ", dl=" + dl + "um, dt=" + dt + "us" + "]: ");
    if(dl == 0) {
         Serial.println("skip");
        return;
    }
    
    // calculate number of steps
    int cnum = abs(dl) / sm->distance_per_cycle;
    Serial.print(String("") + "cycle count=" + cnum);
    // calculate cycle delay
    int cdelay = dt / cnum;

    //debug_printf("cycle delay=%dus, ", cdelay);
    // calculate timer delay - in millis
    // time_per_cycle = step_delay * 4
    int tdelay = (cdelay - sm->time_per_cycle) / 1000;
    Serial.println(String("") + ", timer delay=" + tdelay);
    if(tdelay < 0) {
        if(cdelay != 0) {
            Serial.println(String("") + "Waring: timer delay < 0 [motor=" + sm->name + "], reset to 0");
        }
        tdelay=0;
    }
    
    // Ожидаемое время (для отладки):
    int est_time = (sm->step_delay * 4 + tdelay) * cnum;
    Serial.println(String("") + ", estimated time=" + est_time + "us");
  
    // задать направление в зависиомости от знака
    if(dl > 0) {
        digitalWrite(sm->MOTOR_DIR_PIN, HIGH);
    } else {
        digitalWrite(sm->MOTOR_DIR_PIN, LOW);
    }

    int i=0;
    while (i < cnum && !GLOBAL_DISABLE_MOTORS) {
      // пусть будет 2 шага в одном цикле
        digitalWrite(sm->MOTOR_PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, HIGH);
        delayMicroseconds(sm->step_delay);
        digitalWrite(sm->MOTOR_PULSE_PIN, LOW);
        delayMicroseconds(sm->step_delay);
        
        delay(tdelay);
        i++;
    }
}

void go_cycle_motor(smotor* sm, int spd) {
  if(spd != 0) {
    int cnum = 0;
    int cdelay = 0;
    if(spd > 0) {
      cnum = 100;
    } else if (spd < 0) {
      cnum = -100;
    }
    step_motor(sm, cnum, cdelay);
  }
}

/**
 * Обработать поведение режима постоянной работы моторов,
 * устанавлемоего командой go. Для моторов с ненулевой скоростью,
 * повернуть на один цикл в заданном направлении.
 */
void handle_go() {
  go_cycle_motor(&sm_x, speed_x);
  go_cycle_motor(&sm_y, speed_y);
  go_cycle_motor(&sm_z, speed_z);
}

/**
 * Остановить все моторы, запущенные командой go.
 */
void stop_motors() {
  speed_x = 0;
  speed_y = 0;
  speed_z = 0;
}


void handle_command(char* cmd_line) {
  // новая команда остановит моторы, находящиеся в режиме 
  // постоянной работы
  stop_motors();
  
  String cmd_line_str = String(cmd_line).trim();
  
  int success = 0;
  if (cmd_line_str.startsWith(CMD_STOP)) {
    Serial.println("Handle command: stop");
    success = 1;
    
    stop_motors();
  } else if(cmd_line_str.startsWith(CMD_STEP)) {
    // command 'step' syntax:
    //     step motor_name cnum [cdelay]
    String motor_name;
    String cnum_str;
    String cdelay_str;
    int cnum;
    int cdelay = 0;
    
    int space1 = -1;
    int space2 = -1;
    
    space1 = cmd_line_str.indexOf(' ');
    if(space1 != -1) {
      space2 = cmd_line_str.indexOf(' ', space1 + 1);
      if(space2 != -1) {
        motor_name = cmd_line_str.substring(space1 + 1, space2);
        
        space1 = space2;
        space2 = cmd_line_str.indexOf(' ', space1 + 1);
        if(space2 != -1) {
          cnum_str = cmd_line_str.substring(space1 + 1, space2);
        } else {
          cnum_str = cmd_line_str.substring(space1 + 1);
        }
        
        cnum = cnum_str.toInt();//atoi(cnum_str.toCharArray());
          
        // Необязательный параметр - cdelay
        if(space2 != -1) {
          cdelay_str = cmd_line_str.substring(space2 + 1);
          cdelay = cdelay_str.toInt();//atoi(cnum_str.toCharArray());
        }
          
        // Команда корректна
        success = 1;
        Serial.println("Handle command: [cmd=step, motor=" + motor_name + ", cycle num=" + cnum + ", cycle delay=" + cdelay + "]");
          
        // Выполнить команду
        char motor_name_chars[motor_name.length() + 1];
        motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);

        smotor* sm = smotor_by_id(motor_name_chars);
        if(sm != 0) {
          step_motor(sm, cnum, cdelay);
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  } else if(cmd_line_str.startsWith(CMD_MOVE)) {
    // command 'move' syntax:
    //     move motor_name dl [dt]
    String motor_name;
    String dl_str;
    String dt_str;
    int dl;
    int dt = 0;
    
    int space1 = -1;
    int space2 = -1;
    
    space1 = cmd_line_str.indexOf(' ');
    if(space1 != -1) {
      space2 = cmd_line_str.indexOf(' ', space1 + 1);
      if(space2 != -1) {
        motor_name = cmd_line_str.substring(space1 + 1, space2);
        
        space1 = space2;
        space2 = cmd_line_str.indexOf(' ', space1 + 1);
        if(space2 != -1) {
          dl_str = cmd_line_str.substring(space1 + 1, space2);
        } else {
          dl_str = cmd_line_str.substring(space1 + 1);
        }
        
        dl = dl_str.toInt();//atoi(cnum_str.toCharArray());
          
        // Необязательный параметр - dt
        if(space2 != -1) {
          dt_str = cmd_line_str.substring(space2 + 1);
          dt = dt_str.toInt();//atoi(cnum_str.toCharArray());
        }
          
        // Команда корректна
        success = 1;
        Serial.println("Handle command: [cmd=move, motor=" + motor_name + ", dl=" + dl + ", dt=" + dt + "]");
          
        // Выполнить команду
        char motor_name_chars[motor_name.length() + 1];
        motor_name.toCharArray(motor_name_chars, motor_name.length() + 1);

        smotor* sm = smotor_by_id(motor_name_chars);
        if(sm != 0) {
          step_motor_um(sm, dl, dt);
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  }  else if(cmd_line_str.startsWith(CMD_GO)) {
    // command 'go' syntax:
    //     go motor_name speed
    String motor_name;
    String spd_str;
    int spd;
    
    int space1 = -1;
    int space2 = -1;
    
    space1 = cmd_line_str.indexOf(' ');
    if(space1 != -1) {
      space2 = cmd_line_str.indexOf(' ', space1 + 1);
      if(space2 != -1) {
        motor_name = cmd_line_str.substring(space1 + 1, space2);
        
        spd_str = cmd_line_str.substring(space2 + 1);
        spd = spd_str.toInt();//atoi(cnum_str.toCharArray());
          
        // Команда корректна
        success = 1;
        Serial.println("Handle command: [cmd=go, motor=" + motor_name + ", speed=" + spd + "]");
          
        // Выполнить команду
        if(motor_name.equalsIgnoreCase("x")) {
          speed_x = spd;
        } else if(motor_name.equalsIgnoreCase("y")) {
          speed_y = spd;
        } else if(motor_name.equalsIgnoreCase("z")) {
          speed_z = spd;
        } else {
          Serial.println("Error: Unknown motor: " + motor_name);
        }
      }
    }
  }
  
  if(!success) {
    Serial.println("Can't handle command: " + cmd_line_str);
  }
}

} // namespace rraptor

// WiFi
IPv4 ipServer = {192,168,43,191};
unsigned short portServer = DNETcK::iPersonalPorts44 + 300;

const char * szSsid = "lasto4ka";

#define WiFiConnectMacro() DWIFIcK::connect(szSsid, &status)

typedef enum {
    NONE = 0,
    INITIALIZE,
    LISTEN,
    ISLISTENING,
    AVAILABLECLIENT,
    ACCEPTCLIENT,
    READ,
    WRITE,
    CLOSE,
    EXIT,
    DONE
} STATE;

STATE state = INITIALIZE;

unsigned tStart = 0;
// connection reset timeout
unsigned tWait = 10000;

TcpServer tcpServer;
TcpClient tcpClient;

byte rgbRead[1024];
char tcpReadLine[1024];
int cbRead = 0;
int count = 0;

DNETcK::STATUS status;
/* ***************************************************** */
// Runtime

void printIP(void)
{
  Serial.print("IP Address assigned: ");
  Serial.print((int)ipServer.rgbIP[0]);
  Serial.print(".");
  Serial.print((int)ipServer.rgbIP[1]);
  Serial.print(".");
  Serial.print((int)ipServer.rgbIP[2]);
  Serial.print(".");
  Serial.println((int)ipServer.rgbIP[3]);
}
void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  
  // init stepper motors
  int step_delay = 250;
  // with step_delay=250 1 cycle=1 mls
  rraptor::init_smotor(&rraptor::sm_x, "X", 15, step_delay * 4, step_delay, 0, 1, 2);
  rraptor::init_smotor(&rraptor::sm_y, "Y", 15, step_delay * 4, step_delay, 8, 9, 10);
  rraptor::init_smotor(&rraptor::sm_z, "Z", 15, step_delay * 4, step_delay, 16, 17, 18);
  
  // Wifi
    int conID = DWIFIcK::INVALID_CONNECTION_ID;

    Serial.println("WiFiTCPEchoServer 1.0");
    Serial.println("Digilent, Copyright 2012");
    Serial.println("");

    if((conID = WiFiConnectMacro()) != DWIFIcK::INVALID_CONNECTION_ID) {
        Serial.print("Connection Created, ConID = ");
        Serial.println(conID, DEC);
        printIP();
        state = INITIALIZE;
    } else {
        Serial.print("Unable to connection, status: ");
        Serial.println(status, DEC);
        state = EXIT;
    }

    DNETcK::begin(ipServer);
}

void loop()
{
  //rraptor::handle_command("move x -10000");
  //rraptor::handle_command("step x 1000");
  
  // Провернуть моторы, находящием в режиме постоянной работы.
  rraptor::handle_go();
  
    switch(state) {
    case INITIALIZE:
        if(DNETcK::isInitialized(&status)) {
            Serial.println("IP Stack Initialized");
            state = LISTEN;
        } else if(DNETcK::isStatusAnError(status)) {
            Serial.print("Error in initializing, status: ");
            Serial.println(status, DEC);
            state = EXIT;
        }
        break;

    case LISTEN:
        if(tcpServer.startListening(portServer)) {
            Serial.println("Started Listening");
            state = ISLISTENING;
        } else {
            state = EXIT;
        }
        break;

    case ISLISTENING:
        if(tcpServer.isListening(&status)) {  
            Serial.print("Listening on port: ");
            Serial.print(portServer, DEC);
            Serial.println("");
            
            state = AVAILABLECLIENT;
        } else if(DNETcK::isStatusAnError(status)) {
            state = EXIT;
        }
        break;

    case AVAILABLECLIENT:
        if((count = tcpServer.availableClients()) > 0) {
            Serial.print("Got ");
            Serial.print(count, DEC);
            Serial.println(" clients pending");
            state = ACCEPTCLIENT;
        }
        break;

    case ACCEPTCLIENT:
        
        tcpClient.close(); 

        if(tcpServer.acceptClient(&tcpClient)) {
            Serial.println("Got a Connection");
            state = READ;
            tStart = (unsigned) millis();
        } else {
            state = CLOSE;
        }
        break;

    case READ:
        if((cbRead = tcpClient.available()) > 0) {
            cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
            cbRead = tcpClient.readStream(rgbRead, cbRead);

            Serial.print("Got ");
            Serial.print(cbRead, DEC);
            Serial.println(" bytes");
            
            strncpy(tcpReadLine, (char*)rgbRead, cbRead);
            tcpReadLine[cbRead] = 0; // ensure 0-terminated         
            rraptor::handle_command(tcpReadLine);
            
            state = WRITE;
        } else if( (((unsigned) millis()) - tStart) > tWait ) {
            state = CLOSE;
        }
        break;

    case WRITE:
        if(tcpClient.isConnected()) {               
            Serial.println("Writing: ");  
            for(int i=0; i < cbRead; i++) {
                Serial.print(rgbRead[i], BYTE);
            }
            Serial.println("");  

            tcpClient.writeStream(rgbRead, cbRead);
            state = READ;
            tStart = (unsigned) millis();
        } else {
            Serial.println("Unable to write back.");  
            state = CLOSE;
        }
        break;
        
    case CLOSE:
        tcpClient.close();
        Serial.println("Closing TcpClient");
        Serial.println("");
        rraptor::stop_motors();
        state = ISLISTENING;
        break;

    case EXIT:
        tcpClient.close();
        tcpServer.close();
        Serial.println("Something went wrong, sketch is done.");  
        rraptor::stop_motors();
        state = DONE;
        break;

    case DONE:
    default:
        break;
    }
    
    DNETcK::periodicTasks(); 
}


