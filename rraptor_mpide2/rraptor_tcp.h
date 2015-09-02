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
 * Получить информацию о настройках подключения к сети Wifi. Значения записываются в переменные,
 * переданные по указалям в параметрах.
 * 
 * @param ssid ссылка на массив символов для записи имени сети (32 символа)
 * @param wpa2_passphrase ссылка на массив символов для записи пароля (128 символов)
 * @param static_ip_en ссылка на флаг режима использования статического IP-адреса
 * @param static_ip ссылка на массив символов для записи желаемого статического IP-адреса (16 символов)
 */
void wifi_info(char* ssid, char* wpa2_passphrase, bool* static_ip_en, char* static_ip);

/**
 * Получить информацию о текущем подключении к сети Wifi.
 *
 * @param ssid имя сети
 * @param host_ip ip-адрес
 * @param dns1 DNS1
 * @param dns2 DNS2
 * @param gateway основной шлюз
 * @param subnet_mask маска подсети
 */
void wifi_status(char* ssid, char* host_ip, char* dns1, char* dns2, char* gateway, char* subnet_mask);

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


