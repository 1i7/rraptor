#include "WProgram.h"

#include <chipKITUSBHost.h>
#include <chipKITUSBAndroidHost.h>

#include "rraptor_config.h"
#include "rraptor_protocol.h"

// Информация о текущем устройстве
static char manufacturer[] = "Rraptor";
static char model[] = "Rraptor";
static char description[] = "Rraptor CNC controler";
static char version[] = "0.2-devel";
static char uri[] = "http://rraptor.lasto4ka.su";
static char serial[] = "N/A";

static ANDROID_ACCESSORY_INFORMATION myDeviceInfo = {
    manufacturer, sizeof(manufacturer),
    model, sizeof(model),
    description, sizeof(description),
    version, sizeof(version),
    uri, sizeof(uri),
    serial, sizeof(serial)
};

// Пин статуса подключения к Android
#define USB_ACCESSORY_STATUS_PIN 13

// Локальные переменные
static BOOL deviceAttached = FALSE;
static void* deviceHandle = NULL;

static BOOL readInProgress = FALSE;
static BOOL writeInProgress = FALSE;

static char read_buffer[128];
static char write_buffer[128];
static int write_size;

BOOL USBEventHandlerApplication( uint8_t address, USB_EVENT event, void *data, DWORD size ) {
    BOOL fRet = FALSE;

    // Вызываем обработчик событий для базового хост-контроллера
    // (это важно сделать, т.к. он также включает и выключает питание на ножках контроллера
    // по событиям EVENT_VBUS_REQUEST_POWER и EVENT_VBUS_RELEASE_POWER)
    fRet = USBHost.DefaultEventHandler(address, event, data, size);
  
    switch( event ) {
        // События от драйвера Android
        case EVENT_ANDROID_DETACH:
            #ifdef DEBUG_SERIAL
                Serial.println("Device NOT attached");
            #endif // DEBUG_SERIAL
            
            deviceAttached = FALSE;
            return TRUE;
            break;

        case EVENT_ANDROID_ATTACH:
            #ifdef DEBUG_SERIAL
                Serial.println("Device attached");
            #endif // DEBUG_SERIAL
            
            deviceAttached = TRUE;
            deviceHandle = data;
            return TRUE;

        default :
            break;
    }
    return fRet;
}

void rraptorUSBAccessorySetup() {
    // Инициализируем контроллер USB HOST:
    // Передаем ссылку на обработчик событий
    USBHost.Begin(USBEventHandlerApplication);
    // Передаем информацию об устройстве драйверу Android
    USBAndroidHost.AppStart(&myDeviceInfo);

    // Лампочка для тестов
    pinMode(USB_ACCESSORY_STATUS_PIN, OUTPUT);
}

void rraptorUSBAccessoryTasks() {
    DWORD readSize;
    DWORD writeSize;
    uint8_t errorCode;
    
    // Запускаем периодические задачи для поддержания стека USB в живом и корректном состоянии.
    // Следует выполнять их хотябы один раз внутри цикла или в момент, когда нужно
    // обновить внутреннее состояние контроллера USB хоста.
    USBTasks();

    if(deviceAttached) {      
        // Чтение данных с устройства Android - ждем команду
        if(!readInProgress) {
            // Вызов не блокируется - проверка завершения чтения через AndroidAppIsReadComplete
            errorCode = USBAndroidHost.AppRead(deviceHandle, (uint8_t*)&read_buffer, (DWORD)sizeof(read_buffer));
            if(errorCode == USB_SUCCESS) {
                // Дождались команду - новую читать не будем, пока не придут все данные,
                // проверять завершение операции будем в следующих итерациях цикла
                readInProgress = TRUE;
            } else {
                #ifdef DEBUG_SERIAL
                    Serial.print("Error trying to read: errorCode=");
                    Serial.println(errorCode, HEX);
                #endif // DEBUG_SERIAL
            }
        }

        // Проверим, завершилось ли чтение
        if(USBAndroidHost.AppIsReadComplete(deviceHandle, &errorCode, &readSize)) {
            // Разрешим читать следующую команду
            readInProgress = FALSE;
            
            if(errorCode == USB_SUCCESS) {
                #ifdef DEBUG_SERIAL
                    // Считали порцию данных
                    read_buffer[readSize] = 0; // строка должна оканчиваться нулем
                    Serial.print("Read: ");
                    Serial.println(read_buffer);
                #endif // DEBUG_SERIAL
                
                // и можно выполнить команду, ответ попадет в write_buffer
                writeSize = handleInput(read_buffer, readSize, write_buffer);
                                
                // Если writeSize не 0, отправим назад ответ - инициируем 
                // процедуру записи для следующей итерации цикла (данные уже внутри write_buffer)
                write_size = writeSize;
            } else {
                #ifdef DEBUG_SERIAL
                    Serial.print("Error trying to complete read: errorCode=");
                    Serial.println(errorCode, HEX);
                #endif // DEBUG_SERIAL
            }
        }
        
        // Отправка данных на устройство Android
        if(write_size > 0 && !writeInProgress) {
            #ifdef DEBUG_SERIAL
                Serial.print("Write: ");
                Serial.print(write_buffer);
                Serial.println();
            #endif // DEBUG_SERIAL
          
            writeSize = write_size;
            // Нужная команда уже в буфере для отправки
            // Вызов не блокируется - проверка завершения чтения через AndroidAppIsWriteComplete
            errorCode = USBAndroidHost.AppWrite(deviceHandle, (uint8_t*)&write_buffer, writeSize);
                        
            if(errorCode == USB_SUCCESS) {
                writeInProgress = TRUE;
            } else {
                #ifdef DEBUG_SERIAL
                    Serial.print("Error trying to write: errorCode=");
                    Serial.println(errorCode, HEX);
                #endif // DEBUG_SERIAL
                
                write_size = 0;
            }
        }
        
        if(writeInProgress) {
            // Проверим, завершена ли запись
            if(USBAndroidHost.AppIsWriteComplete(deviceHandle, &errorCode, &writeSize)) {
                writeInProgress = FALSE;
                write_size = 0;
    
                if(errorCode != USB_SUCCESS) {
                    #ifdef DEBUG_SERIAL
                        Serial.print("Error trying to complete write: errorCode=");
                        Serial.println(errorCode, HEX);
                    #endif // DEBUG_SERIAL
                }
            }
        }
    }
}

