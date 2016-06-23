rraptor
=======

rraptor

Настройка среды разработки
1. Главный проект - скетч rrpaptor_mpide2
2. Скачать рядом репозиторий stepper_h
git clone https://github.com/1i7/stepper_h
3. Настроить библиотеки для среды Arduino (скопировать или 
сделать символические ссылки) в каталоге $HOME/Arduino/libraries

cd $HOME/Arduino/libraries
ln -s /home/benderamp/Documents/1i7/stepper_h/stepper_h
ln -s /home/benderamp/Documents/1i7/rraptor/rraptor_h
ln -s /home/benderamp/Documents/1i7/rraptor/rraptor_h/rraptor_serial
ln -s /home/benderamp/Documents/1i7/rraptor/rraptor_h/rraptor_tcp
ln -s /home/benderamp/Documents/1i7/rraptor/rraptor_h/rraptor_usb_accessory

4. Включать/выключать модули в файле rraptor_config.h
DEBUG_SERIAL - вывод отладочный сообщений в последовательный порт
RR_SERIAL - управление станком через последовательный порт
RR_TCP - управление станком через канал связи Wifi (не компилируется после 
    перехода на новый API ChipKIT, в работе)
RR_ROBOT_SERVER - управление станком через подключение к серверу роботов (не работает)
RR_USB_ACCESSORY - управление станком с устройства Android в режиме USB Accessory 
    (проверить с новыми библиотеками ChipKIT, скорее всего не заработает)

