
#include "WProgram.h"

#include "rraptor_config.h"
#include "rraptor_protocol.h"

// Буферы для обмена данными с компьютером через последовательный порт
static char read_buffer[128];
static char write_buffer[128];
static int write_size;

/**
 * Инициализировать канал связи последовательный порт Serial.
 */
void rraptorSerialSetup() {
    #ifndef DEBUG_SERIAL
        Serial.begin(9600);
    #endif // DEBUG_SERIAL 
}

/**
 * Работа канала связи последовательный порт Serial, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorSerialTasks() {    
    int readSize = 0;
    int writeSize = 0;
    
    while (Serial.available() > 0) {
        read_buffer[readSize] = Serial.read();
        readSize++;
        
        // хак: без этой задержки при вводе команд в окно mpide Tools/Serial monitor
        // первый символ введенной строки отделяется от остальных и воспринимается
        // этим кодом как отдельная строка
        if(readSize == 1) {
            delay(100);
        }
    }
    if(readSize > 0) {
        #ifdef DEBUG_SERIAL
            // Считали порцию данных
            read_buffer[readSize] = 0; // строка должна оканчиваться нулем
            Serial.print("Read: ");
            Serial.print(read_buffer);
            Serial.print(" (size=");
            Serial.print(readSize);
            Serial.println(")");
        #endif // DEBUG_SERIAL
        
        // теперь можно выполнить команду, ответ попадет в write_buffer
        writeSize = handleInput(read_buffer, readSize, write_buffer);
        write_size = writeSize;
    }
    
    // отправляем ответ
    if(write_size > 0) {
        #ifdef DEBUG_SERIAL
            Serial.print("Write: ");
            Serial.print(write_buffer);
            Serial.print(" (size=");
            Serial.print(write_size);
            Serial.println(")");
        #endif // DEBUG_SERIAL
            
        Serial.println(write_buffer);
        write_size = 0;
    }
}

