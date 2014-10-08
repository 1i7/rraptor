package com.rraptor.pult.core;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.rraptor.pult.Plotter2DCanvasView.LineDrawingStatus;
import com.rraptor.pult.comm.DeviceConnection;
import com.rraptor.pult.core.DeviceControlService.CommandListener;
import com.rraptor.pult.core.DeviceControlService.DeviceStatus;
import com.rraptor.pult.model.Line2D;
import com.rraptor.pult.model.Point2D;

public class DeviceDrawingManager {
    private final DeviceControlService devControlService;

    // Информация о процессе рисования
    private boolean isDrawing = false;
    private final Map<Line2D, LineDrawingStatus> lineStatus = new HashMap<Line2D, LineDrawingStatus>();
    private boolean drawingCmdWaiting = false;
    private boolean drawingCmdError = false;
    private boolean resendDrawingCmd = false;

    public DeviceDrawingManager(final DeviceControlService devControlService) {
        this.devControlService = devControlService;
    }

    /**
     * Выполнить команду на устройстве. Блокируется до тех пор, пока не будет
     * получен ответ о выполнении команды на устройстве или сообщение об ошибке
     * отправки команды на устройство или операция не будет прервана
     * пользотелем.
     * 
     * @param cmd
     * @throws Exception
     */
    private void executeCommand(final String cmd) throws Exception {
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
            while (isDrawing
                    && (devControlService.getDeviceStatus() != DeviceStatus.IDLE)) {
                Thread.sleep(100);
            }
            if (!isDrawing) {
                throw new Exception("Aborted");
            }

            // TODO: не слишком хороший способ организовать выход из блокировки
            drawingCmdWaiting = devControlService.sendCommand(cmd,
                    new CommandListener() {
                        @Override
                        public void onCommandExecuted(final String cmd,
                                final String reply) {
                            // убедимся, что команда была
                            // принята и приступила к исполнению
                            if (DeviceConnection.REPLY_OK.equals(reply)) {
                                resendDrawingCmd = false;
                                drawingCmdError = false;
                            } else if (DeviceConnection.REPLY_BUSY
                                    .equals(reply)) {
                                resendDrawingCmd = true;
                                drawingCmdError = false;
                            } else {
                                resendDrawingCmd = false;
                                drawingCmdError = true;
                            }
                            drawingCmdWaiting = false;
                        }

                        @Override
                        public void onError(final String cmd, final Exception ex) {
                            resendDrawingCmd = false;
                            drawingCmdError = true;
                            drawingCmdWaiting = false;
                        }
                    });
            // дождаться результата команды здесь
            while (isDrawing && drawingCmdWaiting) {
                Thread.sleep(100);
            }
            // обновим статус устройства до актуального состояния
            // (просто так, на логику работы это не повлияет)
            devControlService.updateDeviceStatus();

            // Проверим условия выхода
            if (!isDrawing) {
                throw new Exception("Aborted");
            }
            if (drawingCmdError) {
                throw new Exception("Error while drawing");
            }
        }
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
     * Сбросить статусы рисуемых линий.
     */
    private void resetLineStatus() {
        lineStatus.clear();
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
     * Начать рисование двумерной фигуры в отдельном потоке. Этот поток живет
     * параллельно с основным потоком выполнения команд из DeviceControlService
     * и добавляет команды рисования в общую очередь в нужный момент. Новая
     * команда рисования добавляется в очередь после того, как завершился
     * процесс рисования для предыдущей команды. Пока идет процесс рисования,
     * устройство может принимать и отвечать на другие команды (например команды
     * статуса и остановки).
     * 
     * @param drawingLines
     */
    public void startDrawingOnDevice(final List<Line2D> drawingLines) {

        new Thread(new Runnable() {
            @Override
            public void run() {
                if (isDrawing) {
                    return;
                }

                isDrawing = true;
                drawingCmdWaiting = false;
                drawingCmdError = false;
                resetLineStatus();
                devControlService.fireOnStartDrawing();

                String cmd;
                Line2D currentLine = null;

                try {
                    Point2D endPoint = null;
                    for (final Line2D line : drawingLines) {
                        currentLine = line;
                        setLineStatus(line, LineDrawingStatus.DRAWING_PROGRESS);
                        devControlService.fireOnDrawingUpdate(line,
                                LineDrawingStatus.DRAWING_PROGRESS);

                        if (!line.getStart().equals(endPoint)) {

                            // поднимем блок
                            cmd = DeviceConnection.CMD_GCODE_G0 + " " + "Z" + 5;
                            executeCommand(cmd);

                            // переместимся в начальную точку
                            cmd = DeviceConnection.CMD_GCODE_G0 + " " + "X"
                                    + line.getStart().getX() + " " + "Y"
                                    + line.getStart().getY();
                            executeCommand(cmd);
                        }

                        // опустим блок на уровень для рисования
                        cmd = DeviceConnection.CMD_GCODE_G01 + " " + "Z" + 0
                                + " F7.5";
                        executeCommand(cmd);

                        // прочертим линию со скоростью 2мм/с
                        cmd = DeviceConnection.CMD_GCODE_G01 + " " + "X"
                                + line.getEnd().getX() + " " + "Y"
                                + line.getEnd().getY() + " F7.5";
                        executeCommand(cmd);

                        setLineStatus(line, LineDrawingStatus.DRAWN);
                        devControlService.fireOnDrawingUpdate(line,
                                LineDrawingStatus.DRAWN);

                        endPoint = line.getEnd();
                    }
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
        devControlService.sendCommand(DeviceConnection.CMD_RR_STOP, null);
        isDrawing = false;
    }
}
