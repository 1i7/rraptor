#include "WProgram.h"

#include <WiFiShieldOrPmodWiFi_G.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

#include "rraptor_config.h"
#include "network_util.h"
#include "rraptor_protocol.h"

// Пин статуса подключения к Wifi
#define WIFI_STATUS_PIN 13

/**
 * Включить/выключить Wifi
 */
static bool wifi_on = true;
/**
 * Перезапустить Wifi
 */
static bool _wifi_restart = false;

// Значения для подключений

// Точка доступа ВайФай
static const char* wifi_ssid = "lasto4ka";
static const char* wifi_wpa2_passphrase = "robotguest";
//static const char* wifi_ssid = "helen";
//static const char* wifi_wpa2_passphrase = "13april1987";

/**
 * Какой адрес использовать при подключении к точке Wifi:
 *   true - использовать статический
 *   false - динамический
 * (при прямом управлении с Пульта лучше использовать статический, 
 * иначе Пульт не узнает, куда подключаться)
 */
static bool wifi_static_ip_en = true;

/**
 * Желаемый статический IP-адрес текущего устройства - 
 * попросим у точки Wifi, если static_ip_en=true
 */
//IPv4 wifi_static_ip = {192,168,115,115};
static IPv4 wifi_static_ip = {192,168,43,115};
//static IPv4 wifi_static_ip = {192,168,1,115};
//IPv4 wifi_static_ip = DNETcK::zIPv4;

// Порт для tcp-сервера
static const int tcp_server_port = DNETcK::iPersonalPorts44 + 114;

static int conectionId = DWIFIcK::INVALID_CONNECTION_ID;

static TcpServer tcpServer;
static TcpClient tcpClient;

// Таймаут неактивности подключенного клиента, миллисекунды
// (отключаем клиента, если он давно не присылал никакие команды)
static int CLIENT_IDLE_TIMEOUT = 10000;
static int clientIdleStart = 0;

// Размеры буферов для чтения команд и записи ответов 
#ifndef CMD_READ_BUFFER_SIZE
#define CMD_READ_BUFFER_SIZE 128
#endif

#ifndef CMD_WRITE_BUFFER_SIZE
#define CMD_WRITE_BUFFER_SIZE 512
#endif

// Буфер для чтения ввода и записи вывода
static char read_buffer[CMD_READ_BUFFER_SIZE];
static char write_buffer[CMD_WRITE_BUFFER_SIZE];
static int write_size;


/**
 * Задать настройки для подключения к сети Wifi
 *
 * @param ssid имя беспроводной сети
 * @param wpa2_passphrase пароль для подключения (NULL для подключения к открытой сети)
 * @param static_ip_en какой IP-адрес использовать при подключении к точке Wifi:
 *     true - использовать статический
 *     false - динамический
 *   (при прямом управлении с Пульта лучше использовать статический, 
 *   иначе Пульт не узнает, куда подключаться)
 * @param static_ip желаемый статический IP-адрес текущего устройства - 
 *     попросим у точки Wifi, если static_ip_en=true
 */
void wifi_configure(char* ssid, char* wpa2_passphrase, bool static_ip_en, char* static_ip) {
    wifi_ssid = ssid;
    wifi_wpa2_passphrase = wpa2_passphrase;
    wifi_static_ip_en = static_ip_en;
    wifi_static_ip = parseIPAddress(static_ip);
}

/**
 * Подключиться к сети Wifi.
 */
void wifi_start() {
    #ifdef DEBUG_SERIAL
        Serial.println("Starting wifi...");
    #endif // DEBUG_SERIAL
    
    // процесс подключения запустится на следующей итерации rraptorTcpTasks()
    wifi_on = true;
}

/**
 * Разорвать подключение Wifi.
 */
void wifi_stop() {
    #ifdef DEBUG_SERIAL
        Serial.println("Stopping wifi...");
    #endif // DEBUG_SERIAL
    
    // процесс отключения запустится на следующей итерации rraptorTcpTasks()
    wifi_on = false;
}

/**
 * Перезапустить подключение Wifi.
 */
void wifi_restart() {
    #ifdef DEBUG_SERIAL
        Serial.println("Restarting wifi...");
    #endif // DEBUG_SERIAL
    
    // процесс переподключения запустится на следующей итерации rraptorTcpTasks()
    wifi_on = false;
    _wifi_restart = true;
}

/**
 * Вывести статус сервера: адрес:порт.
 */
static void printTcpServerStatus() {
    IPv4 host_ip;
    
    if( DNETcK::getMyIP(&host_ip) ) { 
        printIPAddress(&host_ip);
        Serial.print(":");
        Serial.println(tcp_server_port);
    } else {
        Serial.println("IP not assigned");
    }
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
    if(wifi_wpa2_passphrase == NULL) {
        // подключиться к открытой сети
        conId = connectWifiOpen(wifi_ssid, netStatus);
    } else {
        // подключиться к сети с паролем WPA2
        conId = connectWifiWPA2Passphrase(wifi_ssid, wifi_wpa2_passphrase, netStatus);
    }
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


/**
 * Работа канала связи Tcp, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorTcpTasks() {
    DNETcK::STATUS networkStatus;
    int readSize;
    int writeSize;
    
    // static не будет менять значение между вызовами текущего метода
    static bool postConnectDone = false;
    
    // Держим Tcp-стек в живом состоянии
    DNETcK::periodicTasks();
    
    
    if(!wifi_on || _wifi_restart) {
        // Если были подключены и пришел запрос отключиться, завершим стек Wifi и Tcp,
        // если перезапускаем сеть, то завершим все стеки в любом случае на всякий случай
        if(DWIFIcK::isConnected(conectionId) || _wifi_restart) {
            DNETcK::end();
            DWIFIcK::disconnect(conectionId);
            conectionId = DWIFIcK::INVALID_CONNECTION_ID;
            postConnectDone = false;
            
            if(_wifi_restart) {
                _wifi_restart = false;
                wifi_on = true;
            }
        }
    } else if(!DWIFIcK::isConnected(conectionId, &networkStatus)) {
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
            
                if(wifi_static_ip_en) {
                    // Подключимся со статическим ip-адресом
                    DNETcK::begin(wifi_static_ip);
                } else {
                    // Подключимся с динамическим ip-адресом
                    DNETcK::begin();
                }
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
        // Подождем, пока сокет начнет слушать подключения (выполняется быстро, 
        // если бы долго, можно было бы сделать неблокирующим)
        bool starting = true;
        while(starting) {
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
            
            // будем пробовать переподключиться на следующей итерации
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
                Serial.print(" (size=");
                Serial.print(write_size);
                Serial.println(")");
            #endif // DEBUG_SERIAL
            
            tcpClient.writeStream((const byte*)write_buffer, write_size);
            write_size = 0;
            
            // сбросим счетчик неактивности
            clientIdleStart = millis();
        }
        
        // Отключаем клиента, если он давно не присылал никакие команды
        if( (millis() - clientIdleStart) > CLIENT_IDLE_TIMEOUT ) {
            Serial.println("Close connection on timeout");
            tcpClient.close();
        }
    }
}

