
#include "WProgram.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

#include "rraptor_tcp.h"
#include "network_util.h"


/** 
 * Задать настройки подключения Wifi.
 * На входе список параметров и значений в формате: 
 * имя_параметра1=значение_параметра1 [имя_параметра2=значение_параметра2]
 */
int cmd_rr_configure_wifi(char* pnames[], char* pvalues[], int  pcount, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_configure_wifi: ");
        
        for(int i = 0; i < pcount; i++) {
            Serial.print(pnames[i]);
            Serial.print("=");
            Serial.print(pvalues[i]);
            Serial.print(" ");
        }
        Serial.println();
    #endif // DEBUG_SERIAL
    
}

/** 
 * Управление подключением Wifi
 * 
 * @param wifi_cmd дополнительная операция
 *     info: текущие параметры подключения
 *     status: вывести текущий статус подключения
 *     start: подключиться к Wifi
 *     stop:  отключиться от Wifi
 *     restart: перезапустить подключение Wifi
 */
int cmd_rr_wifi(char* wifi_cmd, char* reply_buffer) {
    #ifdef DEBUG_SERIAL
        Serial.print("cmd_rr_wifi: ");
        Serial.print(wifi_cmd);
        Serial.println();
    #endif // DEBUG_SERIAL
    
    if( strcmp(wifi_cmd, "status") == 0 ) {
        printNetworkStatus();
        
        // команда выполнена
        strcpy(reply_buffer, REPLY_OK);
    }
    
    
    return strlen(reply_buffer);
}

