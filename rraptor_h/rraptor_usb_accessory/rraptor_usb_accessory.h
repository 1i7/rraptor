#ifndef RRAPTOR_USB_ACCESSORY_H
#define RRAPTOR_USB_ACCESSORY_H

/**
 * Инициализировать канал связи USB Accessory.
 */
void rraptorUSBAccessorySetup();

/**
 * Работа канала связи USB Accessory, запускать в loop. При получении
 * команды, вызывает handleInput.
 */
void rraptorUSBAccessoryTasks();

#endif // RRAPTOR_USB_ACCESSORY_H


