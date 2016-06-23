// Управление устройством с Сервера Роботов

#include "WProgram.h"

#include <WiFiShieldOrPmodWiFi_G.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

#include "rraptor_config.h"
#include "network_util.h"
#include "rraptor_protocol.h"

// Пин статуса подключения к Wifi
#define WIFI_STATUS_PIN 13

// Значения для подключений

// Точка доступа ВайФай
//static const char* wifi_ssid = "lasto4ka";
//static const char* wifi_wpa2_passphrase = "robotguest";
static const char* wifi_ssid = "helen";
static const char* wifi_wpa2_passphrase = "13april1987";

// Сервер Роботов
static const char* robot_server_host = "robotc.lasto4ka.su";
//const char* robot_server_host = "192.168.1.3";
static const int robot_server_port = 1116;

// Подключение к WiFi
static int conectionId = DWIFIcK::INVALID_CONNECTION_ID;
// TCP-клиент - подключение к серверу
static TcpClient tcpClient;

// Размеры буферов для чтения команд и записи ответов 
#ifndef CMD_READ_BUFFER_SIZE
#define CMD_READ_BUFFER_SIZE 128
#endif

#ifndef CMD_WRITE_BUFFER_SIZE
#define CMD_WRITE_BUFFER_SIZE 512
#endif

// Буферы для обмена данными с сервером
static char read_buffer[CMD_READ_BUFFER_SIZE];
static char write_buffer[CMD_WRITE_BUFFER_SIZE];
static int write_size;

/**
 * Подключиться к открытой сети WiFi.
 */
static int connectWifiOpen(const char* ssid, DNETcK::STATUS *netStatus) {
    #ifdef DEBUG_SERIAL
        Serial.print("SSID: ");
        Serial.println(ssid);
    #endif // DEBUG_SERIAL
      
    return DWIFIcK::connect(ssid, netStatus);   
}

/**
 * Подключиться к сети WiFi, защищенной WPA2 с паролем.
 */
static int connectWifiWPA2Passphrase(const char* ssid, const char* passphrase, DNETcK::STATUS *netStatus) {
    #ifdef DEBUG_SERIAL
        Serial.print("SSID: ");
        Serial.print(ssid);
        Serial.print(", WPA2 passphrase: ");
        Serial.println(passphrase);
    #endif // DEBUG_SERIAL
        
    return DWIFIcK::connect(ssid, passphrase, netStatus);
}

/**
 * Подлключиться к сети Wifi.
 */
static int connectWifi(DNETcK::STATUS *netStatus) {
    int conId = DWIFIcK::INVALID_CONNECTION_ID;
    // Выбрать способ подключения - раскомментировать нужную строку.    
//    conId = connectWifiOpen(wifi_ssid, netStatus);
    conId = connectWifiWPA2Passphrase(wifi_ssid, wifi_wpa2_passphrase, netStatus);
    return conId;
}

/**
 * Инициализировать канал связи с Сервером Роботов.
 */
void rraptorRobotServerSetup() {
    pinMode(WIFI_STATUS_PIN, OUTPUT);
}

/**
 * Работа канала связи с Сервером Роботов, запускать в loop. 
 * При получении команды, вызывает handleInput.
 */
void rraptorRobotServerTasks() {
    DNETcK::STATUS networkStatus;
    int readSize;
    int writeSize;
    
    // Держим Tcp-стек в живом состоянии
    DNETcK::periodicTasks();
        
    if(!DWIFIcK::isConnected(conectionId)) {
        // Не подключены к WiFi - выключим лампочку
        digitalWrite(WIFI_STATUS_PIN, LOW);
        
        // Подключимся к сети WiFi
        bool connectedToWifi = false;
        
        #ifdef DEBUG_SERIAL
            Serial.println("Connecting wifi...");
        #endif // DEBUG_SERIAL
        conectionId = connectWifi(&networkStatus);
  
        if(conectionId != DWIFIcK::INVALID_CONNECTION_ID) {
            // На этом этапе подключение будет создано, даже если указанная 
            // сеть Wifi недоступна или для нее задан неправильный пароль
            #ifdef DEBUG_SERIAL
                Serial.print("Connection created, connection id=");
                Serial.println(conectionId, DEC);

                Serial.print("Initializing IP stack...");
            #endif // DEBUG_SERIAL
            
            // Получим IP и сетевые адреса по DHCP
            DNETcK::begin();
            
            // К открытой сети может подключиться с первой попытки, к сети с паролем
            // может потребоваться несколько циклов (если пароль для сети неправильный,
            // то ошибка вылезет тоже на этом этапе).
            bool initializing = true;
            while(initializing) {
                #ifdef DEBUG_SERIAL 
                    Serial.print(".");
                #endif // DEBUG_SERIAL
                
                // Вызов isInitialized заблокируется до тех пор, пока стек не будет 
                // инициализирован или не истечет время ожидания (по умолчанию 15 секунд). 
                // Если сеть не подключится до истечения таймаута и при этом не произойдет
                // ошибка, isInitialized просто вернет FALSE без кода ошибки, при необходимости
                // можно вызвать его повторно до успеха или ошибки.
                if(DNETcK::isInitialized(&networkStatus)) {
                    // Стек IP инициализирован
                    connectedToWifi = true;
                    
                    initializing = false;
                } else if(DNETcK::isStatusAnError(networkStatus)) {
                    // Стек IP не инициализирован из-за ошибки,
                    // в этом месте больше не пробуем
                    initializing = false;
                }
            }
            #ifdef DEBUG_SERIAL 
                Serial.println();
            #endif // DEBUG_SERIAL
        }
        
        if(connectedToWifi) {
            // Подключились к Wifi
            #ifdef DEBUG_SERIAL 
                Serial.println("Connected to wifi");
                printNetworkStatus();
            #endif // DEBUG_SERIAL
                        
            // включим лампочку
            digitalWrite(WIFI_STATUS_PIN, HIGH);
        } else {
            // Так и не получилось подключиться
            #ifdef DEBUG_SERIAL 
                Serial.println("Retry after 4 seconds...");
                Serial.print("Failed to connect wifi, status: ");
                //Serial.print(networkStatus, DEC);
                printDNETcKStatus(networkStatus);
                Serial.println();
            #endif // DEBUG_SERIAL
            
            // Нужно корректно завершить весь стек IP и Wifi, чтобы
            // иметь возможность переподключиться на следующей итерации
            DNETcK::end();
            DWIFIcK::disconnect(conectionId);
            conectionId = DWIFIcK::INVALID_CONNECTION_ID;
            
            // Немного подождем и попробуем переподключиться на следующей итерации
            #ifdef DEBUG_SERIAL 
                Serial.println("Retry after 4 seconds...");
            #endif // DEBUG_SERIAL
            
            delay(4000);
        }
    } else if(!tcpClient.isConnected()) {
        // Подключимся к Серверу Роботов
        
        bool connectedToServer = false;
        
        #ifdef DEBUG_SERIAL 
                Serial.print("Connecting to Robot Server...");
        #endif // DEBUG_SERIAL
        
        tcpClient.connect(robot_server_host, robot_server_port);
        // Сокет для подключения назначен, подождем, чем завершится процесс подключения
        bool connecting = true;
        while(connecting) {
            #ifdef DEBUG_SERIAL 
                Serial.print(".");
            #endif // DEBUG_SERIAL
            
            if(tcpClient.isConnected(&networkStatus)) {
                // Подключились к сереверу
                connectedToServer = true;
                                    
                connecting = false;
            } else if(DNETcK::isStatusAnError(networkStatus)) {
                // Не смогли подключиться к серверу из-за ошибки,
                // в этом месте больше не пробуем
                connecting = false;                    
            }
        }
        Serial.println();
        
        if(connectedToServer) {
            // Подключились к Серверу Роботов
            #ifdef DEBUG_SERIAL 
                Serial.println("Connected to Robot Server");
            
                printTcpClientStatus(&tcpClient);
            #endif // DEBUG_SERIAL
        } else {
            // Так и не получилось подключиться
            #ifdef DEBUG_SERIAL
                Serial.print("Failed to connect Robot Server, status: ");
                //Serial.print(networkStatus, DEC);
                printDNETcKStatus(networkStatus);
                Serial.println();
            #endif // DEBUG_SERIAL
            
            // Вернем TCP-клиента в исходное состояние
            tcpClient.close();
            
            // Немного подождем и попробуем переподключиться на следующей итерации
            #ifdef DEBUG_SERIAL
                Serial.println("Retry after 4 seconds...");
            #endif // DEBUG_SERIAL
            delay(4000);
        }
    } else {
        // Подключены к серверу - читаем команды, отправляем ответы
        
        // есть чо почитать?
        if((readSize = tcpClient.available()) > 0) {
            readSize = readSize < sizeof(read_buffer) ? readSize : sizeof(read_buffer);
            readSize = tcpClient.readStream((byte*)read_buffer, readSize);
            
            #ifdef DEBUG_SERIAL
                // Считали порцию данных
                read_buffer[readSize] = 0; // строка должна оканчиваться нулем
                Serial.print("Read: ");
                Serial.println(read_buffer);
            #endif // DEBUG_SERIAL
 
            // и можно выполнить команду, ответ попадет в write_buffer
            writeSize = handleInput(read_buffer, readSize, write_buffer);
            write_size = writeSize;
        }
            
        if(write_size > 0) {
            #ifdef DEBUG_SERIAL
                Serial.print("Write: ");
                Serial.print(write_buffer);
                Serial.print(" (size=");
                Serial.print(write_size);
                Serial.println(")");
            #endif // DEBUG_SERIAL
                        
            tcpClient.writeStream((const byte*)write_buffer, write_size);
            write_size = 0;
        }
    }
}


