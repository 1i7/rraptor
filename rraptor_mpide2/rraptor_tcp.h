#ifndef RRAPTOR_TCP_H
#define RRAPTOR_TCP_H

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
void wifi_configure(char* ssid, char* wpa2_passphrase, bool static_ip_en, char* static_ip);

/**
 * Подключиться к сети Wifi.
 */
void wifi_start();

/**
 * Разорвать подключение Wifi.
 */
void wifi_stop();

/**
 * Перезапустить подключение Wifi.
 */
void wifi_restart();


/**
 * Инициализировать канал связи Tcp.
 */
void rraptorTcpSetup();

/**
 * Работа канала связи Tcp, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorTcpTasks();

#endif // RRAPTOR_TCP_H

