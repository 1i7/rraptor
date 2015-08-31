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
static const char* wifi_ssid = "lasto4ka";
static const char* wifi_wpa2_passphrase = "robotguest";
//static const char* wifi_ssid = "helen";
//static const char* wifi_wpa2_passphrase = "13april1987";

// статический IP-адрес для текущего хоста - попросим у 
// точки Wifi (иначе Пульт не узнает, куда подключаться)
//IPv4 host_ip = {192,168,115,115};
static IPv4 host_ip = {192,168,43,115};
//static IPv4 host_ip = {192,168,1,115};

// Порт для tcp-сервера
static const int tcp_server_port = DNETcK::iPersonalPorts44 + 114;

static int conectionId = DWIFIcK::INVALID_CONNECTION_ID;

static TcpServer tcpServer;
static TcpClient tcpClient;

// Таймаут неактивности подключенного клиента, миллисекунды
static int CLIENT_IDLE_TIMEOUT = 10000;
static int clientIdleStart = 0;

static char read_buffer[128];
static char write_buffer[128];
static int write_size;

static void printTcpServerStatus() {
    printIPAddress(&host_ip);
    Serial.print(":");
    Serial.println(tcp_server_port);
}


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
 * Инициализировать канал связи Tcp.
 */
void rraptorTcpSetup() {
    pinMode(WIFI_STATUS_PIN, OUTPUT);
    
    // не блокировать вызовы DNETcK::isInitialized, чтобы другие 
    // модули нормально работали во время подключения к вайфаю
    DNETcK::setDefaultBlockTime(DNETcK::msImmediate);
}

bool postConnectDone = false;

/**
 * Работа канала связи Tcp, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorTcpTasks() {
    DNETcK::STATUS networkStatus;
    int readSize;
    int writeSize;
    
    // Держим Tcp-стек в живом состоянии
    DNETcK::periodicTasks();
        
    if(!DWIFIcK::isConnected(conectionId, &networkStatus)) {
        // Не подключены к WiFi - выключим лампочку
        digitalWrite(WIFI_STATUS_PIN, LOW);
        
        if(conectionId == DWIFIcK::INVALID_CONNECTION_ID) {
            // connectionId некорректный - значит ранее не подключались к сети или 
            // предыдущая попытка завершилась неудачей 
          
            // Подключимся к сети Wifi
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
                #endif // DEBUG_SERIAL
                
                
                // Корректный conectionId еще не говорит о том, что мы подключились, но
                // пока подключаемся, заодно запустим процесс инициализации TCP-стека
                #ifdef DEBUG_SERIAL
                    Serial.println("Initializing IP stack...");
                #endif // DEBUG_SERIAL
            
                // Подключимся со статическим ip-адресом
                DNETcK::begin(host_ip);
            }
        } else if(DNETcK::isStatusAnError(networkStatus)) {
            // Не подключились к сети из-за ошибки
            #ifdef DEBUG_SERIAL
                Serial.print("Failed to connect wifi, status: ");
                printDNETcKStatus(networkStatus);
                Serial.println();
            #endif // DEBUG_SERIAL
            
            // Нужно корректно завершить весь стек IP и Wifi, чтобы
            // иметь возможность переподключиться на следующей итерации
            DNETcK::end();
            DWIFIcK::disconnect(conectionId);
            conectionId = DWIFIcK::INVALID_CONNECTION_ID;
            postConnectDone = false;
        } // else {
            // иначе просто ничего не далаем до тех пор, пока DWIFIcK::isConnected
            // не станет TRUE или не вернет код ошибки
        // }
    } else if(!DNETcK::isInitialized(&networkStatus)) {
        // подключение есть, но стек IP еще не инициализирован
        
        // 1) При стандартных настройках ожидания вызов isInitialized заблокируется до тех пор, пока стек не будет 
        // инициализирован или не истечет время ожидания (по умолчанию 15 секунд). 
        // Если сеть не подключится до истечения таймаута и при этом не произойдет
        // ошибка, isInitialized просто вернет FALSE без кода ошибки, при необходимости
        // можно вызвать его повторно до успеха или ошибки.
        // 2) Но так так мы используем параметр DNETcK::msImmediate, блокировки на этом вызове не будет,
        // просто будем много раз проскакивать это место до тех пор, пока DNETcK::isInitialized не станет TRUE
        // или не вернет код ошибки
      
        if(DNETcK::isStatusAnError(networkStatus)) {
            // Стек IP не инициализирован из-за ошибки,
            // в этот раз не получилось подключиться
            #ifdef DEBUG_SERIAL
                Serial.print("Failed to init wifi network stack, status: ");
                printDNETcKStatus(networkStatus);
                Serial.println();
            #endif // DEBUG_SERIAL
                        
            // Нужно корректно завершить весь стек IP и Wifi, чтобы
            // иметь возможность переподключиться на следующей итерации
            DNETcK::end();
            DWIFIcK::disconnect(conectionId);
            conectionId = DWIFIcK::INVALID_CONNECTION_ID;
            postConnectDone = false;
        } // else {
            // иначе просто ничего не далаем до тех пор, пока DNETcK::isInitialized 
            // не станет TRUE или не вернет код ошибки
        // }
    } else if(!postConnectDone) {
        // Только что подключились к Wifi
        #ifdef DEBUG_SERIAL
            Serial.println("Connected to wifi");
            printNetworkStatus();
        #endif // DEBUG_SERIAL
                        
        // включим лампочку
        digitalWrite(WIFI_STATUS_PIN, HIGH);
            
        // Вернем TCP-сервер в исходное состояние, если он уже запускался 
        // ранее за эту сессию
        tcpServer.close();
        
        // выполнять этот блок только один раз сразу после подключения    
        postConnectDone = true;
    } else if(!tcpServer.isListening()) {
        // Запустим TCP-сервер слушать подключения
        
        bool startedListening = false;
        
        #ifdef DEBUG_SERIAL
            Serial.print("Start listening connection from Pult...");
        #endif // DEBUG_SERIAL
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
            #ifdef DEBUG_SERIAL
                Serial.print("Listen connection from Pult on: ");
                printTcpServerStatus();
            #endif // DEBUG_SERIAL
        } else {
            // Так и не получилось начать слушать подключения
            #ifdef DEBUG_SERIAL
                Serial.print("Failed to start listening, status: ");
                printDNETcKStatus(networkStatus);
                Serial.println();
            #endif // DEBUG_SERIAL
            
            // Вернем TCP-сервер в исходное состояние
            tcpServer.close();
            
            // Немного подождем и попробуем переподключиться на следующей итерации
//            #ifdef DEBUG_SERIAL
//                Serial.println("Retry after 4 seconds...");
//            #endif // DEBUG_SERIAL
//            delay(4000);
        }
    } else if(!tcpClient.isConnected()) {
        // Подождем подключения клиента
        
        if(tcpServer.availableClients() > 0) {
            // закроем старого клиента, если он использовался ранее
            tcpClient.close(); 

            if(tcpServer.acceptClient(&tcpClient)) {
                #ifdef DEBUG_SERIAL
                    Serial.println("Got a Connection: ");
                    printTcpClientStatus(&tcpClient);
                #endif // DEBUG_SERIAL
                                
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
            
            #ifdef DEBUG_SERIAL
                // Считали порцию данных
                read_buffer[readSize] = 0; // строка должна оканчиваться нулем
                Serial.print("Read: ");
                Serial.println(read_buffer);
            #endif // DEBUG_SERIAL
 
            // и можно выполнить команду, ответ попадет в write_buffer
            writeSize = handleInput(read_buffer, readSize, write_buffer);
            write_size = writeSize;
            
            // сбросим счетчик неактивности
            clientIdleStart = millis();
        }
            
        if(write_size > 0) {
            #ifdef DEBUG_SERIAL
                Serial.print("Write: ");
                Serial.print(write_buffer);
                Serial.println();
            #endif // DEBUG_SERIAL
            
            tcpClient.writeStream((const byte*)write_buffer, write_size);
            write_size = 0;
            
            // сбросим счетчик неактивности
            clientIdleStart = millis();
        }
        
        if( (millis() - clientIdleStart) > CLIENT_IDLE_TIMEOUT ) {
            Serial.println("Close connection on timeout");
            tcpClient.close();
        }
    }
}

