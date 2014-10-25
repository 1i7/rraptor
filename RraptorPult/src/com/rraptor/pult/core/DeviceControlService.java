package com.rraptor.pult.core;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.concurrent.TimeoutException;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnectionWifi;
import com.rraptor.pult.comm.DeviceProtocol;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point3D;
import com.rraptor.pult.view.PlotterAreaView.LineDrawingStatus;

public class DeviceControlService extends Service {

    /**
     * Информация о команде для выполнения на устройстве.
     * 
     * @author Anton Moiseev
     * 
     */
    private class CommandInfo {
        private final String command;
        private final CommandListener commandListener;

        public CommandInfo(String command, CommandListener commandListener) {
            super();
            this.command = command;
            this.commandListener = commandListener;
        }

        public String getCommand() {
            return command;
        }

        public CommandListener getCommandListener() {
            return commandListener;
        }
    }

    /**
     * Обратный вызов для получения результата выполения команды, добавленной в
     * очередь на выполнение в фоновом потоке. Выполнение команды может быть
     * завершено двумя способами: команда выполнена на устройстве и получен
     * ответ, выполнение команды было отменено до выполнения ее на устройстве.
     * 
     * Логика работы очереди команд лежит выше логики связи с устройством. Если
     * связь с устройством потеряна или произошла ошибка отправки команды по
     * выбранному каналу связи, команда не будет автоматически отменена, просто
     * ее выполнение будет отложено до восстановления связи с устройством, тогда
     * попытки отправки на устройство будут продолжены.
     * 
     * Однако, в случае разрыва связи с устройством система (например по решению
     * пользователя) может отменить выполнение всех команд из очереди, которые
     * ранее так и не попали на устройство.
     * 
     */
    public interface CommandListener {
        /**
         * Выполнение команды отменено до выполнения на устройстве.
         * 
         * @param cmd
         */
        void onCommandCanceled(final String cmd);

        /**
         * Команды была выполнена на устройстве, получен ответ.
         * 
         * @param cmd
         *            выполненная команда
         * @param reply
         *            ответ от устройства
         */
        void onCommandExecuted(final String cmd, final String reply);
    }

    public enum ConnectionStatus {
        DISCONNECTED, CONNECTING, HANDSHAKE, CONNECTED, ERROR
    }

    /**
     * Class used for the client Binder. Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class DeviceControlBinder extends Binder {
        public DeviceControlService getService() {
            return DeviceControlService.this;
        }
    }

    public enum DeviceStatus {
        UNKNOWN, WORKING, IDLE
    }

    /**
     * Слушает результат выполнения группы команд (команды следуют одна за
     * одной, склеены разделителем) и распределяет ответы между слушателями
     * каждой из комманд в группе.
     * 
     * @author Anton Moiseev
     * 
     */
    public static class MultyCommandListener implements CommandListener {
        private final CommandListener[] cmdListeners;

        public MultyCommandListener(final CommandListener[] cmdListeners) {
            this.cmdListeners = cmdListeners;
        }

        @Override
        public void onCommandCanceled(final String cmd) {
            // разбить склеенную команду на части:
            final String[] commands = cmd
                    .split(DeviceProtocol.COMMAND_SEPARATOR);

            // количество элементов во всех массивах должно совпадать
            for (int i = 0; i < commands.length; i++) {
                if (cmdListeners != null && cmdListeners[i] != null) {
                    cmdListeners[i].onCommandCanceled(commands[i]);
                }
            }
        }

        @Override
        public void onCommandExecuted(final String cmd, final String reply) {
            // разбить склеенную команду и склеенный ответ на части:
            final String[] commands = cmd
                    .split(DeviceProtocol.COMMAND_SEPARATOR);
            final String[] replies = reply
                    .split(DeviceProtocol.COMMAND_SEPARATOR);

            // количество элементов во всех массивах должно совпадать
            for (int i = 0; i < commands.length; i++) {
                if (cmdListeners != null && cmdListeners[i] != null) {
                    cmdListeners[i].onCommandExecuted(commands[i], replies[i]);
                }
            }
        }
    }

    // Публичные события
    // Подключение к устройству
    public static String ACTION_CONNECTION_STATUS_CHANGE = "com.rraptor.pult.CONNECTION_STATUS_CHANGE";
    public static String ACTION_DEVICE_STATUS_CHANGE = "com.rraptor.pult.DEVICE_STATUS_CHANGE";

    public static String EXTRA_CONNECTION_STATUS = "EXTRA_CONNECTION_STATUS";
    public static String EXTRA_DEVICE_STATUS = "EXTRA_DEVICE_STATUS";

    public static String ACTION_DEVICE_CURRENT_POS_CHANGE = "com.rraptor.pult.ACTION_DEVICE_CURRENT_POS_CHANGE";

    public static String EXTRA_CURRENT_POS = "EXTRA_CURRENT_POS";

    // Отладка
    public static String ACTION_DEBUG_MESSAGE_POSTED = "com.rraptor.pult.ACTION_DEBUG_MESSAGE_POSTED";
    public static String EXTRA_DEBUG_MESSAGE = "EXTRA_DEBUG_MESSAGE";

    // Рисование
    public static String ACTION_DEVICE_START_DRAWING = "com.rraptor.pult.ACTION_DEVICE_START_DRAWING";
    public static String ACTION_DEVICE_FINISH_DRAWING = "com.rraptor.pult.ACTION_DEVICE_FINISH_DRAWING";
    public static String ACTION_DEVICE_DRAWING_UPDATE = "com.rraptor.pult.ACTION_DEVICE_DRAWING_UPDATE";
    public static String ACTION_DEVICE_DRAWING_ERROR = "com.rraptor.pult.ACTION_DEVICE_DRAWING_ERROR";

    // Очередь команд рисования
    public static String ACTION_DEVICE_DRAWING_PAUSED = "com.rraptor.pult.ACTION_DRAWING_PAUSED";
    public static String ACTION_DEVICE_DRAWING_RESUMED = "com.rraptor.pult.ACTION_DEVICE_DRAWING_RESUMED";

    public static String EXTRA_EXCEPTION = "EXTRA_EXCEPTION";
    public static String EXTRA_LINE = "EXTRA_LINE";
    public static String EXTRA_LINE_STATUS = "EXTRA_LINE_STATUS";

    //
    private final IBinder deviceControlBinder = new DeviceControlBinder();

    private final DeviceConnection deviceConnection = new DeviceConnectionWifi();

    private final DeviceDrawingManager deviceDrawindManager = new DeviceDrawingManager(
            this);
    private final DeviceStatusManager deviceStatusManager = new DeviceStatusManager(
            this);

    // Информация о текущем поключении
    private DeviceControlService.ConnectionStatus connectionStatus = DeviceControlService.ConnectionStatus.DISCONNECTED;
    private String connectionInfo;
    private String connectionErrorMessage;
    private boolean isSameDevice = true;

    // Информация об очереди команд
    /**
     * "Очередь" команд для выполнения на сервере, состоящая из одного элемента.
     */
    private CommandInfo nextCommand;

    // Информация о подключенном устройстве
    private String deviceName;
    private String deviceModel;
    private String deviceSerialNumber;
    private String deviceDescription;
    private String deviceVersion;
    private String deviceManufacturer;
    private String deviceUri;

    private Point3D deviceWorkingArea;
    private DeviceStatus deviceStatus = DeviceStatus.UNKNOWN;
    private Point3D deviceCurrentPosition;

    // Информация об устройстве
    /**
     * Обрабатывает результат команды получения текущего статуса устройства -
     * устанавливает значение deviceStatus.
     */
    final CommandListener deviceStatusCommandListener = new CommandListener() {

        @Override
        public void onCommandCanceled(final String cmd) {
            // ничего не делаем
        }

        @Override
        public void onCommandExecuted(final String cmd, final String reply) {
            if (DeviceProtocol.CMD_RR_STATUS.equals(cmd)) {
                if (DeviceProtocol.STATUS_IDLE.equals(reply)) {
                    setDeviceStatus(DeviceStatus.IDLE);
                } else if (DeviceProtocol.STATUS_WORKING.equals(reply)) {
                    setDeviceStatus(DeviceStatus.WORKING);
                } else {
                    setDeviceStatus(DeviceStatus.UNKNOWN);
                }
            }
        }
    };

    /**
     * Обрабатывает результат команды получения текущей позиции рабочего блока
     * устройства - устанавливает новое положение deviceCurrentPosition.
     */
    final CommandListener deviceCurrentPositionCommandListener = new CommandListener() {

        @Override
        public void onCommandCanceled(final String cmd) {
            // ничего не делать
        }

        @Override
        public void onCommandExecuted(final String cmd, final String reply) {
            // получить значения из строки, перевести микрометры в
            // миллиметры
            final String[] pos_parts = reply.split(" ");
            double x = Double.parseDouble(pos_parts[0]) / 1000;
            double y = Double.parseDouble(pos_parts[1]) / 1000;
            double z = Double.parseDouble(pos_parts[2]) / 1000;

            final Point3D newPos = new Point3D(x, y, z);

            if (deviceCurrentPosition == null
                    || !deviceCurrentPosition.equals(newPos)) {
                // обновить значения свойств устройства
                deviceCurrentPosition = newPos;
                fireOnDeviceCurrentPosChange(deviceCurrentPosition);
            }
        }
    };

    // Отладочные сообщения
    private final StringBuilder debugMessages = new StringBuilder();

    // Подключение к устройству по Wifi
    private Socket socket;
    private OutputStream serverOut;
    private InputStream serverIn;

    /**
     * Очистить "очередь" команд - отменить выполнение всех команд, которые до
     * сих пор не были выполнены на устройстве.
     */
    public void cancelCommands() {
        if (nextCommand != null && nextCommand.getCommandListener() != null) {
            nextCommand.getCommandListener().onCommandCanceled(
                    nextCommand.getCommand());
        }

        nextCommand = null;
    }

    /**
     * Очистить историю отладочных сообщений.
     */
    public void clearDebugMessages() {
        debugMessages.setLength(0);
    }

    /**
     * Подлключиться к устройству через канал Tcp и запустить процесс отправки
     * команд.
     * 
     */
    public void connectToDeviceTcp() {
        connectToDeviceTcp(DeviceConnectionWifi.DEFAULT_SERVER_HOST,
                DeviceConnectionWifi.DEFAULT_SERVER_PORT);
    }

    /**
     * Подлключиться к устройству через канал Tcp и запустить процесс отправки
     * команд.
     */
    public void connectToDeviceTcp(final String serverHost, final int serverPort) {
        // Все сетевые операции нужно делать в фоновом потоке, чтобы не
        // блокировать интерфейс
        new Thread() {
            @Override
            public void run() {
                try {
                    debug("Connecting to server: " + serverHost + ":"
                            + serverPort + "...");
                    setConnectionStatus(ConnectionStatus.CONNECTING);

                    socket = new Socket(serverHost, serverPort);

                    // Подключились к серверу:
                    // Установим таймаут для чтения ответа на команды -
                    // сервер должет прислать ответ за 5 секунд, иначе он будет
                    // считаться отключенным (в нашем случае это позволит
                    // предотвратить вероятные зависания на блокирующем read,
                    // когда например робот отключился до того, как прислал
                    // ответ и сокет не распрознал это как разрыв связи с
                    // выбросом IOException)
                    socket.setSoTimeout(DeviceConnectionWifi.SERVER_SO_TIMEOUT);

                    // Получаем доступ к потокам ввода/вывода сокета для общения
                    // с сервером (роботом)
                    serverOut = socket.getOutputStream();
                    serverIn = socket.getInputStream();

                    debug("Connected: ");
                    connectionInfo = socket.getInetAddress().getHostName()
                            + ":" + socket.getPort();
                    debug(connectionInfo);

                    // подключились - канал связи есть, но еще нужно узнать,
                    // с кем именно имеем дело
                    setConnectionStatus(ConnectionStatus.HANDSHAKE);

                    // запросим информацию об устройстве и заодно проверим,
                    // то ли это устройство, к которому мы были подключены в
                    // прошлый раз, или новое:
                    isSameDevice = retrieveDeviceInfo();

                    // рабочая область устройства
                    retrieveDeviceWorkingArea();

                    // теперь окончательно подключились:
                    setConnectionStatus(ConnectionStatus.CONNECTED);

                    // приступим к постоянному опросу устройства
                    deviceStatusManager.startPollingDeviceStatus();
                } catch (final Exception e) {

                    debug("Error connecting to server: " + e.getMessage());
                    e.printStackTrace();

                    socket = null;
                    serverOut = null;
                    serverIn = null;

                    connectionErrorMessage = e.getMessage();
                    setConnectionStatus(ConnectionStatus.ERROR);
                }
            }
        }.start();
    }

    /**
     * Отладочные сообщения.
     * 
     * @param msg
     */
    public void debug(String msg) {
        System.out.println(msg);
        if (debugMessages.length() > 4000) {
            debugMessages.delete(0, 500);
        }
        debugMessages.append(msg + "\n");
        fireOnDebugMessagePosted(msg);
    }

    /**
     * Отключиться от устройства - закрыть все потоки и сокет, обнулить
     * переменные.
     */
    public void disconnectFromServer() {
        // мы должны сменить статус подключения в самом начале процедуры,
        // чтобы поток отправки сообщений не пытался отправлять команды
        // в закрываемые сокеты (вероятность, что это произойдет все равно
        // остается, но это не критическая ошибка)
        this.connectionStatus = ConnectionStatus.DISCONNECTED;

        // почистим ресурсы
        try {
            if (serverIn != null) {
                serverIn.close();
            }
            if (serverOut != null) {
                serverOut.close();
            }
            if (socket != null) {
                socket.close();
            }
        } catch (final IOException e) {
            e.printStackTrace();
        } finally {
            serverIn = null;
            serverOut = null;
            socket = null;

            // очистим "очередь" команд
            cancelCommands();

            debug("Disconnected");
            // а событие о смене статуса подключения отправляем в самом конце,
            // т.к. если отправить его в начале, остается возможность, что
            // подписанты могут попробовать переподключиться в тот момент,
            // когда еще не завершился процесс очистки ресурсов старого
            // подключения
            fireOnConnectionStatusChange(ConnectionStatus.DISCONNECTED);
        }
    }

    /**
     * Выполнить команду на устройстве и сразу получить ответ.
     * 
     * @return ответ от устройства
     */
    private String execCommandOnDevice(final String cmd) throws IOException,
            TimeoutException {
        String reply;
        // отправить команду на устройство
        debug("Write: " + cmd);
        serverOut.write((cmd).getBytes());
        serverOut.flush();

        // и сразу прочитать ответ
        final byte[] readBuffer = new byte[256];
        final int readSize = serverIn.read(readBuffer);

        if (readSize != -1) {
            reply = new String(readBuffer, 0, readSize);
            debug("Read: " + "num bytes=" + readSize + ", value=" + reply);
        } else {
            throw new IOException("End of stream");
        }
        return reply;
    }

    /**
     * Выбрать первую команду из очереди; выбранная команда удаляется.
     * 
     * @return первая команда; null, если очередь пуста.
     */
    private CommandInfo fetchCommand() {
        final CommandInfo cmdInfo = nextCommand;
        // удалить выбранную команду из очереди
        nextCommand = null;
        return cmdInfo;
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_CONNECTION_STATUS_CHANGE.
     * 
     * @param e
     */
    private void fireOnConnectionStatusChange(final ConnectionStatus status) {
        final Intent intent = new Intent(ACTION_CONNECTION_STATUS_CHANGE);
        intent.putExtra(EXTRA_CONNECTION_STATUS, status);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEBUG_MESSAGE_POSTED.
     * 
     * @param e
     */
    void fireOnDebugMessagePosted(final String msg) {
        final Intent intent = new Intent(ACTION_DEBUG_MESSAGE_POSTED);
        intent.putExtra(EXTRA_DEBUG_MESSAGE, msg);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_CURRENT_POS_CHANGE.
     * 
     * @param e
     */
    void fireOnDeviceCurrentPosChange(final Point3D pos) {
        final Intent intent = new Intent(ACTION_DEVICE_CURRENT_POS_CHANGE);
        intent.putExtra(EXTRA_CURRENT_POS, pos);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_STATUS_CHANGE.
     * 
     * @param e
     */
    private void fireOnDeviceStatusChange(final DeviceStatus status) {
        final Intent intent = new Intent(ACTION_DEVICE_STATUS_CHANGE);
        intent.putExtra(EXTRA_DEVICE_STATUS, status);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_DRAWING_ERROR.
     * 
     * @param e
     */
    void fireOnDrawingError(final Exception e) {
        final Intent intent = new Intent(ACTION_DEVICE_DRAWING_ERROR);
        intent.putExtra(EXTRA_EXCEPTION, e);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) - ACTION_DEVICE_PAUSED.
     * 
     * @param e
     */
    protected void fireOnDrawingPaused() {
        final Intent intent = new Intent(ACTION_DEVICE_DRAWING_PAUSED);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_RESUMED.
     * 
     * @param e
     */
    protected void fireOnDrawingResumed() {
        final Intent intent = new Intent(ACTION_DEVICE_DRAWING_RESUMED);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_DRAWING_UPDATE.
     * 
     * @param e
     */
    void fireOnDrawingUpdate(final Line2D line,
            final LineDrawingStatus lineStatus) {
        final Intent intent = new Intent(ACTION_DEVICE_DRAWING_UPDATE);
        intent.putExtra(EXTRA_LINE, line);
        intent.putExtra(EXTRA_LINE_STATUS, lineStatus);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_FINISH_DRAWING.
     * 
     * @param e
     */
    void fireOnFinishDrawing() {
        final Intent intent = new Intent(ACTION_DEVICE_FINISH_DRAWING);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_START_DRAWING.
     * 
     * @param e
     */
    void fireOnStartDrawing() {
        final Intent intent = new Intent(ACTION_DEVICE_START_DRAWING);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Сообщение об ошибке подключения к устройству (определено в случае
     * ConnectionStatus.ERROR).
     * 
     * @return
     */
    public String getConnectionErrorMessage() {
        return connectionErrorMessage;
    }

    /**
     * Информация о текущем подключении к устройству (определено в случае
     * ConnectionStatus.CONNECTED).
     * 
     * @return
     */
    public String getConnectionInfo() {
        return connectionInfo;
    }

    /**
     * Статус поключения к устройству.
     * 
     * @return
     */
    public ConnectionStatus getConnectionStatus() {
        return connectionStatus;
    }

    /**
     * Тип поключения к устройству.
     * 
     * @return
     */
    public String getConnectionType() {
        return "Напрямую через WiFi";
    }

    /**
     * Получить все отладочные сообщения.
     * 
     * @return
     */
    public String getDebugMessages() {
        return debugMessages.toString();
    }

    /**
     * Получить текущее положение рабочего блока на устройстве.
     * 
     * @return
     */
    public Point3D getDeviceCurrentPosition() {
        return deviceCurrentPosition;
    }

    /**
     * Получить словесное описание устройства.
     * 
     * @return
     */
    public String getDeviceDescription() {
        return deviceDescription;
    }

    /**
     * Сервис для управления процессом рисования на устройстве.
     * 
     * @return
     */
    public DeviceDrawingManager getDeviceDrawingManager() {
        return deviceDrawindManager;
    }

    /**
     * Получить производителя устройства.
     * 
     * @return
     */
    public String getDeviceManufacturer() {
        return deviceManufacturer;
    }

    /**
     * Получить модель устройства.
     * 
     * @return
     */
    public String getDeviceModel() {
        return deviceModel;
    }

    /**
     * Получить имя устройства.
     * 
     * @return
     */
    public String getDeviceName() {
        return deviceName;
    }

    /**
     * Получить серийный номер устройства.
     * 
     * @return
     */
    public String getDeviceSerialNumber() {
        return deviceSerialNumber;
    }

    /**
     * Статус устройства.
     * 
     * @return
     */
    public DeviceStatus getDeviceStatus() {
        return deviceStatus;
    }

    /**
     * Получить ссылку на страницу устройства.
     * 
     * @return
     */
    public String getDeviceUri() {
        return deviceUri;
    }

    /**
     * Получить версию программного обеспечения устройства.
     * 
     * @return
     */
    public String getDeviceVersion() {
        return deviceVersion;
    }

    /**
     * Получить размер рабочей области устройства.
     * 
     * @return
     */
    public Point3D getDeviceWorkingArea() {
        return deviceWorkingArea;
    }

    /**
     * Подключенное устройство тоже самое, что было подключено в прошлый раз.
     * 
     * @return
     */
    public boolean isSameDevice() {
        return isSameDevice;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return deviceControlBinder;
    }

    @Override
    public void onCreate() {
        System.out.println("DeviceControlService.onCreate()");
        super.onCreate();

        // инициировать промежуточные сервисы:
        deviceDrawindManager.onCreate();
        deviceStatusManager.onCreate();

        // попробуем подключиться к устройству
        connectToDeviceTcp();

        // Запустим бесконечный цикл отправки команд из очереди
        startDeviceOutputWriter();
    }

    @Override
    public void onDestroy() {
        System.out.println("DeviceControlService.onDestroy()");

        disconnectFromServer();

        super.onDestroy();
    }

    @Override
    public boolean onUnbind(Intent intent) {
        System.out.println("DeviceControlService.onUnbind()");
        return super.onUnbind(intent);
    }

    /**
     * Получить информацию об устройстве с устройства.
     * 
     * @return true, если подключенное устройство - то же устройство, которое
     *         было подключено в прошлый раз (совпадают значения всех
     *         информационных полей); false, если подключенное устройство -
     *         новое устройство (значения информационный полей отличаются).
     * @throws TimeoutException
     * @throws IOException
     */
    private boolean retrieveDeviceInfo() throws IOException, TimeoutException {
        // получить значения свойств с устройства
        final String name = execCommandOnDevice(DeviceProtocol.CMD_NAME);
        final String model = execCommandOnDevice(DeviceProtocol.CMD_MODEL);
        final String serialNumber = execCommandOnDevice(DeviceProtocol.CMD_SERIAL_NUMBER);
        final String description = execCommandOnDevice(DeviceProtocol.CMD_DESCRIPTION);
        final String version = execCommandOnDevice(DeviceProtocol.CMD_VERSION);
        final String manufacturer = execCommandOnDevice(DeviceProtocol.CMD_MANUFACTURER);
        final String uri = execCommandOnDevice(DeviceProtocol.CMD_URI);

        // проверить, является ли подключенное устройство тем же, что было
        // подключено в прошлый раз
        boolean isSameDevice = deviceName != null && deviceName.equals(name)
                && deviceModel != null && deviceModel.equals(model)
                && deviceSerialNumber != null
                && deviceSerialNumber.equals(serialNumber)
                && deviceDescription != null
                && deviceDescription.equals(description)
                && deviceVersion != null && deviceVersion.equals(version)
                && deviceManufacturer != null
                && deviceManufacturer.equals(manufacturer) && deviceUri != null
                && deviceUri.equals(uri);

        // обновить значения свойств устройства
        deviceName = name;
        deviceModel = model;
        deviceSerialNumber = serialNumber;
        deviceDescription = description;
        deviceVersion = version;
        deviceManufacturer = manufacturer;
        deviceUri = uri;

        return isSameDevice;
    }

    /**
     * Получить значение рабочей области устройства с устройства.
     * 
     * @return
     * @throws IOException
     * @throws TimeoutException
     */
    private void retrieveDeviceWorkingArea() throws IOException,
            TimeoutException {
        // получить значения свойств с устройства
        final String workingAreaStr = execCommandOnDevice(DeviceProtocol.CMD_RR_WORKING_AREA_DIM);

        // получить значения из строки
        final String[] wa_parts = workingAreaStr.split(" ");
        double max_x = Double.parseDouble(wa_parts[0]);
        double max_y = Double.parseDouble(wa_parts[1]);
        double max_z = Double.parseDouble(wa_parts[2]);

        // обновить значения свойств устройства
        deviceWorkingArea = new Point3D(max_x, max_y, max_z);
    }

    /**
     * Поставить команду в очередь для выполнения на сервере. При переполнении
     * очереди новые команды игнорируются (в простой реализации в очереди может
     * быть всего один элемент).
     * 
     * @param cmd
     * @param cmdListener
     * @return true, если команда успешно добавлена в очередь; false, если
     *         очередь переполнена и команда не может быть добавлена или
     *         отсутствует связь с устройством.
     */
    public boolean sendCommand(final String cmd,
            final CommandListener cmdListener) {
        if (connectionStatus == ConnectionStatus.CONNECTED
                && nextCommand == null) {
            nextCommand = new CommandInfo(cmd, cmdListener);
            debug("Accepted cmd: " + cmd);
            return true;
        } else {
            debug("Rejected cmd: " + cmd);
            return false;
        }
    }

    /**
     * Установить статус подключения, отправить широковещательные оповещения.
     * 
     * @param status
     */
    private void setConnectionStatus(final ConnectionStatus status) {
        if (this.connectionStatus != status) {
            this.connectionStatus = status;
            fireOnConnectionStatusChange(status);
        }
    }

    /**
     * Установить статус устройства, отправить широковещательные оповещения.
     * 
     * @param status
     */
    private void setDeviceStatus(final DeviceStatus status) {
        if (this.deviceStatus != status) {
            this.deviceStatus = status;
            fireOnDeviceStatusChange(status);
        }
    }

    /**
     * Фоновый поток отправки команд на устройство: получаем команду от
     * пользователя (в переменной nextCommand), отправляем на устройство, ждем
     * ответ, получаем ответ, сообщаем о результате, ждем следующую команду от
     * пользователя.
     */
    private void startDeviceOutputWriter() {
        new Thread() {
            @Override
            public void run() {
                CommandInfo execCommand = null;
                CommandListener execCommandListener = null;

                long lastCmdTime = System.currentTimeMillis();
                while (true) {
                    if (connectionStatus == ConnectionStatus.CONNECTED) {
                        // получим из "очереди" команду для выполнения на
                        // устройстве
                        execCommand = fetchCommand();
                        if (execCommand != null) {

                            execCommandListener = execCommand
                                    .getCommandListener();

                            try {
                                final String reply = execCommandOnDevice(execCommand
                                        .getCommand());

                                // отправим сообщение о выполнении команды
                                // подписанту
                                if (execCommandListener != null) {
                                    execCommandListener.onCommandExecuted(
                                            execCommand.getCommand(), reply);
                                }

                                lastCmdTime = System.currentTimeMillis();
                            } catch (final Exception e) {
                                debug("Connection error: " + e.getMessage());
                                e.printStackTrace();

                                // обрыв соединения - закроем потоки
                                // ввода/вывода,
                                // отменим все команды в "очереди" и дожемся
                                // нового
                                // подключения
                                disconnectFromServer();
                            }
                        } else if (System.currentTimeMillis() - lastCmdTime > DeviceConnectionWifi.MAX_IDLE_TIMEOUT) {
                            // поддерживать связь с устройством, если на
                            // него долго не отправляли команды - выполнять
                            // команду PING вне очереди
                            try {
                                execCommandOnDevice(DeviceProtocol.CMD_PING);
                                lastCmdTime = System.currentTimeMillis();
                            } catch (final Exception e) {
                                debug("Connection error: " + e.getMessage());
                                e.printStackTrace();

                                disconnectFromServer();
                            }
                        } else {
                            // на всякий случай - не будем напрягать систему
                            // холостыми циклами
                            try {
                                Thread.sleep(100);
                            } catch (InterruptedException e) {
                            }
                        }
                    } else {
                        // на всякий случай - не будем напрягать систему
                        // холостыми циклами
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                        }
                    }
                }
            }
        }.start();
    }

    /**
     * Обновить текущее положение печатного блока устройства.
     * 
     * @return
     */
    public boolean updateDeviceCurrentPosition() {
        return sendCommand(DeviceProtocol.CMD_RR_CURRENT_POSITION,
                deviceCurrentPositionCommandListener);
    }

    /**
     * Обновить актуальный внутренний статус устройства - добавить команду
     * CMD_RR_STATUS в очередь на выполнение.
     * 
     * @return true, если команда успешно добавлена в очередь false, если
     *         очередь переполнена и команда не может быть добавлена.
     */
    public boolean updateDeviceStatus() {
        return sendCommand(DeviceProtocol.CMD_RR_STATUS,
                deviceStatusCommandListener);
    }

    /**
     * Обновить текущий статус устройство и положение рабочего блока.
     * 
     * @return
     */
    public boolean updateDeviceStatusAndPosition() {
        return sendCommand(DeviceProtocol.CMD_RR_STATUS
                + DeviceProtocol.COMMAND_SEPARATOR
                + DeviceProtocol.CMD_RR_CURRENT_POSITION,
                new MultyCommandListener(new CommandListener[] {
                        deviceStatusCommandListener,
                        deviceCurrentPositionCommandListener }));
    }
}
