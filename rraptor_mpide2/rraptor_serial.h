#ifndef RRAPTOR_SERIAL_H
#define RRAPTOR_SERIAL_H

/**
 * Инициализировать канал связи последовательный порт Serial.
 */
void rraptorSerialSetup();

/**
 * Работа канала связи последовательный порт Serial, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorSerialTasks();

#endif // RRAPTOR_SERIAL_H

