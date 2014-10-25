package com.rraptor.pult.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import com.rraptor.pult.comm.DeviceProtocol;
import com.rraptor.pult.core.DeviceControlService.CommandListener;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;
import com.rraptor.pult.core.DeviceControlService.DeviceStatus;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;
import com.rraptor.pult.view.PlotterAreaView.LineDrawingStatus;

public class DeviceDrawingManager {
    private final DeviceControlService devControlService;
    // TODO: доработать логику переподключения: сделать проверку
    // имени, модели и серийного номера устройств для предыдущего
    // и нового подключений - если все совпадает (возобновлена связь
    // с тем же устройством), разрешить продолжить
    // выполение старых команд, если не совпадают (подключились к
    // новому устройству), обнулить очередь.

    public BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onConnectionStatusChanged((DeviceControlService.ConnectionStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_CONNECTION_STATUS));
            }
        }
    };

    // Информация о процессе рисования
    private boolean isDrawing = false;
    private boolean isDrawingPaused = false;

    private boolean drawingCmdWaiting = false;
    private boolean drawingCmdError = false;
    private boolean resendDrawingCmd = false;

    // Информация о рисунке
    private final Map<Line2D, LineDrawingStatus> lineStatus = new HashMap<Line2D, LineDrawingStatus>();
    private final List<Line2D> drawingLines = new ArrayList<Line2D>();

    public DeviceDrawingManager(final DeviceControlService devControlService) {
        this.devControlService = devControlService;
    }

    /**
     * Выполнить команду на устройстве. Блокируется до тех пор, пока не будет
     * получен ответ о выполнении команды на устройстве или сообщение об ошибке
     * отправки команды на устройство или операция не будет прервана
     * пользователем.
     * 
     * @param cmd
     * @throws Exception
     */
    private void executeCommand(final String cmd) throws InterruptedException,
            Exception {
        resendDrawingCmd = true;
        while (resendDrawingCmd) {
            // Дождёмся момента, когда устройство закончит
            // выполнение предыдущей команды и будет готово
            // принять новую команду рисования. Эта проверка
            // не гарантирует надежность принятия команды
            // на устройстве, тк информация о статусе устройства
            // может не быть актуальной - если окажется, что устройство не
            // готово принять команду, оно просто вернет статус возврата BUSY и
            // мы повторим отправку команды.
            waitForDeviceStatusIdle();

            // TODO: не слишком хороший способ организовать выход из блокировки
            drawingCmdWaiting = devControlService.sendCommand(cmd,
                    new CommandListener() {
                        @Override
                        public void onCommandCanceled(final String cmd) {
                            // отмена отправки команды на устройство (например
                            // из-за обрыва соединения) - не повод отказываться
                            // от выполнения команды, это повод дождаться нового
                            // подключения и повторить попытку, если к тому
                            // времени рисование не будет отменено
                            resendDrawingCmd = true;
                            drawingCmdError = false;
                            drawingCmdWaiting = false;
                        }

                        @Override
                        public void onCommandExecuted(final String cmd,
                                final String reply) {
                            // убедимся, что команда была
                            // принята и приступила к исполнению
                            if (DeviceProtocol.REPLY_OK.equals(reply)) {
                                resendDrawingCmd = false;
                                drawingCmdError = false;
                            } else if (DeviceProtocol.REPLY_BUSY
                                    .equals(reply)) {
                                resendDrawingCmd = true;
                                drawingCmdError = false;
                            } else {
                                resendDrawingCmd = false;
                                drawingCmdError = true;
                            }
                            drawingCmdWaiting = false;
                        }
                    });
            // дождаться результата команды здесь
            while (isDrawing && drawingCmdWaiting) {
                Thread.sleep(100);
            }
            // обновим статус устройства до актуального состояния
            // (просто так, на логику работы это не повлияет)
            // TODO: ОТПРАВЛЯТЬ ЗАПРОС СТАТУСА вместе с командой G-кода в одном
            // пакете через точку с запятой, чтобы гарантированно иметь
            // обновленное значение статуса сразу после старта работы на
            // устройстве
            devControlService.updateDeviceStatus();

            // Проверим условия выхода
            if (!isDrawing) {
                throw new InterruptedException("Aborted");
            }
            if (drawingCmdError) {
                throw new Exception("Error while drawing");
            }
        }
    }

    /**
     * Получить линии рисунка в процессе рисования.
     * 
     * @return
     */
    public List<Line2D> getDrawingLines() {
        return drawingLines;
    }

    /**
     * Статус рисуемой линии.
     * 
     * @param line
     * @return
     */
    public LineDrawingStatus getLineStatus(final Line2D line) {
        final LineDrawingStatus status = lineStatus.get(line);
        return status == null ? LineDrawingStatus.NORMAL : status;
    }

    /**
     * Запущен ли процесс рисования.
     * 
     * @return
     */
    public boolean isDrawing() {
        return isDrawing;
    }

    /**
     * Статус очереди команд рисования для отправки на устройство.
     * 
     * @return true - отправка команд приостановлена; false - отправка команд
     *         работает.
     */
    public boolean isDrawingPaused() {
        return isDrawingPaused;
    }

    /**
     * Обновить статус подключения к устройству. Если нет подключения, поставить
     * рисование на паузу.
     * 
     * @param connectionStatus
     * 
     * @param status
     */
    protected void onConnectionStatusChanged(
            final ConnectionStatus connectionStatus) {
        if (connectionStatus != ConnectionStatus.CONNECTED && !isDrawingPaused) {
            pauseDrawing();
        }
    }

    /**
     * Вызывается при создании сервиса.
     */
    public void onCreate() {
        // получать события от сервиса
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        devControlService.registerReceiver(deviceBroadcastReceiver, filter);
    }

    /**
     * Приостановить процесс отправки команд рисования на устройство.
     */
    public void pauseDrawing() {
        devControlService.debug("DeviceDrawingManager: pauseDrawing");
        isDrawingPaused = true;
        devControlService.fireOnDrawingPaused();
    }

    /**
     * Сбросить статусы рисуемых линий.
     */
    private void resetLineStatus() {
        lineStatus.clear();
    }

    /**
     * Возобновить процесс отправки команд рисования на устройство.
     */
    public void resumeDrawing() {
        devControlService.debug("DeviceDrawingManager: resumeDrawing");
        isDrawingPaused = false;
        devControlService.fireOnDrawingResumed();
    }

    /**
     * Задать фигуру для рисования на устройстве. Фигуру нельзя поменять, если
     * процесс рисования уже запущен - нужно дождаться, пока он завершится или
     * будет прерван вызовом stopDrawingOnDevice.
     * 
     * @param drawingLines
     * @return true, если удалось задать фигуру; false, если процесс рисования
     *         уже запущен, изменить фигуру нельзя.
     */
    public boolean setDrawingLines(final List<Line2D> drawingLines) {
        if (isDrawing) {
            return false;
        } else {
            this.drawingLines.clear();
            if (drawingLines != null) {
                this.drawingLines.addAll(drawingLines);
            }
            return true;
        }
    }

    /**
     * Установить статус линии.
     * 
     * @param line
     * @param status
     */
    private void setLineStatus(final Line2D line, final LineDrawingStatus status) {
        lineStatus.put(line, status);
    }

    /**
     * Начать рисование фигуры в отдельном потоке. Этот поток живет параллельно
     * с основным потоком выполнения команд из DeviceControlService и добавляет
     * команды рисования в общую очередь в нужный момент. Новая команда
     * рисования добавляется в очередь после того, как завершился процесс
     * рисования для предыдущей команды. Пока идет процесс рисования, устройство
     * может принимать и отвечать на другие команды (например команды статуса и
     * остановки).
     * 
     * @param drawingLines
     */
    public void startDrawingOnDevice() {

        new Thread(new Runnable() {
            @Override
            public void run() {
                if (isDrawing) {
                    return;
                }
                devControlService
                        .debug("DeviceDrawingManager: startDrawingOnDevice");

                isDrawing = true;
                drawingCmdWaiting = false;
                drawingCmdError = false;
                resetLineStatus();
                devControlService.fireOnStartDrawing();
                resumeDrawing();

                String cmd;
                Line2D currentLine = null;

                try {
                    Point2D endPoint = null;
                    for (final Line2D line : DeviceDrawingManager.this.drawingLines) {
                        currentLine = line;
                        setLineStatus(line, LineDrawingStatus.DRAWING_PROGRESS);
                        devControlService.fireOnDrawingUpdate(line,
                                LineDrawingStatus.DRAWING_PROGRESS);

                        if (!line.getStart().equals(endPoint)) {

                            // поднимем блок
                            cmd = DeviceProtocol.CMD_GCODE_G0 + " " + "Z" + 5;
                            executeCommand(cmd);

                            // переместимся в начальную точку
                            cmd = DeviceProtocol.CMD_GCODE_G0 + " " + "X"
                                    + line.getStart().getX() + " " + "Y"
                                    + line.getStart().getY();
                            executeCommand(cmd);
                        }

                        // опустим блок на уровень для рисования
                        cmd = DeviceProtocol.CMD_GCODE_G01 + " " + "Z" + 0
                                + " F7.5";
                        executeCommand(cmd);

                        // прочертим линию со скоростью 2мм/с
                        cmd = DeviceProtocol.CMD_GCODE_G01 + " " + "X"
                                + line.getEnd().getX() + " " + "Y"
                                + line.getEnd().getY() + " F7.5";
                        executeCommand(cmd);

                        setLineStatus(line, LineDrawingStatus.DRAWN);
                        devControlService.fireOnDrawingUpdate(line,
                                LineDrawingStatus.DRAWN);

                        endPoint = line.getEnd();
                    }

                    // Дождаться выполнения последней команды
                    waitForDeviceStatusIdle();
                } catch (final Exception e) {
                    if (currentLine != null) {
                        setLineStatus(currentLine,
                                LineDrawingStatus.DRAWING_ERROR);
                        devControlService.fireOnDrawingUpdate(currentLine,
                                LineDrawingStatus.DRAWING_ERROR);
                    }
                    e.printStackTrace();
                    devControlService.fireOnDrawingError(e);
                }

                isDrawing = false;
                devControlService.fireOnFinishDrawing();
            }
        }).start();
    }

    /**
     * Прекратить процесс рисования на устройстве.
     */
    public void stopDrawingOnDevice() {
        // перестать пробовать отправлять новую команду рисования на устройство
        // - это приведет к выбросу AbortedException внутри executeCommand,
        // и поток внутри startDrawingOnDevice тоже завершится
        isDrawing = false;

        // остановить печатный блок на устройстве
        devControlService.sendCommand(DeviceProtocol.CMD_RR_STOP, null);
    }

    /**
     * Дождаться, пока устройство перейдет в состояние IDLE и при этом процесс
     * рисования не будет на паузе.
     * 
     * Возвращается после того, как первый раз поймает статус устройство IDLE.
     * Это не гарантирует того, что после этого вызова устройство не вернется в
     * состояние WORKING.
     * 
     * @throws InterruptedException
     *             рисование прекращено в процессе ожидания
     */
    private void waitForDeviceStatusIdle() throws InterruptedException {
        // Типа дождаться, что статус после предыдущей команды точно обновился
        // TODO: ОТПРАВЛЯТЬ ЗАПРОС СТАТУСА вместе с командой G-кода в одном
        // пакете через точку с запятой, чтобы гарантированно иметь обновленное
        // значение статуса сразу после старта работы на устройстве
        Thread.sleep(2000);
        while (isDrawing
                && (isDrawingPaused || devControlService.getDeviceStatus() != DeviceStatus.IDLE)) {
            Thread.sleep(100);
        }
        if (!isDrawing) {
            throw new InterruptedException("Aborted");
        }
    }
}
