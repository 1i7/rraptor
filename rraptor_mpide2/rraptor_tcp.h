#ifndef RRAPTOR_TCP_H
#define RRAPTOR_TCP_H

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

