package com.rraptor.pult.core;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;

public class DeviceStatusManager {

    private final int STATUS_UPDATE_TIMEOUT = 4000;

    private final DeviceControlService devControlService;

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

    private boolean isPolling = false;

    public DeviceStatusManager(final DeviceControlService devControlService) {
        this.devControlService = devControlService;
    }

    /**
     * Обновить статус подключения к устройству.
     * 
     * @param connectionStatus
     * 
     * @param status
     */
    protected void onConnectionStatusChanged(
            final ConnectionStatus connectionStatus) {
        if (connectionStatus == ConnectionStatus.CONNECTED) {
            startPollingDeviceStatus();
        } else {
            stopPollingDeviceStatus();
        }
    }

    public void onCreate() {
        // получать события от сервиса
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        devControlService.registerReceiver(deviceBroadcastReceiver, filter);
    }

    /**
     * Опрашивать устройство о статусе каждую секунду.
     * 
     * @param drawingLines
     */
    public void startPollingDeviceStatus() {
        devControlService
                .debug("DeviceStatusManager: startPollingDeviceStatus");

        new Thread(new Runnable() {
            @Override
            public void run() {
                if (isPolling) {
                    return;
                }
                isPolling = true;

                while (isPolling) {
                    devControlService.updateDeviceCurrentPosition();

                    try {
                        Thread.sleep(STATUS_UPDATE_TIMEOUT);
                    } catch (InterruptedException e) {
                    }
                    devControlService.updateDeviceStatus();
                    try {
                        Thread.sleep(STATUS_UPDATE_TIMEOUT);
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

        devControlService.debug("DeviceStatusManager: stopPollingDeviceStatus");
        isPolling = false;
    }
}
