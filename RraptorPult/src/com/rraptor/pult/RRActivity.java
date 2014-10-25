package com.rraptor.pult;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.view.View;
import android.widget.Toast;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;
import com.rraptor.pult.core.DeviceControlService.DeviceControlBinder;
import com.rraptor.pult.core.DeviceControlService.DeviceStatus;
import com.rraptor.pult.view.SystemStatusView;

public abstract class RRActivity extends Activity {

    /** Defines callbacks for service binding, passed to bindService() */
    private final ServiceConnection devServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to LocalService, cast the IBinder and get
            // LocalService instance
            DeviceControlBinder binder = (DeviceControlBinder) service;
            devControlService = binder.getService();
            isBound = true;

            onDeviceControlServiceConnected(devControlService);

            System.out.println(RRActivity.this.getClass().getName()
                    + ": Connected to DeviceControlService");
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            isBound = false;

            onDeviceControlServiceDisconnected(devControlService);

            System.out.println(RRActivity.this.getClass().getName()
                    + ": Disconnected from DeviceControlService");
        }
    };

    private final BroadcastReceiver deviceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onConnectionStatusChange((DeviceControlService.ConnectionStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_CONNECTION_STATUS));
            } else if (DeviceControlService.ACTION_DEVICE_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusChange((DeviceControlService.DeviceStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_DEVICE_STATUS));
            } else if (DeviceControlService.ACTION_DEVICE_START_DRAWING
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            } else if (DeviceControlService.ACTION_DEVICE_FINISH_DRAWING
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_ERROR
                    .equals(intent.getAction())) {
                final Exception e = (Exception) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_EXCEPTION);
                onDeviceDrawingError(e);
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_PAUSED
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            } else if (DeviceControlService.ACTION_DEVICE_DRAWING_RESUMED
                    .equals(intent.getAction())) {
                onDeviceDrawingStatusChange();
            }
        }
    };

    private DeviceControlService devControlService;
    private boolean isBound = false;

    private SystemStatusView pnlSystemStatus;

    /**
     * Напечатать отладочное сообщение.
     * 
     * @param msg
     */
    protected void debug(final String msg) {
        devControlService.debug(msg);
    }

    /**
     * Разорвать связь с устройством.
     */
    protected void disconnectDevice() {
        getDeviceControlService().disconnectFromServer();
    }

    /**
     * Ссылка на фоновый сервис, работающий с устройством.
     * 
     * @return
     */
    protected DeviceControlService getDeviceControlService() {
        return devControlService;
    }

    /**
     * Настроить элементы интерфейса - панель статуса подключения (вызывать из
     * onCreate после вызова setContentView)
     */
    protected void initViews() {
        pnlSystemStatus = (SystemStatusView) findViewById(R.id.rr_system_status);
        pnlSystemStatus.setConnectOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                // подключиться/переподключиться к устройству
                devControlService.connectToDeviceTcp();
            }
        });

        // зарегистрировать приёмник широковещательных сообщений (broadcast
        // receiver)
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_START_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_FINISH_DRAWING);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_ERROR);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_PAUSED);
        filter.addAction(DeviceControlService.ACTION_DEVICE_DRAWING_RESUMED);
        registerReceiver(deviceBroadcastReceiver, filter);
    }

    /**
     * Обновить статус подключения к устройству.
     * 
     * @param connectionStatus
     * 
     * @param status
     */
    protected void onConnectionStatusChange(
            final ConnectionStatus connectionStatus) {
        updateViews();
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(deviceBroadcastReceiver);
        super.onDestroy();
    }

    /**
     * Подключились к сервису управления устройством.
     */
    protected void onDeviceControlServiceConnected(
            final DeviceControlService service) {
        updateViews();
    }

    /**
     * Отключились от сервиса управления устройством.
     */
    protected void onDeviceControlServiceDisconnected(
            final DeviceControlService service) {
    }

    /**
     * Процесс рисования завершился с ошибкой
     * 
     * @param ex
     */
    private void onDeviceDrawingError(final Exception ex) {
        Toast.makeText(this, "Не получилось нарисовать: " + ex.getMessage(),
                Toast.LENGTH_LONG).show();
        updateViews();
    }

    /**
     * Обновить информацию о процессе рисования.
     */
    private void onDeviceDrawingStatusChange() {
        updateViews();
    }

    /**
     * Обновить статус устройства.
     * 
     * @param deviceStatus
     * 
     * @param status
     */
    protected void onDeviceStatusChange(final DeviceStatus deviceStatus) {
        updateViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStart() {
        super.onStart();
        // Bind to LocalService
        Intent intent = new Intent(this, DeviceControlService.class);
        startService(intent);
        bindService(intent, devServiceConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop() {
        super.onStop();
        // Unbind from the service
        if (isBound) {
            unbindService(devServiceConnection);
            isBound = false;
        }
    }

    /**
     * Обновить элементы управления в зависимости от текущего состояния.
     */
    private void updateViews() {
        if (devControlService != null) {
            pnlSystemStatus.setConnectionStatus(devControlService
                    .getConnectionStatus());

            switch (devControlService.getConnectionStatus()) {
            case CONNECTED:
                pnlSystemStatus.setConnectionInfo(devControlService
                        .getConnectionInfo());
                break;
            case CONNECTING:
            case HANDSHAKE:
            case DISCONNECTED:
                break;
            case ERROR:
                pnlSystemStatus.setConnectionErrorMessage(devControlService
                        .getConnectionErrorMessage());
                break;
            default:
                break;
            }

            if (devControlService.getConnectionStatus() == ConnectionStatus.CONNECTED) {
                pnlSystemStatus.setDrawingStatus(devControlService
                        .getDeviceDrawingManager().isDrawing(),
                        devControlService.getDeviceDrawingManager()
                                .isDrawingPaused());

                pnlSystemStatus.setDeviceStatus(devControlService
                        .getDeviceStatus());
            }
        }
    }
}
