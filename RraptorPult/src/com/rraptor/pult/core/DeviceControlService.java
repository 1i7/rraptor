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

import com.rraptor.pult.Plotter2DCanvasView.LineDrawingStatus;
import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.comm.DeviceConnectionWifi;
import com.rraptor.pult.model.Line2D;

public class DeviceControlService extends Service {

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
        DISCONNECTED, CONNECTING, CONNECTED, ERROR
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

    // Публичные события
    // Подключение к устройству
    public static String ACTION_CONNECTION_STATUS_CHANGE = "com.rraptor.pult.CONNECTION_STATUS_CHANGE";
    public static String ACTION_DEVICE_STATUS_CHANGE = "com.rraptor.pult.DEVICE_STATUS_CHANGE";

    public static String EXTRA_CONNECTION_STATUS = "EXTRA_CONNECTION_STATUS";
    public static String EXTRA_DEVICE_STATUS = "EXTRA_DEVICE_STATUS";

    // Очередь команд
    public static String ACTION_DEVICE_PAUSED = "com.rraptor.pult.ACTION_DEVICE_PAUSED";
    public static String ACTION_DEVICE_RESUMED = "com.rraptor.pult.ACTION_DEVICE_RESUMED";

    // Отладка
    public static String ACTION_DEBUG_MESSAGE_POSTED = "com.rraptor.pult.ACTION_DEBUG_MESSAGE_POSTED";
    public static String EXTRA_DEBUG_MESSAGE = "EXTRA_DEBUG_MESSAGE";

    // Рисование
    public static String ACTION_DEVICE_START_DRAWING = "com.rraptor.pult.START_DRAWING";
    public static String ACTION_DEVICE_FINISH_DRAWING = "com.rraptor.pult.FINISH_DRAWING";
    public static String ACTION_DEVICE_DRAWING_UPDATE = "com.rraptor.pult.DRAWING_UPDATE";
    public static String ACTION_DEVICE_DRAWING_ERROR = "com.rraptor.pult.DRAWING_ERROR";

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

    /**
     * "Очередь" команд для выполнения на сервере, состоящая из одного элемента.
     */
    private String nextCommand;
    private CommandListener nextCommandListener;

    // Информация об устройстве
    private DeviceStatus deviceStatus = DeviceStatus.UNKNOWN;
    private final CommandListener deviceStatusCommandListener = new CommandListener() {

        @Override
        public void onCommandCanceled(final String cmd) {
            // ничего не делаем
        }

        @Override
        public void onCommandExecuted(final String cmd, final String reply) {
            if (DeviceConnection.CMD_RR_STATUS.equals(cmd)) {
                if (DeviceConnection.STATUS_IDLE.equals(reply)) {
                    setDeviceStatus(DeviceStatus.IDLE);
                } else if (DeviceConnection.STATUS_WORKING.equals(reply)) {
                    setDeviceStatus(DeviceStatus.WORKING);
                } else {
                    setDeviceStatus(DeviceStatus.UNKNOWN);
                }
            }
        }

    };

    // Информация об очереди команд
    private boolean isDevicePaused = false;

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
        if (nextCommandListener != null) {
            nextCommandListener.onCommandCanceled(nextCommand);
        }

        nextCommand = null;
        nextCommandListener = null;
    }

    /**
     * Очистить историю отладочных сообщений.
     */
    public void clearDebugMessages() {
        debugMessages.setLength(0);
    }

    /**
     * Подлключиться к устройству через канал Tcp и запустить (или возобновить)
     * процесс отправки команд.
     * 
     * @param cancelCommands
     *            true: отменить все ранее добавленные в очередь команды; false:
     *            продолжить выполнение команд, которые были добавлены в очередь
     *            во время предыдущего подключения
     */
    public void connectToDeviceTcp(boolean cancelCommands) {
        if (cancelCommands) {
            cancelCommands();
        }
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

                    debug("Connected");
                    connectionInfo = socket.getInetAddress().getHostName()
                            + ":" + socket.getPort();
                    setConnectionStatus(ConnectionStatus.CONNECTED);

                    // приступим к постоянному опросу устройства
                    deviceStatusManager.startPollingDeviceStatus();
                } catch (final Exception e) {
                    socket = null;
                    serverOut = null;
                    serverIn = null;

                    debug("Error connecting to server: " + e.getMessage());
                    setConnectionStatus(ConnectionStatus.ERROR);
                    connectionErrorMessage = e.getMessage();

                    e.printStackTrace();
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
        debugMessages.append(msg + "\n");
        fireOnDebugMessagePosted(msg);
    }

    /**
     * Отключиться от сервера - закрыть все потоки и сокет, обнулить переменные.
     */
    public void disconnectFromServer() {
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

            // остановить рисование
            deviceDrawindManager.stopDrawingOnDevice();
            deviceStatusManager.stopPollingDeviceStatus();

            debug("Disconnected");
            setConnectionStatus(ConnectionStatus.DISCONNECTED);
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
     * Отправить широковещательное сообщение (broadcast) - ACTION_DEVICE_PAUSED.
     * 
     * @param e
     */
    private void fireOnDevicePaused() {
        final Intent intent = new Intent(ACTION_DEVICE_PAUSED);
        getApplicationContext().sendBroadcast(intent);
    }

    /**
     * Отправить широковещательное сообщение (broadcast) -
     * ACTION_DEVICE_RESUMED.
     * 
     * @param e
     */
    private void fireOnDeviceResumed() {
        final Intent intent = new Intent(ACTION_DEVICE_RESUMED);
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
        return "Implement me: connection type";
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
    public String getDeviceCurrentPosition() {
        return "Implement me: device current position";
    }

    /**
     * Получить словесное описание устройства.
     * 
     * @return
     */
    public String getDeviceDescription() {
        return "Implement me: device description";
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
        return "Implement me: device manufacturer";
    }

    /**
     * Получить модель устройства.
     * 
     * @return
     */
    public String getDeviceModel() {
        return "Implement me: device model";
    }

    /**
     * Получить имя устройства.
     * 
     * @return
     */
    public String getDeviceName() {
        return "Implement me: device name";
    }

    /**
     * Получить серийный номер устройства.
     * 
     * @return
     */
    public String getDeviceSerialNumber() {
        return "Implement me: device serial number";
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
        return "Implement me: device uri";
    }

    /**
     * Получить версию программного обеспечения устройства.
     * 
     * @return
     */
    public String getDeviceVersion() {
        return "Implement me: device version";
    }

    /**
     * Получить размер рабочей области устройства.
     * 
     * @return
     */
    public String getDeviceWorkingArea() {
        return "Implement me: device working area";
    }

    /**
     * Статус очереди команд для отправки на устройство.
     * 
     * @return true - отправка команд приостановлена; false - отправка команд
     *         работает.
     */
    public boolean isDevicePaused() {
        return isDevicePaused;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return deviceControlBinder;
    }

    @Override
    public void onCreate() {
        System.out.println("DeviceControlService.onCreate()");
        super.onCreate();

        connectToDeviceTcp(true);

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
     * Приостановить процесс отправки команд на устройство.
     */
    public void pauseDevice() {
        isDevicePaused = true;
        fireOnDevicePaused();
    }

    /**
     * Возобновить процесс отправки команд на устройство.
     */
    public void resumeDevice() {
        isDevicePaused = false;
        fireOnDeviceResumed();
    }

    /**
     * Поставить комнаду в очередь для выполнения на сервере. При переполнении
     * очереди новые команды игнорируются. (в простой реализации в очереди может
     * быть всего один элемент).
     * 
     * @param cmd
     * @param cmdListener
     * @return true, если команда успешно добавлена в очередь; false, если
     *         очередь переполнена и команда не может быть добавлена.
     */
    public boolean sendCommand(final String cmd,
            final CommandListener cmdListener) {
        if (nextCommand == null) {
            nextCommand = cmd;
            this.nextCommandListener = cmdListener;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Установить статус подключения, отправить широковещательные оповещения.
     * 
     * @param status
     */
    private void setConnectionStatus(final ConnectionStatus status) {
        this.connectionStatus = status;
        fireOnConnectionStatusChange(status);
    }

    /**
     * Установить статус устройства, отправить широковещательные оповещения.
     * 
     * @param status
     */
    private void setDeviceStatus(final DeviceStatus status) {
        this.deviceStatus = status;
        fireOnDeviceStatusChange(status);
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
                String execCommand = null;

                long lastCmdTime = System.currentTimeMillis();
                while (true) {
                    if (connectionStatus == ConnectionStatus.CONNECTED
                            && !isDevicePaused && nextCommand != null) {
                        // получим из "очереди" команду для выполнения на
                        // устройстве
                        execCommand = nextCommand;

                        try {
                            final String reply = execCommandOnDevice(execCommand);

                            // отправим сообщение о выполнении команды
                            // подписанту
                            if (nextCommandListener != null) {
                                nextCommandListener.onCommandExecuted(
                                        execCommand, reply);
                            }

                            // очистим "очередь" - можно добавлять следующую
                            // команду
                            nextCommand = null;
                            nextCommandListener = null;

                            lastCmdTime = System.currentTimeMillis();
                        } catch (final Exception e) {
                            // обрыв соединения - не повод отказываться от
                            // выполнения команды, это повод дождаться нового
                            // подключения и повторить попытку, если к тому
                            // времени очередь не будет очищена извне

                            debug("Connection error: " + e.getMessage());
                            e.printStackTrace();

                            disconnectFromServer();
                        }
                    } else if (connectionStatus == ConnectionStatus.CONNECTED
                            && System.currentTimeMillis() - lastCmdTime > DeviceConnectionWifi.MAX_IDLE_TIMEOUT) {
                        // поддерживать связь с устройством, если на
                        // него долго не отправляли команды - выполнять
                        // команду PING вне очереди
                        try {
                            execCommandOnDevice(DeviceConnection.CMD_PING);
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
                }
            }
        }.start();
    }

    /**
     * Обновить актуальный внутренний статус устройства - добавить команду
     * CMD_RR_STATUS в очередь на выполнение.
     * 
     * @return true, если команда успешно добавлена в очередь false, если
     *         очередь переполнена и команда не может быть добавлена.
     */
    public boolean updateDeviceStatus() {
        return sendCommand(DeviceConnection.CMD_RR_STATUS,
                deviceStatusCommandListener);
    }
}
