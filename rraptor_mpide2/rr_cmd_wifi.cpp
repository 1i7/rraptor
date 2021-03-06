
#include "Arduino.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

// для TCP
#ifdef RR_TCP
#include "rraptor_tcp.h"
#else
#pragma message( "RR_TCP module is not enabled, wifi configuration commands will not work" )
#endif // RR_TCP


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
#ifdef RR_TCP    
    // для опущенных параметров использовать значения по умолчанию
    char* ssid = NULL;
    char* password = NULL;
    bool static_ip_en = false;
    char* static_ip = NULL;

    // достанем значения из параметров
    for(int i = 0; i < pcount; i++) {
        if( strcmp(pnames[i], CONFIGURE_WIFI_PARAM_SSID) == 0 ) {
            ssid = pvalues[i];
        } else if( strcmp(pnames[i], CONFIGURE_WIFI_PARAM_PASSWORD) == 0 ) {
            password = pvalues[i];
        } else if( strcmp(pnames[i], CONFIGURE_WIFI_PARAM_STATIC_IP_EN) == 0 ) {
            if( strcmp(pvalues[i], "true") || strcmp(pvalues[i], "TRUE") ) {
                static_ip_en = true;
            } else {
                static_ip_en = false;
            }
        } else if( strcmp(pnames[i], CONFIGURE_WIFI_PARAM_STATIC_IP) == 0 ) {
            static_ip = pvalues[i];
        }
    }
    
    // обязательно указать только имя сети
    if(ssid != NULL) {
        wifi_configure(ssid, password, static_ip_en, static_ip);
        
        // команда выполнена
        strcpy(reply_buffer, REPLY_OK);
    } else {
        // ошибка - не задано имя сети
        strcpy(reply_buffer, REPLY_ERROR);
    }
    
    return strlen(reply_buffer);
#else
    return 0;
#endif // RR_TCP
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
#ifdef RR_TCP
    if( strcmp(wifi_cmd, WIFI_PARAM_INFO) == 0 ) {
        char ssid[32]; // DWIFIcK:: WF_MAX_SSID_LENGTH
        char password[128];
        bool static_ip_en;
        char static_ip[16];
        
        // получим актуальные значения из модуля wifi
        wifi_info(ssid, password, &static_ip_en, static_ip);
        
        //
        sprintf(reply_buffer, "%s=%s", CONFIGURE_WIFI_PARAM_SSID, ssid);
        if(password != NULL && strlen(password) > 0) {
            sprintf(reply_buffer+strlen(reply_buffer), " %s=%s", CONFIGURE_WIFI_PARAM_PASSWORD, password);
        }
        sprintf(reply_buffer+strlen(reply_buffer), " %s=%s", CONFIGURE_WIFI_PARAM_STATIC_IP_EN, static_ip_en ? "true" : "false");
        if(static_ip_en) {
            sprintf(reply_buffer+strlen(reply_buffer), " %s=%s", CONFIGURE_WIFI_PARAM_STATIC_IP, static_ip);
        }
        // команда выполнена
        
    } else if( strcmp(wifi_cmd, WIFI_PARAM_STATUS) == 0 ) {
        char ssid[32]; // DWIFIcK:: WF_MAX_SSID_LENGTH
        char host_ip[16];
        char dns1[16];
        char dns2[16];
        char gateway[16];
        char subnet_mask[16];
      
        wifi_status(ssid, host_ip, dns1, dns2, gateway, subnet_mask);
      
        sprintf(reply_buffer,"ssid=%s ipv4_address=%s dns1=%s dns2=%s gatewy=%s subnet_mask=%s", 
            ssid, host_ip, dns1, dns2, gateway, subnet_mask);
        
        // команда выполнена
        
    } else if( strcmp(wifi_cmd, WIFI_PARAM_START) == 0 ) {
        wifi_start();
        
        // команда выполнена
        strcpy(reply_buffer, REPLY_OK);
        
    } else if( strcmp(wifi_cmd, WIFI_PARAM_STOP) == 0 ) {
        wifi_stop();
        
        strcpy(reply_buffer, REPLY_OK);
        
    } else if( strcmp(wifi_cmd, WIFI_PARAM_RESTART) == 0 ) {
        wifi_restart();
        
        // команда выполнена
        strcpy(reply_buffer, REPLY_OK);
    }
    
    return strlen(reply_buffer);
#else
    return 0;
#endif // RR_TCP
}


