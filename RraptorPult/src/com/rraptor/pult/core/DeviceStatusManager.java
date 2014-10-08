package com.rraptor.pult.core;


public class DeviceStatusManager {
    private final DeviceControlService devControlService;

    private boolean isPolling = false;

    public DeviceStatusManager(final DeviceControlService devControlService) {
        this.devControlService = devControlService;
    }

    /**
     * Опрашивать устройство о статусе каждую секунду.
     * 
     * @param drawingLines
     */
    public void startPollingDeviceStatus() {

        new Thread(new Runnable() {
            @Override
            public void run() {
                if (isPolling) {
                    return;
                }
                isPolling = true;

                while (isPolling) {
                    devControlService.updateDeviceStatus();

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                    }
                }
            }
        }).start();
    }

    /**
     * Прекратить процесс опроса устройства.
     */
    public void stopPollingDeviceStatus() {
        isPolling = false;
    }
}
