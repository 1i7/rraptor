#include <WiFiShieldOrPmodWiFi_G.h>

#include <DNETcK.h>
#include <DWIFIcK.h>

#include "tcp_util.h"
#include "rraptor_protocol.h"

// Пины статуса подключений
#define STATUS_WIFI_PIN 6

// Значения для подключений

// Точка доступа ВайФай
const char* wifi_ssid = "lasto4ka";
const char* wifi_wpa2_passphrase = "robotguest";

// статический IP-адрес для текущего хоста - попросим у 
// точки Wifi (иначе Пульт не узнает, куда подключаться)
IPv4 host_ip = {192,168,117,117};

// Порт для tcp-сервера
const int tcp_server_port = DNETcK::iPersonalPorts44 + 116;

int conectionId = DWIFIcK::INVALID_CONNECTION_ID;

TcpServer tcpServer;
TcpClient tcpClient;

// Таймаут неактивности подключенного клиента, миллисекунды
int CLIENT_IDLE_TIMEOUT = 10000;
int clientIdleStart = 0;

static char read_buffer[128];
static char write_buffer[128];
int read_size;
int write_size;

void printTcpServerStatus() {
    printIPAddress(&host_ip);
    Serial.print(":");
    Serial.println(tcp_server_port);
}

/**
 * Подключиться к открытой сети WiFi.
 */
int connectWifiOpen(const char* ssid, DNETcK::STATUS *netStatus) {
    Serial.print("SSID: ");
    Serial.println(ssid);
  
    return DWIFIcK::connect(wifi_ssid, netStatus);   
}

/**
 * Подключиться к сети WiFi, защищенной WPA2 с паролем.
 */
int connectWifiWPA2Passphrase(const char* ssid, const char* passphrase, DNETcK::STATUS *netStatus) {
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.print(", WPA2 passphrase: ");
    Serial.println(passphrase);
    
    return DWIFIcK::connect(ssid, passphrase, netStatus);
}

/**
 * Подлключиться к сети WiFi.
 */
int connectWifi(DNETcK::STATUS *netStatus) {
    int conId = DWIFIcK::INVALID_CONNECTION_ID;
    // Выбрать способ подключения - раскомментировать нужную строку.    
//    conId = connectWifiOpen(wifi_ssid, netStatus);
    conId = connectWifiWPA2Passphrase(wifi_ssid, wifi_wpa2_passphrase, netStatus);
    return conId;
}

void setup() {
    Serial.begin(9600);
    Serial.println("Start wifi network server demo");
}
    
void loop() {
    DNETcK::STATUS networkStatus;
    int readSize;
    int writeSize;
        
    if(!DWIFIcK::isConnected(conectionId)) {
        // Подключимся к сети Wifi
        
        bool connectedToWifi = false;
        
        Serial.println("Connecting wifi...");
        conectionId = connectWifi(&networkStatus);
  
        if(conectionId != DWIFIcK::INVALID_CONNECTION_ID) {
            // На этом этапе подключение будет создано, даже если указанная 
            // сеть Wifi недоступна или для нее задан неправильный пароль
            Serial.print("Connection created, connection id=");
            Serial.println(conectionId, DEC);

            Serial.print("Initializing IP stack...");
            
            // Подключимся со статическим ip-адресом
            DNETcK::begin(host_ip);
            
            // К открытой сети может подключиться с первой попытки, к сети с паролем
            // может потребоваться несколько циклов (если пароль для сети неправильный,
            // то ошибка вылезет тоже на этом этапе).
            bool initializing = true;
            while(initializing) {
                Serial.print(".");
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
            Serial.println();
        }
        
        if(connectedToWifi) {
            // Подключились к Wifi
            Serial.println("Connected to wifi");
            printNetworkStatus();
        } else {
            // Так и не получилось подключиться
            Serial.print("Failed to connect wifi, status: ");
            printStatus(networkStatus);
            Serial.println();
            
            // Нужно корректно завершить весь стек IP и Wifi, чтобы
            // иметь возможность переподключиться на следующей итерации
            DNETcK::end();
            DWIFIcK::disconnect(conectionId);
            conectionId = DWIFIcK::INVALID_CONNECTION_ID;
            
            // Немного подождем и попробуем переподключиться на следующей итерации
            Serial.println("Retry after 4 seconds...");
            delay(4000);
        }
    } else if(!tcpServer.isListening()) {
        // Подключимся к сети Wifi
        
        bool startedListening = false;
        
        Serial.print("Start listening connection from Pult...");
        tcpServer.startListening(tcp_server_port);
        // Подождем, пока сокет начнет слушать подключения
        bool starting = true;
        while(starting) {
            Serial.print(".");
            if(tcpServer.isListening(&networkStatus)) {
                // Начали слушать
                startedListening = true;
                                    
                starting = false;
            } else if(DNETcK::isStatusAnError(networkStatus)) {
                // Не смогли начать слушать из-за ошибки,
                // в этом месте больше не пробуем
                starting = false;                    
            }
        }
        Serial.println();
        
        if(startedListening) {
            // Начали слушать подключения от пульта
            Serial.print("Listen connection from Pult on: ");
            printTcpServerStatus();
        } else {
            // Так и не получилось начать слушать подключения
            Serial.print("Failed to start listening, status: ");
            printStatus(networkStatus);
            Serial.println();
            
            // Вернем TCP-сервер в исходное состояние
            tcpServer.close();
            
            // Немного подождем и попробуем переподключиться на следующей итерации
            Serial.println("Retry after 4 seconds...");
            delay(4000);
        }
    } else if(!tcpClient.isConnected()) {
        // Подождем подключения клиента
        
        if(tcpServer.availableClients() > 0) {
            // закроем старого клиента, если он использовался ранее
            tcpClient.close(); 

            if(tcpServer.acceptClient(&tcpClient)) {
                Serial.println("Got a Connection: ");
                printTcpClientStatus(&tcpClient);
                
                // начнем счетчик неактивности
                clientIdleStart = millis();
            }
        }
    } else {
        // Пульт подключен - читаем команды, отправляем ответы
        
        // есть что почитать?
        if((readSize = tcpClient.available()) > 0) {
            readSize = readSize < sizeof(read_buffer) ? readSize : sizeof(read_buffer);
            readSize = tcpClient.readStream((byte*)read_buffer, readSize);
            
            // Считали порцию данных - добавим завершающий ноль
            read_buffer[readSize] = 0;
            
            Serial.print("Read: ");
            Serial.println(read_buffer);
 
            // и можно выполнить команду, ответ попадет в write_buffer
            writeSize = handleInput(read_buffer, read_size, write_buffer);
            write_size = writeSize;
            
            // сбросим счетчик неактивности
            clientIdleStart = millis();
        }
            
        if(write_size > 0) {
            Serial.print("Write: ");
            Serial.print(write_buffer);
            Serial.println();
            
            tcpClient.writeStream((const byte*)write_buffer, write_size);
            write_size = 0;
            
            // сбросим счетчик неактивности
            clientIdleStart = millis();
        }
        
        if( (millis() - clientIdleStart) > CLIENT_IDLE_TIMEOUT ) {
            Serial.print("Close connection on timeout");
            tcpClient.close();
        }
    }
}

