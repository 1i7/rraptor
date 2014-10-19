package com.rraptor.pult;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Handler;
import android.os.IBinder;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.rraptor.pult.core.DeviceControlService;
import com.rraptor.pult.core.DeviceControlService.ConnectionStatus;
import com.rraptor.pult.core.DeviceControlService.DeviceControlBinder;
import com.rraptor.pult.core.DeviceControlService.DeviceStatus;

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
                onConnectionStatusChanged((DeviceControlService.ConnectionStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_CONNECTION_STATUS));
            } else if (DeviceControlService.ACTION_DEVICE_STATUS_CHANGE
                    .equals(intent.getAction())) {
                onDeviceStatusChanged((DeviceControlService.DeviceStatus) intent
                        .getSerializableExtra(DeviceControlService.EXTRA_DEVICE_STATUS));
            }
        }
    };

    private DeviceControlService devControlService;
    private boolean isBound = false;

    private TextView txtStatus;
    private Button btnConnect;

    private final Handler handler = new Handler();

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
        txtStatus = (TextView) findViewById(R.id.txt_status);

        btnConnect = (Button) findViewById(R.id.btn_connect);
        btnConnect.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                // подключиться/переподключиться к устройству
                devControlService.connectToDeviceTcp();

            }
        });

        // register broadcast receiver
        final IntentFilter filter = new IntentFilter(
                DeviceControlService.ACTION_CONNECTION_STATUS_CHANGE);
        filter.addAction(DeviceControlService.ACTION_DEVICE_STATUS_CHANGE);
        registerReceiver(deviceBroadcastReceiver, filter);
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
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
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
     * Обновить статус устройства.
     * 
     * @param deviceStatus
     * 
     * @param status
     */
    protected void onDeviceStatusChanged(DeviceStatus deviceStatus) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                updateViews();
            }
        });
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

        switch (devControlService.getConnectionStatus()) {
        case DISCONNECTED:
            txtStatus.setText(R.string.status_disconnected);

            btnConnect.setVisibility(View.VISIBLE);
            btnConnect.setEnabled(true);

            break;
        case CONNECTED:
            txtStatus.setText(getString(R.string.status_connected) + ": "
                    + devControlService.getConnectionInfo());

            btnConnect.setVisibility(View.GONE);
            btnConnect.setEnabled(false);

            break;
        case CONNECTING:
            txtStatus.setText(R.string.status_connecting);

            btnConnect.setVisibility(View.VISIBLE);
            btnConnect.setEnabled(false);

            break;
        case ERROR:
            txtStatus.setText(getString(R.string.status_error) + ": "
                    + devControlService.getConnectionErrorMessage());

            btnConnect.setVisibility(View.VISIBLE);
            btnConnect.setEnabled(true);

            break;
        default:
            break;
        }

        if (devControlService.getConnectionStatus() == ConnectionStatus.CONNECTED) {
            switch (devControlService.getDeviceStatus()) {
            case IDLE:
                txtStatus.setTextColor(Color.GREEN);
                break;
            case WORKING:
                txtStatus.setTextColor(Color.RED);
                break;
            case UNKNOWN:
            default:
                txtStatus.setTextColor(Color.BLACK);
                break;
            }
        } else {
            txtStatus.setTextColor(Color.BLACK);
        }

        // if (ConnectionStatus.CONNECTED.equals(connectionStatus)) {
        // btnCmdLedOn.setEnabled(true);
        // btnCmdLedOff.setEnabled(true);
        // } else {
        // btnCmdLedOn.setEnabled(false);
        // btnCmdLedOff.setEnabled(false);
        // }
    }
}
