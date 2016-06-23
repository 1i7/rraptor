#ifndef RRAPTOR_ROBOT_SERVER_H
#define RRAPTOR_ROBOT_SERVER_H

/**
 * Инициализировать канал связи с Сервером Роботов.
 */
void rraptorRobotServerSetup();

/**
 * Работа канала связи с Сервером Роботов, запускать в loop. 
 * При получении команды, вызывает handleInput.
 */
void rraptorRobotServerTasks();

#endif // RRAPTOR_ROBOT_SERVER_H


