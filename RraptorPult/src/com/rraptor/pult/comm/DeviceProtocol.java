package com.rraptor.pult.comm;

public interface DeviceProtocol {

    /**************************************/
    // Универсальные команды (для Сервера Роботов)

    // Постоянные свойства
    /** Получить собственное имя устройства */
    public static final String CMD_NAME = "name";
    /** Получить модель устройства */
    public static final String CMD_MODEL = "model";
    /** Получить серийный номер устройства */
    public static final String CMD_SERIAL_NUMBER = "sn";
    /** Получить словесное описание устройства */
    public static final String CMD_DESCRIPTION = "description";
    /** Получить версию прошивки устройства */
    public static final String CMD_VERSION = "version";
    /** Получить производителя устройства */
    public static final String CMD_MANUFACTURER = "manufacturer";
    /** Получить ссылку на страницу устройства */
    public static final String CMD_URI = "uri";

    // Команды
    /** Проверить доступность устройства */
    public static final String CMD_PING = "ping";

    /**************************************/
    // Команды Rraptor

    // Постоянные свойства
    /** Получить размер рабочей области */
    public static final String CMD_RR_WORKING_AREA_DIM = "rr_working_area_dim";

    // Динамические свойства
    /** Получить статус устройства */
    public static final String CMD_RR_STATUS = "rr_status";
    /** Получить текущее положение печатающего блока */
    public static final String CMD_RR_CURRENT_POSITION = "rr_current_pos";

    // Команды
    public static final String CMD_RR_STOP = "rr_stop";

    public static final String CMD_RR_GO_X_FORWARD = "rr_go x 1";
    public static final String CMD_RR_GO_X_BACKWARD = "rr_go x -1";
    public static final String CMD_RR_GO_Y_FORWARD = "rr_go y 1";
    public static final String CMD_RR_GO_Y_BACKWARD = "rr_go y -1";
    public static final String CMD_RR_GO_Z_FORWARD = "rr_go z 1";
    public static final String CMD_RR_GO_Z_BACKWARD = "rr_go z -1";

    public static final String CMD_RR_CALIBRATE_X_FORWARD = "rr_calibrate x 1";
    public static final String CMD_RR_CALIBRATE_X_BACKWARD = "rr_calibrate x -1";
    public static final String CMD_RR_CALIBRATE_Y_FORWARD = "rr_calibrate y 1";
    public static final String CMD_RR_CALIBRATE_Y_BACKWARD = "rr_calibrate y -1";
    public static final String CMD_RR_CALIBRATE_Z_FORWARD = "rr_calibrate z 1";
    public static final String CMD_RR_CALIBRATE_Z_BACKWARD = "rr_calibrate z -1";

    /**************************************/
    // Команды G-кода

    public static final String CMD_GCODE_G0 = "G0";
    public static final String CMD_GCODE_G01 = "G01";
    public static final String CMD_GCODE_G02 = "G02";
    public static final String CMD_GCODE_G03 = "G03";

    public static final String REPLY_OK = "ok";
    public static final String REPLY_DONTUNDERSTAND = "dontunderstand";
    public static final String REPLY_ERROR = "error";
    public static final String REPLY_BUSY = "busy";

    /**
     * Разделитель команд в группе. Команды выполняются на устройстве за один
     * сеанс связи одна за одной. Ответы от выполнения каждой из командв в
     * группе объединяются и присылаются одним пакетом, также разделенные этим
     * же разделителем, в порядке выполнения команд.
     */
    public static final String COMMAND_SEPARATOR = ";";

    // Статус устройства
    public static final String STATUS_WORKING = "working";
    public static final String STATUS_IDLE = "idle";
}
